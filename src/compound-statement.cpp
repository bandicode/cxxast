// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/compound-statement.h"

#include <stdexcept>

namespace cxx
{

CompoundStatement::CompoundStatement(std::vector<Statement> stmts)
  : statements(std::move(stmts))
{

}

NodeKind CompoundStatement::node_kind() const
{
  return CompoundStatement::ClassNodeKind;
}

} // namespace cxx
