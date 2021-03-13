// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_SWITCHSTATEMENT_H
#define CXXAST_SWITCHSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API SwitchStatement : public IStatement
{
public:
  Expression value;
  Statement body;

public:
  SwitchStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::SwitchStatement;
  NodeKind node_kind() const override;

  struct Value : priv::FieldEx<SwitchStatement, Expression, &SwitchStatement::value> { };
  struct Body : priv::FieldEx<SwitchStatement, Statement, &SwitchStatement::body> { };

  AstNodeList children() const override;
};

class CXXAST_API CaseStatement : public IStatement
{
public:
  Expression value;
  Statement stmt;

public:
  CaseStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::CaseStatement;
  NodeKind node_kind() const override;

  struct Value : priv::FieldEx<CaseStatement, Expression, &CaseStatement::value> { };
  struct Stmt : priv::FieldEx<CaseStatement, Statement, &CaseStatement::stmt> { };

  AstNodeList children() const override;
};

class CXXAST_API DefaultStatement : public IStatement
{
public:
  Statement stmt;

public:
  DefaultStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::DefaultStatement;
  NodeKind node_kind() const override;

  struct Stmt : priv::FieldEx<DefaultStatement, Statement, &DefaultStatement::stmt> { };

  AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_SWITCHSTATEMENT_H
