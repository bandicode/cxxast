// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/if-statement.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

IfStatement::IfStatement(const Expression& expr, Statement b)
  : condition(expr), body(b)
{

}

NodeKind IfStatement::node_kind() const
{
  return IfStatement::ClassNodeKind;
}

AstNodeList IfStatement::children() const
{
  return make_node_list(initialization, condition, body, else_clause);
}

} // namespace cxx
