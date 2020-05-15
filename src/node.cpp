// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/node.h"

#include <stdexcept>

namespace cxx
{

Node::~Node()
{

}

bool Node::isEntity() const
{
  return false;
}

bool Node::isDocumentation() const
{
  return false;
}

bool Node::isStatement() const
{
  return false;
}

bool Node::isDeclaration() const
{
  return false;
}

size_t Node::childCount() const
{
  return 0;
}

std::shared_ptr<Node> Node::childAt(size_t) const
{
  throw std::runtime_error{ "bad call to Node::childAt()" };
}

void Node::appendChild(std::shared_ptr<Node>)
{
  throw std::runtime_error{ "bad call to Node::appendChild()" };
}

} // namespace cxx