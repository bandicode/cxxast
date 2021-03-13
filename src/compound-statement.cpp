// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/compound-statement.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

class CompoundStatementChildList : public AstNodeListInterface
{
public:
  explicit CompoundStatementChildList(const std::vector<Statement>& vec)
    : m_vec(vec)
  {

  }

  size_t size() const override
  {
    return m_vec.size();
  }

  std::shared_ptr<AstNode> at(size_t index) const override
  {
    return m_vec.at(index).impl();
  }

private:
  const std::vector<Statement>& m_vec;
};

CompoundStatement::CompoundStatement(std::vector<Statement> stmts)
  : statements(std::move(stmts))
{

}

NodeKind CompoundStatement::node_kind() const
{
  return CompoundStatement::ClassNodeKind;
}

AstNodeList CompoundStatement::children() const
{
  return AstNodeList(std::make_shared<CompoundStatementChildList>(statements));
}

} // namespace cxx
