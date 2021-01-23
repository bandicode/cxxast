// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/statement.h"

namespace cxx
{

std::shared_ptr<Statement> Statement::shared_from_this()
{
  return std::static_pointer_cast<Statement>(Node::shared_from_this());
}

bool Statement::isStatement() const
{
  return true;
}

UnexposedStatement::UnexposedStatement(std::string src) :
  source(std::move(src))
{

}

NodeKind UnexposedStatement::node_kind() const
{
  return UnexposedStatement::ClassNodeKind;
}

} // namespace cxx
