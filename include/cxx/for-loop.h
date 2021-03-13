// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FORLOOP_H
#define CXXAST_FORLOOP_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API ForLoop : public IStatement
{
public:
  Statement init;
  Expression condition;
  Expression iter;
  Statement body;
  
public:
  ForLoop() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::ForLoop;
  NodeKind node_kind() const override;

  struct Init : priv::FieldEx<ForLoop, Statement, &ForLoop::init> { };
  struct Condition : priv::FieldEx<ForLoop, Expression, &ForLoop::condition> { };
  struct Iter : priv::FieldEx<ForLoop, Expression, &ForLoop::iter> { };
  struct Body : priv::FieldEx<ForLoop, Statement, &ForLoop::body> { };

  AstNodeList children() const override;
};

class CXXAST_API ForRange : public IStatement
{
public:
  Statement variable;
  Expression container;
  Statement body;

public:
  ForRange() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::ForRange;
  NodeKind node_kind() const override;

  struct Variable : priv::FieldEx<ForRange, Statement, &ForRange::variable> { };
  struct Container : priv::FieldEx<ForRange, Expression, &ForRange::container> { };
  struct Body : priv::FieldEx<ForRange, Statement, &ForRange::body> { };

  AstNodeList children() const override;
};

} // namespace cxx

#endif // CXXAST_FORLOOP_H
