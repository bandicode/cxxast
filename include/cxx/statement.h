// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_STATEMENT_H
#define CXXAST_STATEMENT_H

#include "cxx/node.h"

#include "cxx/sourcelocation.h"

namespace cxx
{

class CXXAST_API IStatement : public AstNode
{
public:
  std::vector<std::shared_ptr<AstNode>> childvec;

public:
  IStatement() = default;

  std::shared_ptr<IStatement> shared_from_this();

  bool isStatement() const override;

  void append(std::shared_ptr<AstNode> n) override;
  AstNodeList children() const override;
};

typedef std::shared_ptr<IStatement> IStatementPtr;

class CXXAST_API Statement : public Handle<IStatement>
{
protected:
  std::shared_ptr<IStatement> d;

public:
  
  using Handle<IStatement>::Handle;

  NodeKind kind() const { return d->node_kind(); }

  bool isDeclaration() const { return d->isDeclaration(); }
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
