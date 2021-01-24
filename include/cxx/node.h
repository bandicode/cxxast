// Copyright (C) 2020 Vincent Chambrin
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
  CompoundStatement,
  IfStatement,
  WhileLoop,
  ClassDeclaration,
  EnumDeclaration,
  //FunctionDeclaration,
  //NamespaceDeclaration,
  VariableDeclaration,
  UnexposedStatement,
  /*Ast */
  AstDeclaration,
  /* Misc */
  MultilineComment,
  Documentation,
};

namespace priv
{

struct CXXAST_API IField
{

};

} // namespace priv

class CXXAST_API Node : public std::enable_shared_from_this<Node>
{
public:
  Node() = default;
  virtual ~Node();

  virtual NodeKind node_kind() const = 0;
  NodeKind kind() const { return node_kind(); }

  virtual bool isEntity() const;
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

class CXXAST_API Handle
{
public:
  std::shared_ptr<Node> node_ptr;

public:
  Handle() = default;
  Handle(const Handle&) = default;
  ~Handle() = default;

  explicit Handle(std::shared_ptr<Node> n)
    : node_ptr(std::move(n))
  {

  }

  NodeKind kind() const { return node_ptr->node_kind(); }

  bool isEntity() const { return node_ptr->isEntity(); }
  bool isDocumentation() const { return node_ptr->isDocumentation(); }
  bool isStatement() const { return node_ptr->isStatement(); }
  bool isDeclaration() const { return node_ptr->isDeclaration(); }

  template<typename T>
  bool is() const { return node_ptr->is<T>(); }

  template<typename F>
  typename F::field_type& get() const { return node_ptr->get<F>(); }

  template<typename F, typename Arg>
  void set(Arg&& value) { return node_ptr->set<F>(std::forward<Arg>(value)); }

  Handle& operator=(const Handle&) = default;
};

template<typename T>
bool test_node_kind(const Node& n)
{
  return T::ClassNodeKind == n.kind();
}

template<>
inline bool test_node_kind<Class>(const Node& n)
{
  return n.kind() == NodeKind::Class || n.kind() == NodeKind::ClassTemplate;
}

template<>
inline bool test_node_kind<Function>(const Node& n)
{
  return n.kind() == NodeKind::Function || n.kind() == NodeKind::FunctionTemplate;
}

namespace priv
{

template<typename T>
struct FieldOfClass : public IField
{
  static T& down_cast(Node& self)
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

  static field_type& get(Node& n)
  {
    return FieldOfClass<C>::down_cast(n).*member;
  }

  static void set(Node& n, field_type val)
  {
    FieldOfClass<C>::down_cast(n).*member = std::move(val);
  }
};

} // namespace priv

class CXXAST_API AstNode : public Node
{
public:
  SourceRange sourcerange;
  std::vector<std::shared_ptr<AstNode>> children;
  std::shared_ptr<Node> node_ptr;

public:
  
  AstNode() = default;
  
  explicit AstNode(const SourceRange& sr)
    : sourcerange(sr)
  {

  }
};

} // namespace cxx

namespace cxx
{

template<typename T>
inline bool Node::is() const
{
  return test_node_kind<T>(*this);
}

template<typename F>
inline typename F::field_type& Node::get()
{
  return F::get(*this);
}

template<typename F, typename Arg>
inline void Node::set(Arg&& arg)
{
  F::set(*this, std::forward<Arg>(arg));
}

} // namespace cxx

#endif // CXXAST_NODE_H
