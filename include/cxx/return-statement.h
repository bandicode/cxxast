// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_RETURNSTATEMENT_H
#define CXXAST_RETURNSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API ReturnStatement : public IStatement
{
public:
  Expression expr;
  
public:
  ReturnStatement() = default;

  explicit ReturnStatement(const Expression& expr);

  static constexpr NodeKind ClassNodeKind = NodeKind::ReturnStatement;
  NodeKind node_kind() const override;

  struct Expr : priv::FieldEx<ReturnStatement, Expression, &ReturnStatement::expr> { };

  AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_RETURNSTATEMENT_H
