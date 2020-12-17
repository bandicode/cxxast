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

} // namespace cxx
