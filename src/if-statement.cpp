// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/if-statement.h"

#include <stdexcept>

namespace cxx
{

IfStatement::IfStatement(const Expression& expr, StatementPtr b)
  : condition(expr), body(b)
{

}

NodeKind IfStatement::node_kind() const
{
  return IfStatement::ClassNodeKind;
}

size_t IfStatement::childCount() const
{
  return 3;
}

std::shared_ptr<Node> IfStatement::childAt(size_t index) const
{
  if (index == 0)
    return this->initialization;
  else if (index == 1)
    return this->body;
  else
    return this->else_clause;
}

void IfStatement::appendChild(std::shared_ptr<Node> n)
{
  throw std::runtime_error{ "bad call to IfStatement::appendChild()" };
}

} // namespace cxx
