// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NODE_H
#define CXXAST_NODE_H

#include "cxx/sourcerange.h"

#include <memory>
#include <string>

namespace cxx
{

class Class;
class Entity;
class Expression;
class Statement;
class Function;

enum class NodeKind
{
  /* Entities */
  Class,
  ClassTemplate,
  Enum,
  EnumValue,
  Function,
  FunctionTemplate,
  FunctionParameter,
  Macro,
  Namespace,
  TemplateParameter,
  Typedef,
  Variable,
  /* Statements */
  NullStatement,
  BreakStatement, 
  CaseStatement,
  CatchStatement,
  ContinueStatement,
  CompoundStatement,
  DefaultStatement,
  DoWhileLoop,
  ForLoop,
  ForRange,
  IfStatement,
  ReturnStatement,
  SwitchStatement,
  TryBlock,
  WhileLoop,
  ClassDeclaration,
  EnumDeclaration,
  EnumeratorDeclaration,
  FunctionDeclaration,
  NamespaceDeclaration,
  VariableDeclaration,
  TemplateParameterDeclaration,
  UnexposedStatement,
  /* Expressions */
  UnexposedExpression,
  /*Ast */
  AstRootNode,
  AstUnexposedNode,
  /* Misc */
  MultilineComment,
  Documentation,
};

CXXAST_API std::string to_string(NodeKind k);

namespace priv
{

struct CXXAST_API IField
{

};

} // namespace priv

class CXXAST_API INode : public std::enable_shared_from_this<INode>
{
public:
  INode() = default;
  virtual ~INode();

  virtual NodeKind node_kind() const = 0;
  NodeKind kind() const { return node_kind(); }

  virtual bool isEntity() const;
  virtual bool isAstNode() const;
  virtual bool isDocumentation() const;
  virtual bool isStatement() const;
  virtual bool isDeclaration() const;

  template<typename T>
  bool is() const;

  template<typename F>
  typename F::field_type& get();

  template<typename F, typename Arg>
  void set(Arg&& arg);
};

template<typename T>
class Handle
{
protected:
  std::shared_ptr<T> d;

public:
  Handle() = default;
  Handle(const Handle&) = default;
  ~Handle() = default;

  Handle(std::shared_ptr<T> impl)
    : d(std::move(impl))
  {

  }

  // @TODO: not sure about this one
  NodeKind kind() const { return d->node_kind(); }

  bool isNull() const { return d == nullptr; }

  template<typename U>
  bool is() const { return d->template is<U>(); }

  template<typename F>
  typename F::field_type& get() const { return d->template get<F>(); }

  template<typename F, typename Arg>
  void set(Arg&& value) { return d->template set<F>(std::forward<Arg>(value)); }

  const std::shared_ptr<T>& impl() const { return d; }

  Handle<T>& operator=(const Handle<T>&) = default;

  bool operator==(const Handle<T>& other) const { return impl() == other.impl(); }
  bool operator!=(const Handle<T>& other) const { return impl() != other.impl(); }
};


class CXXAST_API Node : public Handle<INode>
{
public:
  using Handle<INode>::Handle;

  bool isEntity() const { return d->isEntity(); }
  bool isDocumentation() const { return d->isDocumentation(); }
  bool isStatement() const { return d->isStatement(); }
  bool isDeclaration() const { return d->isDeclaration(); }

  Entity toEntity() const;
  Statement toStatement() const;
};

template<typename T>
bool test_node_kind(const INode& n)
{
  return T::ClassNodeKind == n.kind();
}

template<>
inline bool test_node_kind<Class>(const INode& n)
{
  return n.kind() == NodeKind::Class || n.kind() == NodeKind::ClassTemplate;
}

template<>
inline bool test_node_kind<Function>(const INode& n)
{
  return n.kind() == NodeKind::Function || n.kind() == NodeKind::FunctionTemplate;
}

template<>
inline bool test_node_kind<AstNode>(const INode& n)
{
  return n.isAstNode();
}

namespace priv
{

template<typename T>
struct FieldOfClass : public IField
{
  static T& down_cast(INode& self)
  {
    return static_cast<T&>(self);
  }
};

template<typename C, typename T>
struct Field : public FieldOfClass<C>
{
  typedef T field_type;
};

template<typename C, typename T, T C::*member>
struct FieldEx : public Field<C, T>
{
  typedef T field_type;

  static field_type& get(INode& n)
  {
    return FieldOfClass<C>::down_cast(n).*member;
  }

  static void set(INode& n, field_type val)
  {
    FieldOfClass<C>::down_cast(n).*member = std::move(val);
  }
};

} // namespace priv

enum class AstNodeKind
{
  Root = 0,
  /* for now basic copy of CXCursorKind */
  UnexposedDecl,
  StructDecl,
  UnionDecl,
  ClassDecl,
  EnumDecl,
  FieldDecl,
  EnumConstantDecl,
  FunctionDecl,
  VarDecl,
  ParmDecl,
  ObjCInterfaceDecl,
  ObjCCategoryDecl,
  ObjCProtocolDecl,
  ObjCPropertyDecl,
  ObjCIvarDecl,
  ObjCInstanceMethodDecl,
  ObjCClassMethodDecl,
  ObjCImplementationDecl,
  ObjCCategoryImplDecl,
  TypedefDecl,
  CXXMethod,
  Namespace,
  LinkageSpec,
  Constructor,
  Destructor,
  ConversionFunction,
  TemplateTypeParameter,
  NonTypeTemplateParameter,
  TemplateTemplateParameter,
  FunctionTemplate,
  ClassTemplate,
  ClassTemplatePartialSpecialization,
  NamespaceAlias,
  UsingDirective,
  UsingDeclaration,
  TypeAliasDecl,
  ObjCSynthesizeDecl,
  ObjCDynamicDecl,
  CXXAccessSpecifier,
  //FirstDecl,
  //LastDecl,
  //FirstRef,
  ObjCSuperClassRef,
  ObjCProtocolRef,
  ObjCClassRef,
  TypeRef,
  CXXBaseSpecifier,
  TemplateRef,
  NamespaceRef,
  MemberRef,
  LabelRef,
  OverloadedDeclRef,
  VariableRef,
  LastRef,
  //FirstInvalid,
  InvalidFile,
  NoDeclFound,
  NotImplemented,
  InvalidCode,
  //LastInvalid,
  //FirstExpr,
  UnexposedExpr,
  DeclRefExpr,
  MemberRefExpr,
  CallExpr,
  ObjCMessageExpr,
  BlockExpr,
  IntegerLiteral,
  FloatingLiteral,
  ImaginaryLiteral,
  StringLiteral,
  CharacterLiteral,
  ParenExpr,
  UnaryOperator,
  ArraySubscriptExpr,
  BinaryOperator,
  CompoundAssignOperator,
  ConditionalOperator,
  CStyleCastExpr,
  CompoundLiteralExpr,
  InitListExpr,
  AddrLabelExpr,
  StmtExpr,
  GenericSelectionExpr,
  GNUNullExpr,
  CXXStaticCastExpr,
  CXXDynamicCastExpr,
  CXXReinterpretCastExpr,
  CXXConstCastExpr,
  CXXFunctionalCastExpr,
  CXXTypeidExpr,
  CXXBoolLiteralExpr,
  CXXNullPtrLiteralExpr,
  CXXThisExpr,
  CXXThrowExpr,
  CXXNewExpr,
  CXXDeleteExpr,
  UnaryExpr,
  ObjCStringLiteral,
  ObjCEncodeExpr,
  ObjCSelectorExpr,
  ObjCProtocolExpr,
  ObjCBridgedCastExpr,
  PackExpansionExpr,
  SizeOfPackExpr,
  LambdaExpr,
  ObjCBoolLiteralExpr,
  ObjCSelfExpr,
  OMPArraySectionExpr,
  ObjCAvailabilityCheckExpr,
  FixedPointLiteral,
  //LastExpr,
  //FirstStmt,
  UnexposedStmt,
  LabelStmt,
  CompoundStmt,
  CaseStmt,
  DefaultStmt,
  IfStmt,
  SwitchStmt,
  WhileStmt,
  DoStmt,
  ForStmt,
  GotoStmt,
  IndirectGotoStmt,
  ContinueStmt,
  BreakStmt,
  ReturnStmt,
  GCCAsmStmt,
  //AsmStmt,
  ObjCAtTryStmt,
  ObjCAtCatchStmt,
  ObjCAtFinallyStmt,
  ObjCAtThrowStmt,
  ObjCAtSynchronizedStmt,
  ObjCAutoreleasePoolStmt,
  ObjCForCollectionStmt,
  CXXCatchStmt,
  CXXTryStmt,
  CXXForRangeStmt,
  SEHTryStmt,
  SEHExceptStmt,
  SEHFinallyStmt,
  MSAsmStmt,
  NullStmt,
  DeclStmt,
  OMPParallelDirective,
  OMPSimdDirective,
  OMPForDirective,
  OMPSectionsDirective,
  OMPSectionDirective,
  OMPSingleDirective,
  OMPParallelForDirective,
  OMPParallelSectionsDirective,
  OMPTaskDirective,
  OMPMasterDirective,
  OMPCriticalDirective,
  OMPTaskyieldDirective,
  OMPBarrierDirective,
  OMPTaskwaitDirective,
  OMPFlushDirective,
  SEHLeaveStmt,
  OMPOrderedDirective,
  OMPAtomicDirective,
  OMPForSimdDirective,
  OMPParallelForSimdDirective,
  OMPTargetDirective,
  OMPTeamsDirective,
  OMPTaskgroupDirective,
  OMPCancellationPointDirective,
  OMPCancelDirective,
  OMPTargetDataDirective,
  OMPTaskLoopDirective,
  OMPTaskLoopSimdDirective,
  OMPDistributeDirective,
  OMPTargetEnterDataDirective,
  OMPTargetExitDataDirective,
  OMPTargetParallelDirective,
  OMPTargetParallelForDirective,
  OMPTargetUpdateDirective,
  OMPDistributeParallelForDirective,
  OMPDistributeParallelForSimdDirective,
  OMPDistributeSimdDirective,
  OMPTargetParallelForSimdDirective,
  OMPTargetSimdDirective,
  OMPTeamsDistributeDirective,
  OMPTeamsDistributeSimdDirective,
  OMPTeamsDistributeParallelForSimdDirective,
  OMPTeamsDistributeParallelForDirective,
  OMPTargetTeamsDirective,
  OMPTargetTeamsDistributeDirective,
  OMPTargetTeamsDistributeParallelForDirective,
  OMPTargetTeamsDistributeParallelForSimdDirective,
  OMPTargetTeamsDistributeSimdDirective,
  //LastStmt,
  TranslationUnit,
  //FirstAttr,
  UnexposedAttr,
  IBActionAttr,
  IBOutletAttr,
  IBOutletCollectionAttr,
  CXXFinalAttr,
  CXXOverrideAttr,
  AnnotateAttr,
  AsmLabelAttr,
  PackedAttr,
  PureAttr,
  ConstAttr,
  NoDuplicateAttr,
  CUDAConstantAttr,
  CUDADeviceAttr,
  CUDAGlobalAttr,
  CUDAHostAttr,
  CUDASharedAttr,
  VisibilityAttr,
  DLLExport,
  DLLImport,
  //LastAttr,
  PreprocessingDirective,
  MacroDefinition,
  MacroExpansion,
  //MacroInstantiation,
  InclusionDirective,
  //FirstPreprocessing,
  //LastPreprocessing,
  ModuleImportDecl,
  TypeAliasTemplateDecl,
  StaticAssert,
  FriendDecl,
  //FirstExtraDecl,
  //LastExtraDecl,
  OverloadCandidate,
};

CXXAST_API std::string to_string(AstNodeKind k);

// @TODO: validate the ast design
/*
 * Some thoughts on the ast design:
 * 
 * For large project, having both the ast and the semantic tree might consume too 
 * much memory and be cache unfriendly (lots of small allocations).
 * 
 * Add a virtual file() to AstNode. This function returns the file() of the parent,
 * except in the case of AstRootNode which returns the actual file.
 * That way in an AstNode we can store just the a pair of (line,col) for the start and 
 * the end of the source range.
 * 
 * Instead of having both a generic AstNode and semantic node for each statement 
 * (e.g. AstNode and WhileLoop), a WhileLoop could be derived from AstNode (source 
 * location is optional).
 * 
 * AstNode would have a parent() and astParent(). astParent() is parent() except 
 * for the FunctionBodyCompoundStatement where the parent() is the Function and
 * the astParent() is the AstFunctionDecl.
 * 
 * An AstNode would no longer have a std::vector<std::shared_ptr<AstNode>> but would 
 * provide a virtual children() method that would return a AstNodeChildren.
 * The AstNodeChildren has size() and at() methods and consists of a shared_ptr to
 * a AstNodeChildrenImpl.
 * There could be several implementation:
 * - a default shared implementation when there is no children
 * - an implementation that takes a ref to a std::vector<std::shared_ptr<AstNode>>
 * - an implementation that takes a pointer to the parent and invoke some __size__()
 *   and __at__() function
 * - a generic impl, created with a createAstNodeChildrenImpl(...) that takes as input 
 *   AstNode, Expression, Statements, Types, etc... check whether they are not null 
 *   and part of the ast and fills a std::array<sizeof...(Args)> that is used as storage.
 *   The size must be stored explicitely as the std::array may not be fully filled.
 * 
 * This design would allow both a fast, typed traversal of the ast and a generic, slightly 
 * slower traversal.
 * The AstNodeKind would only be used for non exposed node. 
 * Exposed node have their own C++ class and so their own cxx::NodeKind.
 * 
 * Should keywords be part of the ast ?
 * If they are, there is less special case to handle. 
 * But storing keywords may be overkill as they are easily retrievable from the source code
 * and provide very little added meaning.
 * Besides, this would prevent the sharing of very common nodes like the "int" type.
 * Likewise, the "while" keyword is always at the beginning of a WhileLoop so storing them 
 * in the ast is useless.
 * I suggest not storing them.
 * That's why the createAstNodeChildrenImpl() described earlier must check whether the node 
 * is part of an ast, for example the "int" type would be shared by all ast and therefore 
 * would have an invalid location.
 */
class AstNodeList;

class CXXAST_API AstNode : public INode
{
public:
  SourceRange sourcerange;
  std::weak_ptr<AstNode> weak_parent;

public:
  
  AstNode() = default;
  
  explicit AstNode(const SourceRange& sr)
    : sourcerange(sr)
  {

  }

  bool isAstNode() const override;

  virtual std::shared_ptr<INode> parent() const;
  std::shared_ptr<AstNode> astParent() const;
  virtual void append(std::shared_ptr<AstNode> n);
  virtual AstNodeList children() const;


  void updateSourceRange();

  virtual std::shared_ptr<File> file() const;
};

class CXXAST_API AstNodeListInterface
{
public:
  AstNodeListInterface() = default;
  ~AstNodeListInterface() = default;

  virtual size_t size() const = 0;
  virtual std::shared_ptr<AstNode> at(size_t index) const = 0;

  AstNodeListInterface& operator=(const AstNodeListInterface&) = delete;
};

class AstNodeListIterator;

class CXXAST_API AstNodeList
{
public:
  AstNodeList();
  AstNodeList(const AstNodeList&) = default;
  ~AstNodeList();

  explicit AstNodeList(std::shared_ptr<AstNodeListInterface> impl);

  bool empty() const;
  size_t size() const;
  std::shared_ptr<AstNode> at(size_t index) const;
  std::shared_ptr<AstNode> front() const;
  std::shared_ptr<AstNode> back() const;

  AstNodeListIterator begin() const;
  AstNodeListIterator end() const;

  AstNodeList& operator=(const AstNodeList&) = default;

  bool operator==(const AstNodeList& other) const;
  bool operator!=(const AstNodeList& other) const;

private:
  std::shared_ptr<AstNodeListInterface> d;
};

class CXXAST_API AstNodeListIterator
{
public:
  explicit AstNodeListIterator(const AstNodeList& list, size_t i = 0);

  AstNodeListIterator(const AstNodeListIterator&) = default;
  ~AstNodeListIterator() = default;

  std::shared_ptr<AstNode> operator*() const;

  AstNodeListIterator& operator++();
  AstNodeListIterator operator++(int);

  AstNodeListIterator& operator=(const AstNodeListIterator&) = default;

  bool operator==(const AstNodeListIterator& other) const;
  bool operator!=(const AstNodeListIterator& other) const;

private:
  const AstNodeList* m_list;
  size_t m_index = 0;
};

class CXXAST_API AstRootNode : public AstNode
{
public:
  std::vector<std::shared_ptr<AstNode>> childvec;

public:
  explicit AstRootNode();

  NodeKind node_kind() const override;

  void append(std::shared_ptr<AstNode> n) override;
  AstNodeList children() const override;
};

class CXXAST_API UnexposedAstNode : public AstNode
{
public:
  std::vector<std::shared_ptr<AstNode>> childvec;
  AstNodeKind kind = AstNodeKind::Root;

public:
  explicit UnexposedAstNode(AstNodeKind k);

  NodeKind node_kind() const override;

  void append(std::shared_ptr<AstNode> n) override;
  AstNodeList children() const override;
};

// Forward-declare some utility functions so that they can be used within templates
bool is_null(const Expression& expr);
bool is_null(const Statement& stmt);
std::shared_ptr<AstNode> to_ast_node(const Expression& expr);
std::shared_ptr<AstNode> to_ast_node(const Statement& stmt);

} // namespace cxx

namespace cxx
{

template<typename T>
inline bool INode::is() const
{
  return test_node_kind<T>(*this);
}

template<typename F>
inline typename F::field_type& INode::get()
{
  return F::get(*this);
}

template<typename F, typename Arg>
inline void INode::set(Arg&& arg)
{
  F::set(*this, std::forward<Arg>(arg));
}

} // namespace cxx

#endif // CXXAST_NODE_H
