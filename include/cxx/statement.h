// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_STATEMENT_H
#define CXXAST_STATEMENT_H

#include "cxx/node.h"

#include "cxx/sourcelocation.h"

namespace cxx
{

class CXXAST_API IStatement : public INode
{
public:
  IStatement() = default;

  std::shared_ptr<IStatement> shared_from_this();

  bool isStatement() const override;
};

typedef std::shared_ptr<IStatement> IStatementPtr;

class CXXAST_API Statement
{
protected:
  std::shared_ptr<IStatement> d;

public:
  Statement() = default;
  Statement(const Statement&) = default;
  ~Statement() = default;

  Statement(std::shared_ptr<IStatement> impl)
    : d(std::move(impl))
  {

  }

  bool isNull() const { return d == nullptr; }

  NodeKind kind() const { return d->node_kind(); }

  bool isDeclaration() const { return d->isDeclaration(); }

  template<typename T>
  bool is() const { return d->is<T>(); }

  template<typename F>
  typename F::field_type& get() const { return d->get<F>(); }

  template<typename F, typename Arg>
  void set(Arg&& value) { return d->set<F>(std::forward<Arg>(value)); }

  const std::shared_ptr<IStatement>& impl() const { return d; }

  Statement& operator=(const Statement&) = default;
};

inline bool operator==(const Statement& lhs, const Statement& rhs)
{
  return lhs.impl() == rhs.impl();
}

inline bool operator!=(const Statement& lhs, const Statement& rhs)
{
  return lhs.impl() != rhs.impl();
}

class CXXAST_API AstStatement : public AstNode
{
public:
  AstStatement() = default;
};

class CXXAST_API UnexposedStatement : public IStatement
{
public:
  UnexposedStatement();

  static constexpr NodeKind ClassNodeKind = NodeKind::UnexposedStatement;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_STATEMENT_H
