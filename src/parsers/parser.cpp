// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/parser.h"

#include "cxx/parsers/restricted-parser.h"

#include "cxx/clang/clang-translation-unit.h"
#include "cxx/clang/clang-token.h"

#include "cxx/name_p.h"

#include "cxx/class.h"
#include "cxx/enum.h"
#include "cxx/function.h"
#include "cxx/function-body.h"
#include "cxx/namespace.h"
#include "cxx/statements.h"

#include "cxx/class-declaration.h"
#include "cxx/enum-declaration.h"
#include "cxx/function-declaration.h"
#include "cxx/namespace-declaration.h"
#include "cxx/template-declaration.h"
#include "cxx/variable-declaration.h"

#include "cxx/filesystem.h"
#include "cxx/program.h"

#include <algorithm>
#include <iostream>
#include <map>

namespace cxx
{

namespace parsers
{

struct StateGuard
{
  std::vector<std::shared_ptr<cxx::INode>>& m_stack;

  StateGuard(std::vector<std::shared_ptr<cxx::INode>>& stack, std::shared_ptr<cxx::INode> node)
    : m_stack(stack)
  {
    m_stack.push_back(node);
  }

  ~StateGuard()
  {
    m_stack.pop_back();
  }
};

template<typename T>
struct RAIIVectorSharedGuard
{
  std::vector<std::shared_ptr<T>>& target_;

  RAIIVectorSharedGuard(std::vector<std::shared_ptr<T>>& target, std::shared_ptr<T> elem)
    : target_(target)
  {
    target_.push_back(elem);
  }

  ~RAIIVectorSharedGuard()
  {
    target_.pop_back();
  }
};

template<typename T>
struct RAIIGuard
{
  T& target_;
  T value_;

  RAIIGuard(T& target)
    : target_(target), value_(target)
  {

  }

  ~RAIIGuard()
  {
    target_ = value_;
  }
};

struct StacksGuard
{
  RAIIVectorSharedGuard<cxx::INode> prog_;
  RAIIVectorSharedGuard<cxx::AstNode> ast_;

  StacksGuard(std::vector<std::shared_ptr<cxx::INode>>& p, std::vector<std::shared_ptr<cxx::AstNode>>& a, 
    std::shared_ptr<cxx::INode> p_elem, std::shared_ptr<cxx::AstNode> a_elem)
    : prog_(p, p_elem),
      ast_(a, a_elem)
  {

  }
};

LibClangParser::LibClangParser()
  : m_filesystem(FileSystem::GlobalInstance()),
    m_index{*this}
{
  m_program = std::make_shared<Program>();
}

LibClangParser::~LibClangParser()
{
}

LibClangParser::LibClangParser(FileSystem& fs)
  : m_filesystem(fs),
    m_index{ *this }
{
  m_program = std::make_shared<Program>();
}

LibClangParser::LibClangParser(std::shared_ptr<Program> prog)
  : m_filesystem(FileSystem::GlobalInstance()),
    m_index{ *this }
{
  m_program = prog;
}

LibClangParser::LibClangParser(std::shared_ptr<Program> prog, cxx::FileSystem& fs)
  : m_filesystem(fs),
    m_index{ *this }
{
  m_program = prog;
}

std::shared_ptr<Program> LibClangParser::program() const
{
  return m_program;
}

bool LibClangParser::parse(const std::string& file)
{
  this->skipped_declarations.clear();

  try
  {
    m_tu = m_index.parseTranslationUnit(file, includedirs, skip_function_bodies ? CXTranslationUnit_SkipFunctionBodies : CXTranslationUnit_None);
  }
  catch (...)
  {
    return false;
  }

  StateGuard stack_guard{ m_program_stack, m_program->globalNamespace() };

  m_tu_file = clang_getFile(m_tu, file.data());

  ClangCursor c = m_tu.getCursor();

  c.visitChildren([this](ClangCursor c) {
    visit_tu(c);
    });


  commitCurrentFile();

  return true;
}

cxx::AccessSpecifier LibClangParser::getAccessSpecifier(CX_CXXAccessSpecifier as)
{
  switch (as)
  {
  case CX_CXXAccessSpecifier::CX_CXXPublic:
    return cxx::AccessSpecifier::PUBLIC;
  case CX_CXXAccessSpecifier::CX_CXXProtected:
    return cxx::AccessSpecifier::PROTECTED;
  case CX_CXXAccessSpecifier::CX_CXXPrivate:
    return cxx::AccessSpecifier::PRIVATE;
  }

  return cxx::AccessSpecifier::PRIVATE;
}

std::shared_ptr<File> LibClangParser::getFile(const std::string& path)
{
  return m_filesystem.get(path);
}

void LibClangParser::commitCurrentFile()
{
  if (m_current_file)
  {
    m_parsed_files.insert(m_current_file);
    m_current_file->ast->updateSourceRange();
    m_current_file = nullptr;
    m_unlocated_nodes.clear();
  }
}

cxx::INode& LibClangParser::curNode()
{
  return *m_program_stack.back();
}

void LibClangParser::astWrite(std::shared_ptr<AstNode> n)
{
    cxx::AstNode& cur = *m_ast_stack.back();
    cur.append(n);
}

void LibClangParser::write(std::shared_ptr<IEntity> e)
{
  switch (curNode().kind())
  {
  case NodeKind::Namespace:
    static_cast<cxx::Namespace&>(curNode()).entities.push_back(e);
    break;
  case NodeKind::Class:
  case NodeKind::ClassTemplate:
    static_cast<cxx::Class&>(curNode()).members.push_back(e);
    break;
  case NodeKind::Enum:
    static_cast<cxx::Enum&>(curNode()).values.push_back(std::static_pointer_cast<cxx::EnumValue>(e));
    break;
  default:
    throw std::runtime_error{ "LibClangParser::write() failed" };
  }

  e->weak_parent = std::static_pointer_cast<cxx::IEntity>(curNode().shared_from_this());
}

static AstNodeKind convert_astnodekind(CXCursorKind k)
{
  switch (k)
  {
  case CXCursor_UnexposedDecl: return AstNodeKind::UnexposedDecl;
  case CXCursor_StructDecl: return AstNodeKind::StructDecl;
  case CXCursor_UnionDecl: return AstNodeKind::UnionDecl;
  case CXCursor_ClassDecl: return AstNodeKind::ClassDecl;
  case CXCursor_EnumDecl: return AstNodeKind::EnumDecl;
  case CXCursor_FieldDecl: return AstNodeKind::FieldDecl;
  case CXCursor_EnumConstantDecl: return AstNodeKind::EnumConstantDecl;
  case CXCursor_FunctionDecl: return AstNodeKind::FunctionDecl;
  case CXCursor_VarDecl: return AstNodeKind::VarDecl;
  case CXCursor_ParmDecl: return AstNodeKind::ParmDecl;
  case CXCursor_ObjCInterfaceDecl: return AstNodeKind::ObjCInterfaceDecl;
  case CXCursor_ObjCCategoryDecl: return AstNodeKind::ObjCCategoryDecl;
  case CXCursor_ObjCProtocolDecl: return AstNodeKind::ObjCProtocolDecl;
  case CXCursor_ObjCPropertyDecl: return AstNodeKind::ObjCPropertyDecl;
  case CXCursor_ObjCIvarDecl: return AstNodeKind::ObjCIvarDecl;
  case CXCursor_ObjCInstanceMethodDecl: return AstNodeKind::ObjCInstanceMethodDecl;
  case CXCursor_ObjCClassMethodDecl: return AstNodeKind::ObjCClassMethodDecl;
  case CXCursor_ObjCImplementationDecl: return AstNodeKind::ObjCImplementationDecl;
  case CXCursor_ObjCCategoryImplDecl: return AstNodeKind::ObjCCategoryImplDecl;
  case CXCursor_TypedefDecl: return AstNodeKind::TypedefDecl;
  case CXCursor_CXXMethod: return AstNodeKind::CXXMethod;
  case CXCursor_Namespace: return AstNodeKind::Namespace;
  case CXCursor_LinkageSpec: return AstNodeKind::LinkageSpec;
  case CXCursor_Constructor: return AstNodeKind::Constructor;
  case CXCursor_Destructor: return AstNodeKind::Destructor;
  case CXCursor_ConversionFunction: return AstNodeKind::ConversionFunction;
  case CXCursor_TemplateTypeParameter: return AstNodeKind::TemplateTypeParameter;
  case CXCursor_NonTypeTemplateParameter: return AstNodeKind::NonTypeTemplateParameter;
  case CXCursor_TemplateTemplateParameter: return AstNodeKind::TemplateTemplateParameter;
  case CXCursor_FunctionTemplate: return AstNodeKind::FunctionTemplate;
  case CXCursor_ClassTemplate: return AstNodeKind::ClassTemplate;
  case CXCursor_ClassTemplatePartialSpecialization: return AstNodeKind::ClassTemplatePartialSpecialization;
  case CXCursor_NamespaceAlias: return AstNodeKind::NamespaceAlias;
  case CXCursor_UsingDirective: return AstNodeKind::UsingDirective;
  case CXCursor_UsingDeclaration: return AstNodeKind::UsingDeclaration;
  case CXCursor_TypeAliasDecl: return AstNodeKind::TypeAliasDecl;
  case CXCursor_ObjCSynthesizeDecl: return AstNodeKind::ObjCSynthesizeDecl;
  case CXCursor_ObjCDynamicDecl: return AstNodeKind::ObjCDynamicDecl;
  case CXCursor_CXXAccessSpecifier: return AstNodeKind::CXXAccessSpecifier;
  //case CXCursor_FirstDecl: return AstNodeKind::FirstDecl;
  //case CXCursor_LastDecl: return AstNodeKind::LastDecl;
  //case CXCursor_FirstRef: return AstNodeKind::FirstRef;
  case CXCursor_ObjCSuperClassRef: return AstNodeKind::ObjCSuperClassRef;
  case CXCursor_ObjCProtocolRef: return AstNodeKind::ObjCProtocolRef;
  case CXCursor_ObjCClassRef: return AstNodeKind::ObjCClassRef;
  case CXCursor_TypeRef: return AstNodeKind::TypeRef;
  case CXCursor_CXXBaseSpecifier: return AstNodeKind::CXXBaseSpecifier;
  case CXCursor_TemplateRef: return AstNodeKind::TemplateRef;
  case CXCursor_NamespaceRef: return AstNodeKind::NamespaceRef;
  case CXCursor_MemberRef: return AstNodeKind::MemberRef;
  case CXCursor_LabelRef: return AstNodeKind::LabelRef;
  case CXCursor_OverloadedDeclRef: return AstNodeKind::OverloadedDeclRef;
  case CXCursor_VariableRef: return AstNodeKind::VariableRef;
  //case CXCursor_LastRef: return AstNodeKind::LastRef;
  //case CXCursor_FirstInvalid: return AstNodeKind::FirstInvalid;
  case CXCursor_InvalidFile: return AstNodeKind::InvalidFile;
  case CXCursor_NoDeclFound: return AstNodeKind::NoDeclFound;
  case CXCursor_NotImplemented: return AstNodeKind::NotImplemented;
  case CXCursor_InvalidCode: return AstNodeKind::InvalidCode;
  //case CXCursor_LastInvalid: return AstNodeKind::LastInvalid;
  //case CXCursor_FirstExpr: return AstNodeKind::FirstExpr;
  case CXCursor_UnexposedExpr: return AstNodeKind::UnexposedExpr;
  case CXCursor_DeclRefExpr: return AstNodeKind::DeclRefExpr;
  case CXCursor_MemberRefExpr: return AstNodeKind::MemberRefExpr;
  case CXCursor_CallExpr: return AstNodeKind::CallExpr;
  case CXCursor_ObjCMessageExpr: return AstNodeKind::ObjCMessageExpr;
  case CXCursor_BlockExpr: return AstNodeKind::BlockExpr;
  case CXCursor_IntegerLiteral: return AstNodeKind::IntegerLiteral;
  case CXCursor_FloatingLiteral: return AstNodeKind::FloatingLiteral;
  case CXCursor_ImaginaryLiteral: return AstNodeKind::ImaginaryLiteral;
  case CXCursor_StringLiteral: return AstNodeKind::StringLiteral;
  case CXCursor_CharacterLiteral: return AstNodeKind::CharacterLiteral;
  case CXCursor_ParenExpr: return AstNodeKind::ParenExpr;
  case CXCursor_UnaryOperator: return AstNodeKind::UnaryOperator;
  case CXCursor_ArraySubscriptExpr: return AstNodeKind::ArraySubscriptExpr;
  case CXCursor_BinaryOperator: return AstNodeKind::BinaryOperator;
  case CXCursor_CompoundAssignOperator: return AstNodeKind::CompoundAssignOperator;
  case CXCursor_ConditionalOperator: return AstNodeKind::ConditionalOperator;
  case CXCursor_CStyleCastExpr: return AstNodeKind::CStyleCastExpr;
  case CXCursor_CompoundLiteralExpr: return AstNodeKind::CompoundLiteralExpr;
  case CXCursor_InitListExpr: return AstNodeKind::InitListExpr;
  case CXCursor_AddrLabelExpr: return AstNodeKind::AddrLabelExpr;
  case CXCursor_StmtExpr: return AstNodeKind::StmtExpr;
  case CXCursor_GenericSelectionExpr: return AstNodeKind::GenericSelectionExpr;
  case CXCursor_GNUNullExpr: return AstNodeKind::GNUNullExpr;
  case CXCursor_CXXStaticCastExpr: return AstNodeKind::CXXStaticCastExpr;
  case CXCursor_CXXDynamicCastExpr: return AstNodeKind::CXXDynamicCastExpr;
  case CXCursor_CXXReinterpretCastExpr: return AstNodeKind::CXXReinterpretCastExpr;
  case CXCursor_CXXConstCastExpr: return AstNodeKind::CXXConstCastExpr;
  case CXCursor_CXXFunctionalCastExpr: return AstNodeKind::CXXFunctionalCastExpr;
  case CXCursor_CXXTypeidExpr: return AstNodeKind::CXXTypeidExpr;
  case CXCursor_CXXBoolLiteralExpr: return AstNodeKind::CXXBoolLiteralExpr;
  case CXCursor_CXXNullPtrLiteralExpr: return AstNodeKind::CXXNullPtrLiteralExpr;
  case CXCursor_CXXThisExpr: return AstNodeKind::CXXThisExpr;
  case CXCursor_CXXThrowExpr: return AstNodeKind::CXXThrowExpr;
  case CXCursor_CXXNewExpr: return AstNodeKind::CXXNewExpr;
  case CXCursor_CXXDeleteExpr: return AstNodeKind::CXXDeleteExpr;
  case CXCursor_UnaryExpr: return AstNodeKind::UnaryExpr;
  case CXCursor_ObjCStringLiteral: return AstNodeKind::ObjCStringLiteral;
  case CXCursor_ObjCEncodeExpr: return AstNodeKind::ObjCEncodeExpr;
  case CXCursor_ObjCSelectorExpr: return AstNodeKind::ObjCSelectorExpr;
  case CXCursor_ObjCProtocolExpr: return AstNodeKind::ObjCProtocolExpr;
  case CXCursor_ObjCBridgedCastExpr: return AstNodeKind::ObjCBridgedCastExpr;
  case CXCursor_PackExpansionExpr: return AstNodeKind::PackExpansionExpr;
  case CXCursor_SizeOfPackExpr: return AstNodeKind::SizeOfPackExpr;
  case CXCursor_LambdaExpr: return AstNodeKind::LambdaExpr;
  case CXCursor_ObjCBoolLiteralExpr: return AstNodeKind::ObjCBoolLiteralExpr;
  case CXCursor_ObjCSelfExpr: return AstNodeKind::ObjCSelfExpr;
  case CXCursor_OMPArraySectionExpr: return AstNodeKind::OMPArraySectionExpr;
  case CXCursor_ObjCAvailabilityCheckExpr: return AstNodeKind::ObjCAvailabilityCheckExpr;
  case CXCursor_FixedPointLiteral: return AstNodeKind::FixedPointLiteral;
  //case CXCursor_LastExpr: return AstNodeKind::LastExpr;
  //case CXCursor_FirstStmt: return AstNodeKind::FirstStmt;
  case CXCursor_UnexposedStmt: return AstNodeKind::UnexposedStmt;
  case CXCursor_LabelStmt: return AstNodeKind::LabelStmt;
  case CXCursor_CompoundStmt: return AstNodeKind::CompoundStmt;
  case CXCursor_CaseStmt: return AstNodeKind::CaseStmt;
  case CXCursor_DefaultStmt: return AstNodeKind::DefaultStmt;
  case CXCursor_IfStmt: return AstNodeKind::IfStmt;
  case CXCursor_SwitchStmt: return AstNodeKind::SwitchStmt;
  case CXCursor_WhileStmt: return AstNodeKind::WhileStmt;
  case CXCursor_DoStmt: return AstNodeKind::DoStmt;
  case CXCursor_ForStmt: return AstNodeKind::ForStmt;
  case CXCursor_GotoStmt: return AstNodeKind::GotoStmt;
  case CXCursor_IndirectGotoStmt: return AstNodeKind::IndirectGotoStmt;
  case CXCursor_ContinueStmt: return AstNodeKind::ContinueStmt;
  case CXCursor_BreakStmt: return AstNodeKind::BreakStmt;
  case CXCursor_ReturnStmt: return AstNodeKind::ReturnStmt;
  case CXCursor_GCCAsmStmt: return AstNodeKind::GCCAsmStmt;
  //case CXCursor_AsmStmt: return AstNodeKind::AsmStmt;
  case CXCursor_ObjCAtTryStmt: return AstNodeKind::ObjCAtTryStmt;
  case CXCursor_ObjCAtCatchStmt: return AstNodeKind::ObjCAtCatchStmt;
  case CXCursor_ObjCAtFinallyStmt: return AstNodeKind::ObjCAtFinallyStmt;
  case CXCursor_ObjCAtThrowStmt: return AstNodeKind::ObjCAtThrowStmt;
  case CXCursor_ObjCAtSynchronizedStmt: return AstNodeKind::ObjCAtSynchronizedStmt;
  case CXCursor_ObjCAutoreleasePoolStmt: return AstNodeKind::ObjCAutoreleasePoolStmt;
  case CXCursor_ObjCForCollectionStmt: return AstNodeKind::ObjCForCollectionStmt;
  case CXCursor_CXXCatchStmt: return AstNodeKind::CXXCatchStmt;
  case CXCursor_CXXTryStmt: return AstNodeKind::CXXTryStmt;
  case CXCursor_CXXForRangeStmt: return AstNodeKind::CXXForRangeStmt;
  case CXCursor_SEHTryStmt: return AstNodeKind::SEHTryStmt;
  case CXCursor_SEHExceptStmt: return AstNodeKind::SEHExceptStmt;
  case CXCursor_SEHFinallyStmt: return AstNodeKind::SEHFinallyStmt;
  case CXCursor_MSAsmStmt: return AstNodeKind::MSAsmStmt;
  case CXCursor_NullStmt: return AstNodeKind::NullStmt;
  case CXCursor_DeclStmt: return AstNodeKind::DeclStmt;
  case CXCursor_OMPParallelDirective: return AstNodeKind::OMPParallelDirective;
  case CXCursor_OMPSimdDirective: return AstNodeKind::OMPSimdDirective;
  case CXCursor_OMPForDirective: return AstNodeKind::OMPForDirective;
  case CXCursor_OMPSectionsDirective: return AstNodeKind::OMPSectionsDirective;
  case CXCursor_OMPSectionDirective: return AstNodeKind::OMPSectionDirective;
  case CXCursor_OMPSingleDirective: return AstNodeKind::OMPSingleDirective;
  case CXCursor_OMPParallelForDirective: return AstNodeKind::OMPParallelForDirective;
  case CXCursor_OMPParallelSectionsDirective: return AstNodeKind::OMPParallelSectionsDirective;
  case CXCursor_OMPTaskDirective: return AstNodeKind::OMPTaskDirective;
  case CXCursor_OMPMasterDirective: return AstNodeKind::OMPMasterDirective;
  case CXCursor_OMPCriticalDirective: return AstNodeKind::OMPCriticalDirective;
  case CXCursor_OMPTaskyieldDirective: return AstNodeKind::OMPTaskyieldDirective;
  case CXCursor_OMPBarrierDirective: return AstNodeKind::OMPBarrierDirective;
  case CXCursor_OMPTaskwaitDirective: return AstNodeKind::OMPTaskwaitDirective;
  case CXCursor_OMPFlushDirective: return AstNodeKind::OMPFlushDirective;
  case CXCursor_SEHLeaveStmt: return AstNodeKind::SEHLeaveStmt;
  case CXCursor_OMPOrderedDirective: return AstNodeKind::OMPOrderedDirective;
  case CXCursor_OMPAtomicDirective: return AstNodeKind::OMPAtomicDirective;
  case CXCursor_OMPForSimdDirective: return AstNodeKind::OMPForSimdDirective;
  case CXCursor_OMPParallelForSimdDirective: return AstNodeKind::OMPParallelForSimdDirective;
  case CXCursor_OMPTargetDirective: return AstNodeKind::OMPTargetDirective;
  case CXCursor_OMPTeamsDirective: return AstNodeKind::OMPTeamsDirective;
  case CXCursor_OMPTaskgroupDirective: return AstNodeKind::OMPTaskgroupDirective;
  case CXCursor_OMPCancellationPointDirective: return AstNodeKind::OMPCancellationPointDirective;
  case CXCursor_OMPCancelDirective: return AstNodeKind::OMPCancelDirective;
  case CXCursor_OMPTargetDataDirective: return AstNodeKind::OMPTargetDataDirective;
  case CXCursor_OMPTaskLoopDirective: return AstNodeKind::OMPTaskLoopDirective;
  case CXCursor_OMPTaskLoopSimdDirective: return AstNodeKind::OMPTaskLoopSimdDirective;
  case CXCursor_OMPDistributeDirective: return AstNodeKind::OMPDistributeDirective;
  case CXCursor_OMPTargetEnterDataDirective: return AstNodeKind::OMPTargetEnterDataDirective;
  case CXCursor_OMPTargetExitDataDirective: return AstNodeKind::OMPTargetExitDataDirective;
  case CXCursor_OMPTargetParallelDirective: return AstNodeKind::OMPTargetParallelDirective;
  case CXCursor_OMPTargetParallelForDirective: return AstNodeKind::OMPTargetParallelForDirective;
  case CXCursor_OMPTargetUpdateDirective: return AstNodeKind::OMPTargetUpdateDirective;
  case CXCursor_OMPDistributeParallelForDirective: return AstNodeKind::OMPDistributeParallelForDirective;
  case CXCursor_OMPDistributeParallelForSimdDirective: return AstNodeKind::OMPDistributeParallelForSimdDirective;
  case CXCursor_OMPDistributeSimdDirective: return AstNodeKind::OMPDistributeSimdDirective;
  case CXCursor_OMPTargetParallelForSimdDirective: return AstNodeKind::OMPTargetParallelForSimdDirective;
  case CXCursor_OMPTargetSimdDirective: return AstNodeKind::OMPTargetSimdDirective;
  case CXCursor_OMPTeamsDistributeDirective: return AstNodeKind::OMPTeamsDistributeDirective;
  case CXCursor_OMPTeamsDistributeSimdDirective: return AstNodeKind::OMPTeamsDistributeSimdDirective;
  case CXCursor_OMPTeamsDistributeParallelForSimdDirective: return AstNodeKind::OMPTeamsDistributeParallelForSimdDirective;
  case CXCursor_OMPTeamsDistributeParallelForDirective: return AstNodeKind::OMPTeamsDistributeParallelForDirective;
  case CXCursor_OMPTargetTeamsDirective: return AstNodeKind::OMPTargetTeamsDirective;
  case CXCursor_OMPTargetTeamsDistributeDirective: return AstNodeKind::OMPTargetTeamsDistributeDirective;
  case CXCursor_OMPTargetTeamsDistributeParallelForDirective: return AstNodeKind::OMPTargetTeamsDistributeParallelForDirective;
  case CXCursor_OMPTargetTeamsDistributeParallelForSimdDirective: return AstNodeKind::OMPTargetTeamsDistributeParallelForSimdDirective;
  case CXCursor_OMPTargetTeamsDistributeSimdDirective: return AstNodeKind::OMPTargetTeamsDistributeSimdDirective;
  //case CXCursor_LastStmt: return AstNodeKind::LastStmt;
  case CXCursor_TranslationUnit: return AstNodeKind::TranslationUnit;
  //case CXCursor_FirstAttr: return AstNodeKind::FirstAttr;
  case CXCursor_UnexposedAttr: return AstNodeKind::UnexposedAttr;
  case CXCursor_IBActionAttr: return AstNodeKind::IBActionAttr;
  case CXCursor_IBOutletAttr: return AstNodeKind::IBOutletAttr;
  case CXCursor_IBOutletCollectionAttr: return AstNodeKind::IBOutletCollectionAttr;
  case CXCursor_CXXFinalAttr: return AstNodeKind::CXXFinalAttr;
  case CXCursor_CXXOverrideAttr: return AstNodeKind::CXXOverrideAttr;
  case CXCursor_AnnotateAttr: return AstNodeKind::AnnotateAttr;
  case CXCursor_AsmLabelAttr: return AstNodeKind::AsmLabelAttr;
  case CXCursor_PackedAttr: return AstNodeKind::PackedAttr;
  case CXCursor_PureAttr: return AstNodeKind::PureAttr;
  case CXCursor_ConstAttr: return AstNodeKind::ConstAttr;
  case CXCursor_NoDuplicateAttr: return AstNodeKind::NoDuplicateAttr;
  case CXCursor_CUDAConstantAttr: return AstNodeKind::CUDAConstantAttr;
  case CXCursor_CUDADeviceAttr: return AstNodeKind::CUDADeviceAttr;
  case CXCursor_CUDAGlobalAttr: return AstNodeKind::CUDAGlobalAttr;
  case CXCursor_CUDAHostAttr: return AstNodeKind::CUDAHostAttr;
  case CXCursor_CUDASharedAttr: return AstNodeKind::CUDASharedAttr;
  case CXCursor_VisibilityAttr: return AstNodeKind::VisibilityAttr;
  case CXCursor_DLLExport: return AstNodeKind::DLLExport;
  case CXCursor_DLLImport: return AstNodeKind::DLLImport;
  //case CXCursor_LastAttr: return AstNodeKind::LastAttr;
  case CXCursor_PreprocessingDirective: return AstNodeKind::PreprocessingDirective;
  case CXCursor_MacroDefinition: return AstNodeKind::MacroDefinition;
  case CXCursor_MacroExpansion: return AstNodeKind::MacroExpansion;
  //case CXCursor_MacroInstantiation: return AstNodeKind::MacroInstantiation;
  case CXCursor_InclusionDirective: return AstNodeKind::InclusionDirective;
  //case CXCursor_FirstPreprocessing: return AstNodeKind::FirstPreprocessing;
  //case CXCursor_LastPreprocessing: return AstNodeKind::LastPreprocessing;
  case CXCursor_ModuleImportDecl: return AstNodeKind::ModuleImportDecl;
  case CXCursor_TypeAliasTemplateDecl: return AstNodeKind::TypeAliasTemplateDecl;
  case CXCursor_StaticAssert: return AstNodeKind::StaticAssert;
  case CXCursor_FriendDecl: return AstNodeKind::FriendDecl;
  //case CXCursor_FirstExtraDecl: return AstNodeKind::FirstExtraDecl;
  //case CXCursor_LastExtraDecl: return AstNodeKind::LastExtraDecl;
  case CXCursor_OverloadCandidate: return AstNodeKind::OverloadCandidate;
  default: return AstNodeKind::Root;
  }
}

std::shared_ptr<AstNode> LibClangParser::createAstNode(const ClangCursor& c)
{
  auto ret = std::make_shared<UnexposedAstNode>(convert_astnodekind(c.kind()));

  localize(ret, c);

  return ret;
}

void LibClangParser::localize(const std::shared_ptr<AstNode>& node, const ClangCursor& c)
{
  node->sourcerange = getCursorExtent(c);

  // It seems that for some headers in MSVC implementation of the standard library, 
  // some CXCursor_UnexposedExpr do not have any children nor a valid location.
  // We store these node in a vector so that it will be possible to later fix them.
  if (node->sourcerange.begin.line == -1)
    m_unlocated_nodes.push_back(node);
}

void LibClangParser::localizeParentize(const std::shared_ptr<AstNode>& node, const ClangCursor& c)
{
  localize(node, c);
  node->weak_parent = m_ast_stack.back();
}

void LibClangParser::bind(const std::shared_ptr<AstNode>& astnode, const std::shared_ptr<INode>& n)
{
  program()->astmap[n.get()] = astnode;
}

void LibClangParser::visit(const ClangCursor& cursor)
{
  CXCursorKind kind = cursor.kind();

  switch (kind)
  {
  case CXCursor_Namespace:
    return visit_namespace(cursor);
  case CXCursor_ClassDecl:
  case CXCursor_StructDecl:
  case CXCursor_ClassTemplate:
    return visit_class(cursor);
  case CXCursor_EnumDecl:
    return visit_enum(cursor);
  case CXCursor_CXXMethod:
  case CXCursor_FunctionDecl:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return visit_function(cursor);
  case CXCursor_EnumConstantDecl:
    return visit_enumconstant(cursor);
  case CXCursor_VarDecl:
    return visit_vardecl(cursor);
  case CXCursor_FieldDecl:
    return visit_fielddecl(cursor);
  case CXCursor_CXXAccessSpecifier:
    return visit_accessspecifier(cursor);
  case CXCursor_TemplateTypeParameter:
    return visit_template_type_parameter(cursor);
  default:
    return visit_unexposed(cursor);
  }
}

void LibClangParser::visit_tu(const ClangCursor& cursor)
{
  // We are working at translation-unit level

  auto cursor_file = getCursorFile(cursor);

  if (!clang_File_isEqual(m_current_cxfile, cursor_file))
  {
    // We have reached another file, let's see if it has already been parsed
    auto file = getFile(getCursorFilePath(cursor));

    if (m_parsed_files.find(file) != m_parsed_files.end())
    {
      // The file has already been parsed, we skip until we reach a non-parsed file
      return;
    }

    commitCurrentFile();

    m_current_cxfile = cursor_file;
    m_current_file = file;

    if (m_current_file->ast == nullptr)
      m_current_file->ast = std::make_shared<AstRootNode>();

    assert(m_ast_stack.size() <= 1);
    m_ast_stack.clear();
    m_ast_stack.push_back(m_current_file->ast);
  }

  return visit(cursor);
}

void LibClangParser::visit_namespace(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();
  auto entity = static_cast<Namespace*>(m_program_stack.back().get())->getOrCreateNamespace(name);

  auto decl = std::make_shared<NamespaceDeclaration>(entity);
  localizeParentize(decl, cursor);
  astWrite(decl);
  
  {
    RAIIVectorSharedGuard<cxx::AstNode> astguard{ m_ast_stack, decl };
    RAIIVectorSharedGuard<cxx::INode> guard{ m_program_stack, entity };

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_class(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();

  const bool is_template = cursor.kind() == CXCursor_ClassTemplate;

  auto entity = [&]() -> std::shared_ptr<Class> {
    if (curNode().is<Namespace>())
    {
      auto& ns = static_cast<Namespace&>(curNode());

      if (is_template)
        return ns.getOrCreate<ClassTemplate>(name, std::vector<std::shared_ptr<TemplateParameter>>(), std::move(name));
      else
        return ns.getOrCreate<Class>(name, std::move(name));
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      std::shared_ptr<Class> result = !is_template ? (std::make_shared<Class>(std::move(name), cla.shared_from_this())) :
        (std::make_shared<ClassTemplate>(std::vector<std::shared_ptr<TemplateParameter>>(), std::move(name), cla.shared_from_this()));
      result->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(result);
      return result;
    }
  }();

  auto decl = std::make_shared<ClassDeclaration>(entity);
  localizeParentize(decl, cursor);
  astWrite(decl);

  if (isForwardDeclaration(cursor))
    return;

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, decl };

  bind(decl, entity);

  m_cursor_entity_map[cursor] = entity;

  cxx::AccessSpecifier default_access = [&]() {
    if (clang_getCursorKind(cursor) == CXCursor_StructDecl)
      return cxx::AccessSpecifier::PUBLIC;
    else
      return cxx::AccessSpecifier::PRIVATE;
  }();

  {
    RAIIVectorSharedGuard<cxx::INode> guard{ m_program_stack, entity };
    RAIIGuard<cxx::AccessSpecifier> access_guard{ m_access_specifier };

    m_access_specifier = default_access;

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_enum(const ClangCursor& cursor)
{
  std::string name = getCursorSpelling(cursor);

  std::shared_ptr<Enum> entity = [&]() {
    if (curNode().is<Namespace>())
      return static_cast<Namespace&>(curNode()).createEnum(name);

    Class& cla = static_cast<Class&>(curNode());
    auto result = std::make_shared<Enum>(std::move(name), cla.shared_from_this());
    result->enum_class = clang_EnumDecl_isScoped(cursor);
    result->setAccessSpecifier(m_access_specifier);
    cla.members.push_back(result);
    return result;
  }();

  auto decl = std::make_shared<EnumDeclaration>(entity);
  localizeParentize(decl, cursor);
  astWrite(decl);

  bind(decl, entity);

  {
    RAIIVectorSharedGuard<cxx::AstNode> astguard{ m_ast_stack, decl };
    RAIIVectorSharedGuard<cxx::INode> guard{ m_program_stack, entity };

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_enumconstant(const ClangCursor& cursor)
{
  std::string n = cursor.getSpelling();

  auto& en = static_cast<Enum&>(curNode());
  auto val = std::make_shared<EnumValue>(std::move(n), std::static_pointer_cast<cxx::Enum>(en.shared_from_this()));
  en.values.push_back(val);

  auto decl = std::make_shared<EnumeratorDeclaration>(val);
  localizeParentize(decl, cursor);
  astWrite(decl);

  bind(decl, val);
}

static bool are_equiv_param(const cxx::FunctionParameter& a, const cxx::FunctionParameter& b)
{
  return a.type == b.type;
}

static bool are_equiv_func(const cxx::Function& a, const cxx::Function& b)
{
  if (a.name != b.name)
    return false;

  if (a.parameters.size() != b.parameters.size())
    return false;

  if (a.return_type != b.return_type)
    return false;

  for (size_t i(0); i < a.parameters.size(); ++i)
  {
    if (!are_equiv_param(*a.parameters.at(i), *b.parameters.at(i)))
      return false;
  }

  return true;
}

static std::shared_ptr<cxx::Function> find_equiv_func(cxx::INode& current_node, const cxx::Function& func)
{
  if (current_node.is<cxx::Class>())
  {
    for (const auto& m : static_cast<cxx::Class&>(current_node).members)
    {
      if (m->is<cxx::Function>())
      {
        if (are_equiv_func(static_cast<cxx::Function&>(*m), func))
          return std::static_pointer_cast<cxx::Function>(m);
      }
    }
  }
  else if(current_node.is<cxx::Namespace>())
  { 
    for (const auto& m : static_cast<cxx::Namespace&>(current_node).entities)
    {
      if (m->is<cxx::Function>())
      {
        if (are_equiv_func(static_cast<cxx::Function&>(*m), func))
          return std::static_pointer_cast<cxx::Function>(m);
      }
    }
  }

  return nullptr;
}

static void update_func(cxx::Function& func, const cxx::Function& new_one)
{
  for (size_t i(0); i < func.parameters.size(); ++i)
  {
    if (func.parameters.at(i)->default_value == cxx::Expression())
    {
      if (new_one.parameters.at(i)->default_value != cxx::Expression())
        func.parameters.at(i)->default_value = new_one.parameters.at(i)->default_value;
    }
  }

  if (func.body.isNull() && !new_one.body.isNull())
  {
    func.body = new_one.body;
  }
}

void LibClangParser::visit_function(const ClangCursor& cursor)
{
  // Tricky:
  // We may be dealing with a forward declaration, and the body may not be available.
  // We must create the Function nonetheless, even without the body.
  // Further declarations may provide the body or additional default parameters.

  auto decl = std::make_shared<FunctionDeclaration>();
  localizeParentize(decl, cursor);
  astWrite(decl);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, decl };

  std::shared_ptr<Function> entity;

  try
  {
    entity = parseFunction(cursor);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
    return;
  }

  decl->entity_ptr = entity;

  bool is_member = [&]() {
    switch (cursor.kind())
    {
    case CXCursor_CXXMethod:
    case CXCursor_Constructor:
    case CXCursor_Destructor:
      return true;
    default:
      return false;
    }
  }();

  auto semantic_parent = [&]() -> std::shared_ptr<cxx::INode> {
    if (is_member)
    {
      ClangCursor parent = cursor.getSemanticParent();
      auto it = m_cursor_entity_map.find(parent);

      if (it == m_cursor_entity_map.end())
        return nullptr;

      return it->second;
    }
    else
    {
      return curNode().shared_from_this();
    }
  }();

  if (semantic_parent == nullptr)
  {
    // @TODO: do not silently ignore this declaration
    return;
  }

  auto func = find_equiv_func(*semantic_parent, *entity);

  if (!func)
  {
    entity->weak_parent = std::static_pointer_cast<cxx::IEntity>(curNode().shared_from_this());

    if (curNode().is<Namespace>())
    {
      static_cast<Namespace&>(curNode()).entities.push_back(entity);
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      entity->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(entity);
    }

    func = entity;
  }
  else
  {
    decl->entity_ptr = func;
    update_func(*func, *entity);
  }

  if (!isForwardDeclaration(cursor))
    bind(decl, func);
}

void LibClangParser::visit_vardecl(const ClangCursor& cursor)
{
  auto decl = std::make_shared<VariableDeclaration>();
  localizeParentize(decl, cursor);
  astWrite(decl);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, decl };

  std::shared_ptr<Variable> var;

  try
  {
    var = parseVariable(cursor);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    // Example of declaration that fails:
    // const typename _Iosb<_Dummy>::_Fmtflags count_ = 8;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
    return;
  }

  decl->entity_ptr = var;

  if (curNode().is<cxx::Class>())
  {
    // @TODO: allow access specifier on Variable
    // var->setAccessSpecifier(m_access_specifier);

    var->specifiers() |= VariableSpecifier::Static;

    write(var);
  }
  else if (curNode().is<cxx::Namespace>())
  {
    write(var);
  }
  else
  {
    // @TODO ?
  }

  bind(decl, var);
}

void LibClangParser::visit_fielddecl(const ClangCursor& cursor)
{
  assert(curNode().is<cxx::Class>());

  auto decl = std::make_shared<VariableDeclaration>();
  localizeParentize(decl, cursor);
  astWrite(decl);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, decl };

  try
  {
    auto var = parseVariable(cursor);
    write(var);
    decl->entity_ptr = var;
    bind(decl, var);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    // Example of field that will fail:
    // struct { int n; } my_field;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
  }
}

void LibClangParser::visit_accessspecifier(const ClangCursor& cursor)
{
  ClangTokenSet tokens = m_tu.tokenize(cursor.getExtent());

  CX_CXXAccessSpecifier aspec = cursor.getCXXAccessSpecifier();

  m_access_specifier = getAccessSpecifier(aspec);

  auto astnode = createAstNode(cursor);
  localizeParentize(astnode, cursor);
  astWrite(astnode);
}

void LibClangParser::visit_template_type_parameter(const ClangCursor& cursor)
{
  if (!curNode().is<ClassTemplate>())
    throw std::runtime_error{ "Not implemented" };

  auto decl = std::make_shared<TemplateParameterDeclaration>();
  localizeParentize(decl, cursor);
  astWrite(decl);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, decl };

  ClassTemplate& ct = static_cast<ClassTemplate&>(curNode());

  auto ttparam = std::make_shared<TemplateParameter>(cursor.getSpelling());
  ttparam->weak_parent = ct.shared_from_this();
  ct.template_parameters.push_back(ttparam);

  decl->entity_ptr = ttparam;

  bind(decl, ttparam);
}

void LibClangParser::visit_unexposed(const ClangCursor& cursor)
{
  std::shared_ptr<AstNode> astnode = createAstNode(cursor);
  astWrite(astnode);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, astnode };

  // The following code is commented-out to keep the behaviour as close as before.
  //cursor.visitChildren([this](const ClangCursor& cc) {
  //  this->visit_unexposed(cc);
  //  });
}

std::shared_ptr<cxx::Variable> LibClangParser::parseVariable(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();
  Type type = parseType(cursor.getType());

  auto parent = std::static_pointer_cast<cxx::IEntity>(curNode().shared_from_this());
  auto var = std::make_shared<Variable>(type, std::move(name), parent);

  if (cursor.childCount() == 1)
    var->defaultValue() = parseExpression(cursor.childAt(0));

  return var;
}

std::shared_ptr<cxx::Function> LibClangParser::parseFunction(const ClangCursor& cursor)
{
  auto func = std::make_shared<cxx::Function>(getCursorSpelling(cursor), std::dynamic_pointer_cast<cxx::IEntity>(m_program_stack.back()));

  RAIIVectorSharedGuard<cxx::INode> guard{ m_program_stack, func };

  CXCursorKind kind = clang_getCursorKind(cursor);

  if (kind == CXCursor_Constructor)
  {
    func->kind = FunctionKind::Constructor;

    // @TODO: handle 'explicit' specifier
  }
  else if (kind == CXCursor_Destructor)
  {
    func->kind = FunctionKind::Destructor;
  }
  else
  {
    if (clang_CXXMethod_isConst(cursor))
      func->specifiers |= FunctionSpecifier::Const;
    if (clang_CXXMethod_isStatic(cursor))
      func->specifiers |= FunctionSpecifier::Static;
    if (clang_CXXMethod_isVirtual(cursor))
      func->specifiers |= FunctionSpecifier::Virtual;
    if (clang_CXXMethod_isPureVirtual(cursor))
      func->specifiers |= FunctionSpecifier::Pure;
  }

  int exception_spec_type = cursor.getExceptionSpecificationType();

  if (exception_spec_type == CXCursor_ExceptionSpecificationKind_BasicNoexcept)
    func->specifiers |= FunctionSpecifier::Noexcept;

  CXType function_type = clang_getCursorType(cursor);

  if (kind != CXCursor_Constructor && kind != CXCursor_Destructor)
  {
    func->return_type = parseType(clang_getResultType(function_type));
  }
  else
  {
    func->return_type = Type::Void;
  }

  const int num_args = clang_Cursor_getNumArguments(cursor);

  for (int i = 0; i < num_args; ++i)
  {
    CXCursor arg_cursor = clang_Cursor_getArgument(cursor, i);
    parseFunctionArgument(*func, arg_cursor);
  }

  // The children of a function seems to appear in the following order:
  // - return type (optional)
  // - parameters (CXCursor_ParmDecl)
  // - body (CXCursor_CompoundStmt)

  // @TODO: use the following visitChildren to parse the parameters,
  // for the return type it is more complicated as it does not always appear
  // in the children...
  cursor.visitChildren([&](const ClangCursor& c) {

    if (c.kind() == CXCursor_CompoundStmt)
      func->body = parseFunctionBody(func, c);

    });
  
  return func;
}

void LibClangParser::parseFunctionArgument(cxx::Function& func, CXCursor cursor)
{
  Type t = parseType(clang_getCursorType(cursor));
  std::string name = getCursorSpelling(cursor);

  auto param = std::make_shared<cxx::FunctionParameter>(t, std::move(name), std::static_pointer_cast<cxx::Function>(func.shared_from_this()));

  // Parse default-argument
  {
    std::string default_value_expr;

    auto data = std::make_pair(this, &default_value_expr);
    clang_visitChildren(cursor, &LibClangParser::param_decl_visitor, &data);

    if (!default_value_expr.empty())
      param->default_value = Expression{ std::move(default_value_expr) };
  }

  func.parameters.push_back(param);
}

CXChildVisitResult LibClangParser::param_decl_visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
  std::pair<LibClangParser*, std::string*>* p = (std::pair<LibClangParser*, std::string*>*) data;
  return p->first->visitFunctionParamDecl(cursor, parent, *(p->second));
}

CXChildVisitResult LibClangParser::visitFunctionParamDecl(CXCursor cursor, CXCursor parent, std::string& param)
{
  std::string spelling = getCursorSpelling(cursor);

  if (spelling.empty())
  {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXToken* tokens = 0;
    unsigned int nTokens = 0;
    clang_tokenize(m_tu, range, &tokens, &nTokens);
    for (unsigned int i = 0; i < nTokens; i++)
    {
      spelling += getTokenSpelling(m_tu, tokens[i]);
    }
    clang_disposeTokens(m_tu, tokens, nTokens);
  }

  param += spelling;

  return CXChildVisit_Continue;
}

Statement LibClangParser::parseStatement(const ClangCursor& c)
{
  CXCursorKind k = c.kind();

  switch (k)
  {
  case CXCursor_NullStmt:
    return parseNullStatement(c);
  case CXCursor_CompoundStmt:
    return parseCompoundStatement(c);
  case CXCursor_IfStmt:
    return parseIf(c);
  case CXCursor_WhileStmt:
    return parseWhile(c);
  default:
    return parseUnexposedStatement(c);
  }
}

Statement LibClangParser::parseFunctionBody(std::shared_ptr<cxx::Function> f, const ClangCursor& c)
{
  auto astnode = std::make_shared<FunctionBody>(f);
  localizeParentize(astnode, c);
  astWrite(astnode);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, astnode };

  c.visitChildren([&](const ClangCursor& child) {
    astnode->statements.push_back(parseStatement(child));
    });

  return { astnode };
}

std::shared_ptr<cxx::IStatement> LibClangParser::parseNullStatement(const ClangCursor& c)
{
  auto result = std::make_shared<NullStatement>();
  localizeParentize(result, c);
  return result;
}

std::shared_ptr<cxx::IStatement> LibClangParser::parseCompoundStatement(const ClangCursor& c)
{
  auto result = std::make_shared<CompoundStatement>();
  localizeParentize(result, c);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, result };

  c.visitChildren([&](const ClangCursor& child) {
    result->statements.push_back(parseStatement(child));
    });

  return result;
}

std::shared_ptr<cxx::IStatement> LibClangParser::parseIf(const ClangCursor& c)
{
  auto result = std::make_shared<IfStatement>();

  localizeParentize(result, c);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, result };

  // The children if a CXCursor_IfStmt seems to appear in the following order:
  // - condition
  // - body
  // - else-clause (optional)

  enum IfParsingState
  {
    IfParsing_Cond,
    IfParsing_Body,
    IfParsing_Else,
  };

  IfParsingState state = IfParsing_Cond;

  c.visitChildren([&](const ClangCursor& child) {

    if (state == IfParsing_Cond)
    {
      result->condition = parseExpression(child);
      state = IfParsing_Body;
    }
    else if (state == IfParsing_Body)
    {
      result->body = parseStatement(child);
      state = IfParsing_Else;
    }
    else if (state == IfParsing_Else)
    {
      result->else_clause = parseStatement(child);
    }

    });

  return result;
}

std::shared_ptr<cxx::IStatement> LibClangParser::parseWhile(const ClangCursor& c)
{
  auto result = std::make_shared<WhileLoop>();

  localizeParentize(result, c);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, result };

  // The children if a CXCursor_WhileStmt seems to appear in the following order:
  // - condition
  // - body

  enum WhileParsingState
  {
    WhileParsing_Cond,
    WhileParsing_Body,
  };

  WhileParsingState state = WhileParsing_Cond;

  c.visitChildren([&](const ClangCursor& child) {

    if (state == WhileParsing_Cond)
    {
      result->condition = parseExpression(child);
    }
    else if (state == WhileParsing_Body)
    {
      result->body = parseStatement(child);
    }

    });

  return result;
}

std::shared_ptr<cxx::IStatement> LibClangParser::parseUnexposedStatement(const ClangCursor& c)
{
  auto result = std::make_shared<UnexposedStatement>(convert_astnodekind(c.kind()));
 
  localizeParentize(result, c);

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, result };

  c.visitChildren([&](const ClangCursor& child) {
    result->childvec.push_back(parseUnexposedStatement(child));
    });

  return result;
}

std::string LibClangParser::getSpelling(const ClangTokenSet& tokens)
{
  if (tokens.size() == 0)
    return std::string();

  std::string result = tokens.at(0).getSpelling();

  CXSourceRange range = tokens.at(0).getExtent();
  cxx::SourceLocation loc = getLocation(clang_getRangeEnd(range));

  for (size_t i = 1; i < tokens.size(); i++)
  {
    range = tokens.at(i).getExtent();
    cxx::SourceLocation tokloc = getLocation(clang_getRangeStart(range));

    if (tokloc.line() != loc.line() || tokloc.column() != loc.column())
      result.push_back(' ');

    result += tokens.at(i).getSpelling();

    loc = getLocation(clang_getRangeEnd(range));
  }

  return result;
}

cxx::Expression LibClangParser::parseExpression(const ClangCursor& c)
{
  std::string spelling = c.getSpelling();

  if(!spelling.empty())
    return Expression{ std::move(spelling) };

  CXSourceRange range = c.getExtent();
  ClangTokenSet tokens = m_tu.tokenize(range);

  spelling = getSpelling(tokens);

  // @TODO: derived IExpression from AstNode & parse unexposed expressions
  //c.visitChildren([&](const ClangCursor& child) {
  //  recursiveUnexposedParse(child);
  //  });

  return Expression{ std::move(spelling) };
}

static void remove_prefix(std::string& str, const std::string& prefix)
{
  if (str.find(prefix) == 0)
    str.erase(0, prefix.size());
}

cxx::Type LibClangParser::parseType(CXType t)
{
  bool is_const = clang_isConstQualifiedType(t);
  bool is_volatile = clang_isVolatileQualifiedType(t);

  cxx::CVQualifier cv_qual = make_cv_qual(is_const, is_volatile);

  if (t.kind == CXTypeKind::CXType_LValueReference || t.kind == CXTypeKind::CXType_RValueReference)
  {
    cxx::Reference ref = t.kind == CXTypeKind::CXType_LValueReference ? cxx::Reference::LValue : cxx::Reference::RValue;

    CXType nested_type = clang_getPointeeType(t);

    return Type::reference(Type::cvQualified(parseType(nested_type), cv_qual), ref);
  }
  else if (t.kind == CXTypeKind::CXType_Pointer)
  {
    CXType nested_type = clang_getPointeeType(t);

    return Type::cvQualified(Type::pointer(parseType(nested_type)), cv_qual);
  }
  else
  {
    if (t.kind == CXTypeKind::CXType_Elaborated)
    {
      std::string spelling = getTypeSpelling(t);
      return RestrictedParser::parseType(spelling);
    }
    else
    {
      std::string spelling = getTypeSpelling(t);
      remove_prefix(spelling, "const ");

      return Type{ std::move(spelling), cv_qual };
    }
  }
}

cxx::SourceLocation LibClangParser::getCursorLocation(CXCursor cursor)
{
  CXSourceLocation location = clang_getCursorLocation(cursor);
  return getLocation(location);
}

cxx::SourceLocation LibClangParser::getLocation(const CXSourceLocation& location)
{
  CXFile file;
  unsigned int line, col, offset;
  clang_getSpellingLocation(location, &file, &line, &col, &offset);

  if (clang_File_isEqual(m_current_cxfile, file))
  {
    return cxx::SourceLocation(m_current_file, line, col);
  }
  else
  {
    std::string file_name = toStdString(clang_getFileName(file));
    File::normalizePath(file_name);
    auto result_file = getFile(file_name);

    return cxx::SourceLocation(result_file, line, col);
  }
}

cxx::SourceRange LibClangParser::getCursorExtent(CXCursor cursor)
{
  CXSourceRange range = clang_getCursorExtent(cursor);

  if (clang_Range_isNull(range)) 
    return {};

  cxx::SourceLocation start = getLocation(clang_getRangeStart(range));
  cxx::SourceLocation end = getLocation(clang_getRangeEnd(range));

  return cxx::SourceRange(start, end);
}

} // namespace parsers

} // namespace cxx

