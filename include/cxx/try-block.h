// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TRYBLOCK_H
#define CXXAST_TRYBLOCK_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API TryBlock : public IStatement
{
public:
  Statement body;
  std::vector<Statement> handlers;
  
public:
  TryBlock() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::TryBlock;
  NodeKind node_kind() const override;

  struct Body : priv::FieldEx<TryBlock, Statement, &TryBlock::body> { };
  struct Handlers : priv::FieldEx<TryBlock, std::vector<Statement>, &TryBlock::handlers> { };

  AstNodeList children() const override;
};

class CXXAST_API CatchStatement : public IStatement
{
public:
  Statement var;
  Statement body;

public:
  CatchStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::CatchStatement;
  NodeKind node_kind() const override;

  struct Var : priv::FieldEx<CatchStatement, Statement, &CatchStatement::var> { };
  struct Body : priv::FieldEx<CatchStatement, Statement, &CatchStatement::body> { };

  AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_TRYBLOCK_H
