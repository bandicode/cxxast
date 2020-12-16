// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NODE_H
#define CXXAST_NODE_H

#include "cxx/sourcelocation.h"

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
  FunctionDeclaration,
  NamespaceDeclaration,
  VariableDeclaration,
  UnexposedStatement,
  /* Misc */
  MultilineComment,
  Documentation,
};

class CXXAST_API Node : public std::enable_shared_from_this<Node>
{
public:
  SourceLocation location;

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

class CXXAST_API AstNode : public Node
{
  using Node::Node;
};

} // namespace cxx

namespace cxx
{

template<typename T>
inline bool Node::is() const
{
  return test_node_kind<T>(*this);
}

} // namespace cxx

#endif // CXXAST_NODE_H
