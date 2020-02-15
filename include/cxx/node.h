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

class Documentation;

class CXXAST_API Node : public std::enable_shared_from_this<Node>
{
private:
  SourceLocation m_location;

public:
  Node() = default;
  virtual ~Node();

  virtual const std::string& type() const = 0;

  virtual bool isEntity() const;

  template<typename T>
  bool is() const;

  const SourceLocation& location() const;
  SourceLocation& location();
};


template<typename T>
bool test_node_type(const Node& n)
{
  return T::TypeId == n.type();
}

} // namespace cxx

namespace cxx
{

template<typename T>
inline bool Node::is() const
{
  return test_node_type<T>(*this);
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
