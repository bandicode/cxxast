// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_EXPRESSIONSTATEMENT_H
#define CXXAST_EXPRESSIONSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API ExpressionStatement : public IStatement
{
public:
  Expression expr;

public:
  ExpressionStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::ExpressionStatement;
  NodeKind node_kind() const override;

  struct Expr : priv::FieldEx<ExpressionStatement, Expression, &ExpressionStatement::expr> { };

  //AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_EXPRESSIONSTATEMENT_H
