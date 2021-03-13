// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DOWHILELOOP_H
#define CXXAST_DOWHILELOOP_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API DoWhileLoop : public IStatement
{
public:
  Statement body;
  Expression condition;
  
public:
  DoWhileLoop() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::DoWhileLoop;
  NodeKind node_kind() const override;

  struct Body : priv::FieldEx<DoWhileLoop, Statement, &DoWhileLoop::body> { };
  struct Condition : priv::FieldEx<DoWhileLoop, Expression, &DoWhileLoop::condition> { };

  AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_DOWHILELOOP_H
