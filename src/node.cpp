// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/node.h"

#include "cxx/entity.h"
#include "cxx/statement.h"

#include <stdexcept>

namespace cxx
{

INode::~INode()
{

}

bool INode::isEntity() const
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

Entity Handle::toEntity() const
{
  return isEntity() ? Entity{ std::static_pointer_cast<IEntity>(node_ptr) } : Entity{};
}

Statement Handle::toStatement() const
{
  return isStatement() ? Statement{ std::static_pointer_cast<IStatement>(node_ptr) } : Statement{};
}

NodeKind AstNode::node_kind() const
{
  return NodeKind::AstNode;
}

} // namespace cxx