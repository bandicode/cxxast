// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/compound-statement.h"

#include <stdexcept>

namespace cxx
{

CompoundStatement::CompoundStatement(std::vector<StatementPtr> stmts)
  : statements(std::move(stmts))
{

}

NodeKind CompoundStatement::node_kind() const
{
  return CompoundStatement::ClassNodeKind;
}

size_t CompoundStatement::childCount() const
{
  return this->statements.size();
}

std::shared_ptr<Node> CompoundStatement::childAt(size_t index) const
{
  return this->statements.at(index);
}

void CompoundStatement::appendChild(std::shared_ptr<Node> n)
{
  if (!n->isStatement())
    throw std::runtime_error{ "bad call to CompoundStatement::appendChild()" };

  this->statements.push_back(std::static_pointer_cast<Statement>(n));
}

} // namespace cxx
