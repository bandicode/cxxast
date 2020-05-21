// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/while-loop.h"

#include <stdexcept>

namespace cxx
{

WhileLoop::WhileLoop(const Expression& cond, std::shared_ptr<Statement> b)
  : condition(cond), body(b)
{

}

NodeKind WhileLoop::node_kind() const
{
  return WhileLoop::ClassNodeKind;
}

size_t WhileLoop::childCount() const
{
  return 1;
}

std::shared_ptr<Node> WhileLoop::childAt(size_t index) const
{
  return this->body;
}

void WhileLoop::appendChild(std::shared_ptr<Node> n)
{
  if (!n->isStatement())
    throw std::runtime_error{ "bad call to WhileLoop::appendChild()" };

  this->body = std::static_pointer_cast<Statement>(n);
}

} // namespace cxx
