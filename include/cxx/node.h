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

enum class NodeKind
{
  Class,
  Enum,
  EnumValue,
  Function,
  FunctionParameter,
  Namespace,
  TemplateParameter,
  TranslationUnit,
  MultilineComment,
  Documentation,
};

class CXXAST_API Node : public std::enable_shared_from_this<Node>
{
private:
  SourceLocation m_location;

public:
  Node() = default;
  virtual ~Node();

  virtual NodeKind node_kind() const = 0;
  NodeKind kind() const { return node_kind(); }

  virtual bool isEntity() const;
  virtual bool isDocumentation() const;

  template<typename T>
  bool is() const;

  const SourceLocation& location() const;
  SourceLocation& location();
};


template<typename T>
bool test_node_kind(const Node& n)
{
  return T::ClassNodeKind == n.kind();
}

} // namespace cxx

namespace cxx
{

template<typename T>
inline bool Node::is() const
{
  return test_node_kind<T>(*this);
}

inline const SourceLocation& Node::location() const
{
  return m_location;
}

inline SourceLocation& Node::location()
{
  return m_location;
}

} // namespace cxx

#endif // CXXAST_NODE_H
