// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/while-loop.h"

#include <stdexcept>

namespace cxx
{

WhileLoop::WhileLoop(const Expression& cond, std::shared_ptr<IStatement> b)
  : condition(cond), body(b)
{

}

NodeKind WhileLoop::node_kind() const
{
  return WhileLoop::ClassNodeKind;
}

} // namespace cxx
