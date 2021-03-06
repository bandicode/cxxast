// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/node.h"

#include "cxx/astnodelist_p.h"
#include "cxx/entity.h"
#include "cxx/statement.h"

#include <stdexcept>

namespace cxx
{

std::string to_string(NodeKind k)
{
  switch (k)
  {
  case NodeKind::Class: return "Class";
  case NodeKind::ClassTemplate: return "ClassTemplate";
  case NodeKind::Enum: return "Enum";
  case NodeKind::EnumValue: return "EnumValue";
  case NodeKind::Function: return "Function";
  case NodeKind::FunctionTemplate: return "FunctionTemplate";
  case NodeKind::FunctionParameter: return "FunctionParameter";
  case NodeKind::Macro: return "Macro";
  case NodeKind::Namespace: return "Namespace";
  case NodeKind::TemplateParameter: return "TemplateParameter";
  case NodeKind::Typedef: return "Typedef";
  case NodeKind::Variable: return "Variable";
  case NodeKind::NullStatement: return "NullStatement";
  case NodeKind::BreakStatement: return "BreakStatement";
  case NodeKind::CaseStatement: return "CaseStatement";
  case NodeKind::CatchStatement: return "CatchStatement";
  case NodeKind::ContinueStatement: return "ContinueStatement";
  case NodeKind::CompoundStatement: return "CompoundStatement";
  case NodeKind::DefaultStatement: return "DefaultStatement";
  case NodeKind::DoWhileLoop: return "DoWhileLoop";
  case NodeKind::ExpressionStatement: return "ExpressionStatement";
  case NodeKind::ForLoop: return "ForLoop";
  case NodeKind::ForRange: return "ForRange";
  case NodeKind::IfStatement: return "IfStatement";
  case NodeKind::ReturnStatement: return "ReturnStatement";
  case NodeKind::SwitchStatement: return "SwitchStatement";
  case NodeKind::TryBlock: return "TryBlock";
  case NodeKind::WhileLoop: return "WhileLoop";
  case NodeKind::AccessSpecifierDeclaration: return "AccessSpecifierDeclaration";
  case NodeKind::ClassDeclaration: return "ClassDeclaration";
  case NodeKind::EnumDeclaration: return "EnumDeclaration";
  case NodeKind::EnumeratorDeclaration: return "EnumeratorDeclaration";
  case NodeKind::FunctionDeclaration: return "FunctionDeclaration";
  case NodeKind::NamespaceDeclaration: return "NamespaceDeclaration";
  case NodeKind::ParameterDeclaration: return "ParameterDeclaration";
  case NodeKind::VariableDeclaration: return "VariableDeclaration";
  case NodeKind::TemplateParameterDeclaration: return "TemplateParameterDeclaration";
  case NodeKind::TypedefDeclaration: return "TypedefDeclaration";
  case NodeKind::UnexposedStatement: return "UnexposedStatement";
  case NodeKind::UnexposedExpression: return "UnexposedExpression";
  case NodeKind::AstRootNode: return "AstRootNode";
  case NodeKind::AstUnexposedNode: return "AstUnexposedNode";
  case NodeKind::MultilineComment: return "MultilineComment";
  case NodeKind::Documentation: return "Documentation";
  default: return "";
  }
}

INode::~INode()
{

}

bool INode::isEntity() const
{
  return false;
}

bool INode::isAstNode() const
{
  return false;
}

bool INode::isDocumentation() const
{
  return false;
}

bool INode::isStatement() const
{
  return false;
}

bool INode::isDeclaration() const
{
  return false;
}

Entity Node::toEntity() const
{
  return isEntity() ? Entity{ std::static_pointer_cast<IEntity>(d) } : Entity{};
}

Statement Node::toStatement() const
{
  return isStatement() ? Statement{ std::static_pointer_cast<IStatement>(d) } : Statement{};
}

std::string to_string(AstNodeKind k)
{
  switch (k)
  {
  case cxx::AstNodeKind::Root: return "root";
  case AstNodeKind::UnexposedDecl: return "UnexposedDecl";
  case AstNodeKind::StructDecl: return "StructDecl";
  case AstNodeKind::UnionDecl: return "UnionDecl";
  case AstNodeKind::ClassDecl: return "ClassDecl";
  case AstNodeKind::EnumDecl: return "EnumDecl";
  case AstNodeKind::FieldDecl: return "FieldDecl";
  case AstNodeKind::EnumConstantDecl: return "EnumConstantDecl";
  case AstNodeKind::FunctionDecl: return "FunctionDecl";
  case AstNodeKind::VarDecl: return "VarDecl";
  case AstNodeKind::ParmDecl: return "ParmDecl";
  case AstNodeKind::ObjCInterfaceDecl: return "ObjCInterfaceDecl";
  case AstNodeKind::ObjCCategoryDecl: return "ObjCCategoryDecl";
  case AstNodeKind::ObjCProtocolDecl: return "ObjCProtocolDecl";
  case AstNodeKind::ObjCPropertyDecl: return "ObjCPropertyDecl";
  case AstNodeKind::ObjCIvarDecl: return "ObjCIvarDecl";
  case AstNodeKind::ObjCInstanceMethodDecl: return "ObjCInstanceMethodDecl";
  case AstNodeKind::ObjCClassMethodDecl: return "ObjCClassMethodDecl";
  case AstNodeKind::ObjCImplementationDecl: return "ObjCImplementationDecl";
  case AstNodeKind::ObjCCategoryImplDecl: return "ObjCCategoryImplDecl";
  case AstNodeKind::TypedefDecl: return "TypedefDecl";
  case AstNodeKind::CXXMethod: return "CXXMethod";
  case AstNodeKind::Namespace: return "Namespace";
  case AstNodeKind::LinkageSpec: return "LinkageSpec";
  case AstNodeKind::Constructor: return "Constructor";
  case AstNodeKind::Destructor: return "Destructor";
  case AstNodeKind::ConversionFunction: return "ConversionFunction";
  case AstNodeKind::TemplateTypeParameter: return "TemplateTypeParameter";
  case AstNodeKind::NonTypeTemplateParameter: return "NonTypeTemplateParameter";
  case AstNodeKind::TemplateTemplateParameter: return "TemplateTemplateParameter";
  case AstNodeKind::FunctionTemplate: return "FunctionTemplate";
  case AstNodeKind::ClassTemplate: return "ClassTemplate";
  case AstNodeKind::ClassTemplatePartialSpecialization: return "ClassTemplatePartialSpecialization";
  case AstNodeKind::NamespaceAlias: return "NamespaceAlias";
  case AstNodeKind::UsingDirective: return "UsingDirective";
  case AstNodeKind::UsingDeclaration: return "UsingDeclaration";
  case AstNodeKind::TypeAliasDecl: return "TypeAliasDecl";
  case AstNodeKind::ObjCSynthesizeDecl: return "ObjCSynthesizeDecl";
  case AstNodeKind::ObjCDynamicDecl: return "ObjCDynamicDecl";
  case AstNodeKind::CXXAccessSpecifier: return "CXXAccessSpecifier";
  //case AstNodeKind::FirstDecl: return "FirstDecl";
  //case AstNodeKind::LastDecl: return "LastDecl";
  //case AstNodeKind::FirstRef: return "FirstRef";
  case AstNodeKind::ObjCSuperClassRef: return "ObjCSuperClassRef";
  case AstNodeKind::ObjCProtocolRef: return "ObjCProtocolRef";
  case AstNodeKind::ObjCClassRef: return "ObjCClassRef";
  case AstNodeKind::TypeRef: return "TypeRef";
  case AstNodeKind::CXXBaseSpecifier: return "CXXBaseSpecifier";
  case AstNodeKind::TemplateRef: return "TemplateRef";
  case AstNodeKind::NamespaceRef: return "NamespaceRef";
  case AstNodeKind::MemberRef: return "MemberRef";
  case AstNodeKind::LabelRef: return "LabelRef";
  case AstNodeKind::OverloadedDeclRef: return "OverloadedDeclRef";
  case AstNodeKind::VariableRef: return "VariableRef";
  case AstNodeKind::LastRef: return "LastRef";
  //case AstNodeKind::FirstInvalid: return "FirstInvalid";
  case AstNodeKind::InvalidFile: return "InvalidFile";
  case AstNodeKind::NoDeclFound: return "NoDeclFound";
  case AstNodeKind::NotImplemented: return "NotImplemented";
  case AstNodeKind::InvalidCode: return "InvalidCode";
  //case AstNodeKind::LastInvalid: return "LastInvalid";
  //case AstNodeKind::FirstExpr: return "FirstExpr";
  case AstNodeKind::UnexposedExpr: return "UnexposedExpr";
  case AstNodeKind::DeclRefExpr: return "DeclRefExpr";
  case AstNodeKind::MemberRefExpr: return "MemberRefExpr";
  case AstNodeKind::CallExpr: return "CallExpr";
  case AstNodeKind::ObjCMessageExpr: return "ObjCMessageExpr";
  case AstNodeKind::BlockExpr: return "BlockExpr";
  case AstNodeKind::IntegerLiteral: return "IntegerLiteral";
  case AstNodeKind::FloatingLiteral: return "FloatingLiteral";
  case AstNodeKind::ImaginaryLiteral: return "ImaginaryLiteral";
  case AstNodeKind::StringLiteral: return "StringLiteral";
  case AstNodeKind::CharacterLiteral: return "CharacterLiteral";
  case AstNodeKind::ParenExpr: return "ParenExpr";
  case AstNodeKind::UnaryOperator: return "UnaryOperator";
  case AstNodeKind::ArraySubscriptExpr: return "ArraySubscriptExpr";
  case AstNodeKind::BinaryOperator: return "BinaryOperator";
  case AstNodeKind::CompoundAssignOperator: return "CompoundAssignOperator";
  case AstNodeKind::ConditionalOperator: return "ConditionalOperator";
  case AstNodeKind::CStyleCastExpr: return "CStyleCastExpr";
  case AstNodeKind::CompoundLiteralExpr: return "CompoundLiteralExpr";
  case AstNodeKind::InitListExpr: return "InitListExpr";
  case AstNodeKind::AddrLabelExpr: return "AddrLabelExpr";
  case AstNodeKind::StmtExpr: return "StmtExpr";
  case AstNodeKind::GenericSelectionExpr: return "GenericSelectionExpr";
  case AstNodeKind::GNUNullExpr: return "GNUNullExpr";
  case AstNodeKind::CXXStaticCastExpr: return "CXXStaticCastExpr";
  case AstNodeKind::CXXDynamicCastExpr: return "CXXDynamicCastExpr";
  case AstNodeKind::CXXReinterpretCastExpr: return "CXXReinterpretCastExpr";
  case AstNodeKind::CXXConstCastExpr: return "CXXConstCastExpr";
  case AstNodeKind::CXXFunctionalCastExpr: return "CXXFunctionalCastExpr";
  case AstNodeKind::CXXTypeidExpr: return "CXXTypeidExpr";
  case AstNodeKind::CXXBoolLiteralExpr: return "CXXBoolLiteralExpr";
  case AstNodeKind::CXXNullPtrLiteralExpr: return "CXXNullPtrLiteralExpr";
  case AstNodeKind::CXXThisExpr: return "CXXThisExpr";
  case AstNodeKind::CXXThrowExpr: return "CXXThrowExpr";
  case AstNodeKind::CXXNewExpr: return "CXXNewExpr";
  case AstNodeKind::CXXDeleteExpr: return "CXXDeleteExpr";
  case AstNodeKind::UnaryExpr: return "UnaryExpr";
  case AstNodeKind::ObjCStringLiteral: return "ObjCStringLiteral";
  case AstNodeKind::ObjCEncodeExpr: return "ObjCEncodeExpr";
  case AstNodeKind::ObjCSelectorExpr: return "ObjCSelectorExpr";
  case AstNodeKind::ObjCProtocolExpr: return "ObjCProtocolExpr";
  case AstNodeKind::ObjCBridgedCastExpr: return "ObjCBridgedCastExpr";
  case AstNodeKind::PackExpansionExpr: return "PackExpansionExpr";
  case AstNodeKind::SizeOfPackExpr: return "SizeOfPackExpr";
  case AstNodeKind::LambdaExpr: return "LambdaExpr";
  case AstNodeKind::ObjCBoolLiteralExpr: return "ObjCBoolLiteralExpr";
  case AstNodeKind::ObjCSelfExpr: return "ObjCSelfExpr";
  case AstNodeKind::OMPArraySectionExpr: return "OMPArraySectionExpr";
  case AstNodeKind::ObjCAvailabilityCheckExpr: return "ObjCAvailabilityCheckExpr";
  case AstNodeKind::FixedPointLiteral: return "FixedPointLiteral";
  //case AstNodeKind::LastExpr: return "LastExpr";
  //case AstNodeKind::FirstStmt: return "FirstStmt";
  case AstNodeKind::UnexposedStmt: return "UnexposedStmt";
  case AstNodeKind::LabelStmt: return "LabelStmt";
  case AstNodeKind::CompoundStmt: return "CompoundStmt";
  case AstNodeKind::CaseStmt: return "CaseStmt";
  case AstNodeKind::DefaultStmt: return "DefaultStmt";
  case AstNodeKind::IfStmt: return "IfStmt";
  case AstNodeKind::SwitchStmt: return "SwitchStmt";
  case AstNodeKind::WhileStmt: return "WhileStmt";
  case AstNodeKind::DoStmt: return "DoStmt";
  case AstNodeKind::ForStmt: return "ForStmt";
  case AstNodeKind::GotoStmt: return "GotoStmt";
  case AstNodeKind::IndirectGotoStmt: return "IndirectGotoStmt";
  case AstNodeKind::ContinueStmt: return "ContinueStmt";
  case AstNodeKind::BreakStmt: return "BreakStmt";
  case AstNodeKind::ReturnStmt: return "ReturnStmt";
  case AstNodeKind::GCCAsmStmt: return "GCCAsmStmt";
  //case AstNodeKind::AsmStmt: return "AsmStmt";
  case AstNodeKind::ObjCAtTryStmt: return "ObjCAtTryStmt";
  case AstNodeKind::ObjCAtCatchStmt: return "ObjCAtCatchStmt";
  case AstNodeKind::ObjCAtFinallyStmt: return "ObjCAtFinallyStmt";
  case AstNodeKind::ObjCAtThrowStmt: return "ObjCAtThrowStmt";
  case AstNodeKind::ObjCAtSynchronizedStmt: return "ObjCAtSynchronizedStmt";
  case AstNodeKind::ObjCAutoreleasePoolStmt: return "ObjCAutoreleasePoolStmt";
  case AstNodeKind::ObjCForCollectionStmt: return "ObjCForCollectionStmt";
  case AstNodeKind::CXXCatchStmt: return "CXXCatchStmt";
  case AstNodeKind::CXXTryStmt: return "CXXTryStmt";
  case AstNodeKind::CXXForRangeStmt: return "CXXForRangeStmt";
  case AstNodeKind::SEHTryStmt: return "SEHTryStmt";
  case AstNodeKind::SEHExceptStmt: return "SEHExceptStmt";
  case AstNodeKind::SEHFinallyStmt: return "SEHFinallyStmt";
  case AstNodeKind::MSAsmStmt: return "MSAsmStmt";
  case AstNodeKind::NullStmt: return "NullStmt";
  case AstNodeKind::DeclStmt: return "DeclStmt";
  case AstNodeKind::OMPParallelDirective: return "OMPParallelDirective";
  case AstNodeKind::OMPSimdDirective: return "OMPSimdDirective";
  case AstNodeKind::OMPForDirective: return "OMPForDirective";
  case AstNodeKind::OMPSectionsDirective: return "OMPSectionsDirective";
  case AstNodeKind::OMPSectionDirective: return "OMPSectionDirective";
  case AstNodeKind::OMPSingleDirective: return "OMPSingleDirective";
  case AstNodeKind::OMPParallelForDirective: return "OMPParallelForDirective";
  case AstNodeKind::OMPParallelSectionsDirective: return "OMPParallelSectionsDirective";
  case AstNodeKind::OMPTaskDirective: return "OMPTaskDirective";
  case AstNodeKind::OMPMasterDirective: return "OMPMasterDirective";
  case AstNodeKind::OMPCriticalDirective: return "OMPCriticalDirective";
  case AstNodeKind::OMPTaskyieldDirective: return "OMPTaskyieldDirective";
  case AstNodeKind::OMPBarrierDirective: return "OMPBarrierDirective";
  case AstNodeKind::OMPTaskwaitDirective: return "OMPTaskwaitDirective";
  case AstNodeKind::OMPFlushDirective: return "OMPFlushDirective";
  case AstNodeKind::SEHLeaveStmt: return "SEHLeaveStmt";
  case AstNodeKind::OMPOrderedDirective: return "OMPOrderedDirective";
  case AstNodeKind::OMPAtomicDirective: return "OMPAtomicDirective";
  case AstNodeKind::OMPForSimdDirective: return "OMPForSimdDirective";
  case AstNodeKind::OMPParallelForSimdDirective: return "OMPParallelForSimdDirective";
  case AstNodeKind::OMPTargetDirective: return "OMPTargetDirective";
  case AstNodeKind::OMPTeamsDirective: return "OMPTeamsDirective";
  case AstNodeKind::OMPTaskgroupDirective: return "OMPTaskgroupDirective";
  case AstNodeKind::OMPCancellationPointDirective: return "OMPCancellationPointDirective";
  case AstNodeKind::OMPCancelDirective: return "OMPCancelDirective";
  case AstNodeKind::OMPTargetDataDirective: return "OMPTargetDataDirective";
  case AstNodeKind::OMPTaskLoopDirective: return "OMPTaskLoopDirective";
  case AstNodeKind::OMPTaskLoopSimdDirective: return "OMPTaskLoopSimdDirective";
  case AstNodeKind::OMPDistributeDirective: return "OMPDistributeDirective";
  case AstNodeKind::OMPTargetEnterDataDirective: return "OMPTargetEnterDataDirective";
  case AstNodeKind::OMPTargetExitDataDirective: return "OMPTargetExitDataDirective";
  case AstNodeKind::OMPTargetParallelDirective: return "OMPTargetParallelDirective";
  case AstNodeKind::OMPTargetParallelForDirective: return "OMPTargetParallelForDirective";
  case AstNodeKind::OMPTargetUpdateDirective: return "OMPTargetUpdateDirective";
  case AstNodeKind::OMPDistributeParallelForDirective: return "OMPDistributeParallelForDirective";
  case AstNodeKind::OMPDistributeParallelForSimdDirective: return "OMPDistributeParallelForSimdDirective";
  case AstNodeKind::OMPDistributeSimdDirective: return "OMPDistributeSimdDirective";
  case AstNodeKind::OMPTargetParallelForSimdDirective: return "OMPTargetParallelForSimdDirective";
  case AstNodeKind::OMPTargetSimdDirective: return "OMPTargetSimdDirective";
  case AstNodeKind::OMPTeamsDistributeDirective: return "OMPTeamsDistributeDirective";
  case AstNodeKind::OMPTeamsDistributeSimdDirective: return "OMPTeamsDistributeSimdDirective";
  case AstNodeKind::OMPTeamsDistributeParallelForSimdDirective: return "OMPTeamsDistributeParallelForSimdDirective";
  case AstNodeKind::OMPTeamsDistributeParallelForDirective: return "OMPTeamsDistributeParallelForDirective";
  case AstNodeKind::OMPTargetTeamsDirective: return "OMPTargetTeamsDirective";
  case AstNodeKind::OMPTargetTeamsDistributeDirective: return "OMPTargetTeamsDistributeDirective";
  case AstNodeKind::OMPTargetTeamsDistributeParallelForDirective: return "OMPTargetTeamsDistributeParallelForDirective";
  case AstNodeKind::OMPTargetTeamsDistributeParallelForSimdDirective: return "OMPTargetTeamsDistributeParallelForSimdDirective";
  case AstNodeKind::OMPTargetTeamsDistributeSimdDirective: return "OMPTargetTeamsDistributeSimdDirective";
  //case AstNodeKind::LastStmt: return "LastStmt";
  case AstNodeKind::TranslationUnit: return "TranslationUnit";
  //case AstNodeKind::FirstAttr: return "FirstAttr";
  case AstNodeKind::UnexposedAttr: return "UnexposedAttr";
  case AstNodeKind::IBActionAttr: return "IBActionAttr";
  case AstNodeKind::IBOutletAttr: return "IBOutletAttr";
  case AstNodeKind::IBOutletCollectionAttr: return "IBOutletCollectionAttr";
  case AstNodeKind::CXXFinalAttr: return "CXXFinalAttr";
  case AstNodeKind::CXXOverrideAttr: return "CXXOverrideAttr";
  case AstNodeKind::AnnotateAttr: return "AnnotateAttr";
  case AstNodeKind::AsmLabelAttr: return "AsmLabelAttr";
  case AstNodeKind::PackedAttr: return "PackedAttr";
  case AstNodeKind::PureAttr: return "PureAttr";
  case AstNodeKind::ConstAttr: return "ConstAttr";
  case AstNodeKind::NoDuplicateAttr: return "NoDuplicateAttr";
  case AstNodeKind::CUDAConstantAttr: return "CUDAConstantAttr";
  case AstNodeKind::CUDADeviceAttr: return "CUDADeviceAttr";
  case AstNodeKind::CUDAGlobalAttr: return "CUDAGlobalAttr";
  case AstNodeKind::CUDAHostAttr: return "CUDAHostAttr";
  case AstNodeKind::CUDASharedAttr: return "CUDASharedAttr";
  case AstNodeKind::VisibilityAttr: return "VisibilityAttr";
  case AstNodeKind::DLLExport: return "DLLExport";
  case AstNodeKind::DLLImport: return "DLLImport";
  //case AstNodeKind::LastAttr: return "LastAttr";
  case AstNodeKind::PreprocessingDirective: return "PreprocessingDirective";
  case AstNodeKind::MacroDefinition: return "MacroDefinition";
  case AstNodeKind::MacroExpansion: return "MacroExpansion";
  //case AstNodeKind::MacroInstantiation: return "MacroInstantiation";
  case AstNodeKind::InclusionDirective: return "InclusionDirective";
  //case AstNodeKind::FirstPreprocessing: return "FirstPreprocessing";
  //case AstNodeKind::LastPreprocessing: return "LastPreprocessing";
  case AstNodeKind::ModuleImportDecl: return "ModuleImportDecl";
  case AstNodeKind::TypeAliasTemplateDecl: return "TypeAliasTemplateDecl";
  case AstNodeKind::StaticAssert: return "StaticAssert";
  case AstNodeKind::FriendDecl: return "FriendDecl";
  //case AstNodeKind::FirstExtraDecl: return "FirstExtraDecl";
  //case AstNodeKind::LastExtraDecl: return "LastExtraDecl";
  case AstNodeKind::OverloadCandidate: return "OverloadCandidate";
  default: return "root";
  }
}

bool AstNode::isAstNode() const
{
  return true;
}

std::shared_ptr<INode> AstNode::parent() const
{
  return astParent();
}

std::shared_ptr<AstNode> AstNode::astParent() const
{
  return weak_parent.lock();
}

void AstNode::append(std::shared_ptr<AstNode> n)
{
  throw std::runtime_error{ "not supported" };
}

AstNodeList AstNode::children() const
{
  return AstNodeList();
}

void AstNode::updateSourceRange()
{
  if (children().empty())
    return;

  sourcerange = children().front()->sourcerange;
  sourcerange.end = children().back()->sourcerange.end;
}

std::shared_ptr<File> AstNode::file() const
{
  return sourcerange.file.lock();
}


AstNodeList::AstNodeList()
  : d(std::make_shared<AstEmptyNodeList>())
{

}

AstNodeList::~AstNodeList()
{

}

AstNodeList::AstNodeList(std::shared_ptr<AstNodeListInterface> impl)
  : d(impl)
{

}

bool AstNodeList::empty() const
{
  return size() == 0;
}

size_t AstNodeList::size() const
{
  return d->size();
}

std::shared_ptr<AstNode> AstNodeList::at(size_t index) const
{
  return d->at(index);
}

std::shared_ptr<AstNode> AstNodeList::front() const
{
  return at(0);
}

std::shared_ptr<AstNode> AstNodeList::back() const
{
  return at(size() - 1);
}

AstNodeListIterator AstNodeList::begin() const
{
  return AstNodeListIterator(*this);
}

AstNodeListIterator AstNodeList::end() const
{
  return AstNodeListIterator(*this, size());
}

bool AstNodeList::operator==(const AstNodeList& other) const
{
  return d == other.d;
}

bool AstNodeList::operator!=(const AstNodeList& other) const
{
  return d != other.d;
}


AstNodeListIterator::AstNodeListIterator(const AstNodeList& list, size_t i)
  : m_list(&list),
    m_index(i)
{

}

std::shared_ptr<AstNode> AstNodeListIterator::operator*() const
{
  return m_list->at(m_index);
}

AstNodeListIterator& AstNodeListIterator::operator++()
{
  ++m_index;
  return *this;
}

AstNodeListIterator AstNodeListIterator::operator++(int)
{
  AstNodeListIterator it{ *this };
  ++m_index;
  return it;
}

bool AstNodeListIterator::operator==(const AstNodeListIterator& other) const
{
  return m_list == other.m_list && m_index == other.m_index;
}

bool AstNodeListIterator::operator!=(const AstNodeListIterator& other) const
{
  return !(*this == other);
}


AstRootNode::AstRootNode()
{

}

NodeKind AstRootNode::node_kind() const
{
  return NodeKind::AstRootNode;
}

void AstRootNode::append(std::shared_ptr<AstNode> n)
{
  childvec.push_back(n);
  n->weak_parent = std::static_pointer_cast<AstNode>(shared_from_this());
}

AstNodeList AstRootNode::children() const
{
  return AstNodeList(std::make_shared<AstVectorRefNodeList>(childvec));
}


UnexposedAstNode::UnexposedAstNode(AstNodeKind k)
  : kind(k)
{

}

NodeKind UnexposedAstNode::node_kind() const
{
  return NodeKind::AstUnexposedNode;
}

void UnexposedAstNode::append(std::shared_ptr<AstNode> n)
{
  childvec.push_back(n);
  n->weak_parent = std::static_pointer_cast<AstNode>(shared_from_this());
}

AstNodeList UnexposedAstNode::children() const
{
  return AstNodeList(std::make_shared<AstVectorRefNodeList>(childvec));
}

} // namespace cxx