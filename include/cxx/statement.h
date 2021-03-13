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
  IStatement() = default;

  std::shared_ptr<IStatement> shared_from_this();

  bool isStatement() const override;
};

typedef std::shared_ptr<IStatement> IStatementPtr;

class CXXAST_API Statement : public Handle<IStatement>
{
public:
  
  using Handle<IStatement>::Handle;

  bool isDeclaration() const { return d->isDeclaration(); }
};

class CXXAST_API UnexposedStatement : public IStatement
{
public:
  std::vector<std::shared_ptr<AstNode>> childvec;
  AstNodeKind kind = AstNodeKind::Root;

public:
  UnexposedStatement();
  explicit UnexposedStatement(AstNodeKind k);

  static constexpr NodeKind ClassNodeKind = NodeKind::UnexposedStatement;
  NodeKind node_kind() const override;

  void append(std::shared_ptr<AstNode> n) override;
  AstNodeList children() const override;
};

inline bool is_null(const Statement& stmt)
{
  return stmt.isNull();
}

inline std::shared_ptr<AstNode> to_ast_node(const Statement& stmt)
{
  return stmt.impl();
}

} // namespace cxx

#endif // CXXAST_STATEMENT_H
