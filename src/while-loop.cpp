// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/while-loop.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

WhileLoop::WhileLoop(const Expression& cond, const Statement& b)
  : condition(cond), body(b)
{

}

NodeKind WhileLoop::node_kind() const
{
  return WhileLoop::ClassNodeKind;
}

AstNodeList WhileLoop::children() const
{
  return make_node_list(condition, body);
}

} // namespace cxx
