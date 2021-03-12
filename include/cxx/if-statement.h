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

class CXXAST_API IfStatement : public IStatement
{
public:
  Statement initialization;
  Expression condition;
  Statement body;
  Statement else_clause;
  
public:
  IfStatement() = default;

  IfStatement(const Expression& expr, Statement b);

  static constexpr NodeKind ClassNodeKind = NodeKind::IfStatement;
  NodeKind node_kind() const override;

  struct Init : priv::FieldEx<IfStatement, Statement, &IfStatement::initialization> { };
  struct Condition : priv::FieldEx<IfStatement, Expression, &IfStatement::condition> { };
  struct Body : priv::FieldEx<IfStatement, Statement, &IfStatement::body> { };
  struct ElseClause : priv::FieldEx<IfStatement, Statement, &IfStatement::else_clause> { };

  AstNodeList children() const override;
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_IFSTATEMENT_H
