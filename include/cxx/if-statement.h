// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_IFSTATEMENT_H
#define CXXAST_IFSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

#include <vector>

namespace cxx
{

class CXXAST_API IfStatement : public Statement
{
public:
  StatementPtr initialization;
  Expression condition;
  StatementPtr body;
  StatementPtr else_clause;
  
public:
  IfStatement() = default;

  IfStatement(const Expression& expr, StatementPtr b);

  static constexpr NodeKind ClassNodeKind = NodeKind::IfStatement;
  NodeKind node_kind() const override;

  struct Init : priv::FieldEx<IfStatement, StatementPtr, &IfStatement::initialization> { };
  struct Condition : priv::FieldEx<IfStatement, Expression, &IfStatement::condition> { };
  struct Body : priv::FieldEx<IfStatement, StatementPtr, &IfStatement::body> { };
  struct ElseClause : priv::FieldEx<IfStatement, StatementPtr, &IfStatement::else_clause> { };
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_IFSTATEMENT_H
