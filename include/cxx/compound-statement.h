// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_COMPOUNDSTATEMENT_H
#define CXXAST_COMPOUNDSTATEMENT_H

#include "cxx/statement.h"

#include <vector>

namespace cxx
{

class CXXAST_API CompoundStatement : public Statement
{
public:
  std::vector<StatementPtr> statements;
  
public:
  CompoundStatement() = default;

  explicit CompoundStatement(std::vector<StatementPtr> stmts);

  static constexpr NodeKind ClassNodeKind = NodeKind::CompoundStatement;
  NodeKind node_kind() const override;

  struct Statements : public priv::Field<CompoundStatement, std::vector<StatementPtr>>
  {
    static field_type& get(Node& n)
    {
      return down_cast(n).statements;
    }

    static void set(Node& n, field_type val)
    {
      down_cast(n).statements = std::move(val);
    }
  };
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_COMPOUNDSTATEMENT_H
