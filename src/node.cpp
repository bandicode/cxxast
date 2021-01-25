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

Entity Node::toEntity() const
{
  return isEntity() ? Entity{ std::static_pointer_cast<IEntity>(d) } : Entity{};
}

Statement Node::toStatement() const
{
  return isStatement() ? Statement{ std::static_pointer_cast<IStatement>(d) } : Statement{};
}

NodeKind AstNode::node_kind() const
{
  return NodeKind::AstNode;
}

void AstNode::updateSourceRange()
{
  if (children.empty())
    return;

  sourcerange = children.front()->sourcerange;
  sourcerange.end = children.back()->sourcerange.end;
}

} // namespace cxx