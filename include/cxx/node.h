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
class Entity;
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
  AstNode,
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

class CXXAST_API INode : public std::enable_shared_from_this<INode>
{
public:
  INode() = default;
  virtual ~INode();

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

  template<typename T>
  bool is() const { return d->is<T>(); }

  template<typename F>
  typename F::field_type& get() const { return d->get<F>(); }

  template<typename F, typename Arg>
  void set(Arg&& value) { return d->set<F>(std::forward<Arg>(value)); }

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

class CXXAST_API AstNode : public INode
{
public:
  SourceRange sourcerange;
  std::vector<std::shared_ptr<AstNode>> children;
  std::shared_ptr<INode> node_ptr;
  // @TODO: add 'type' field ? would be usefull for tokens

public:
  
  AstNode() = default;
  
  explicit AstNode(const SourceRange& sr)
    : sourcerange(sr)
  {

  }

  explicit AstNode(const SourceRange& sr, std::shared_ptr<INode> n)
    : sourcerange(sr),
      node_ptr(std::move(n))
  {

  }

  NodeKind node_kind() const override;
};

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
