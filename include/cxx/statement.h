// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_STATEMENT_H
#define CXXAST_STATEMENT_H

#include "cxx/node.h"

#include "cxx/sourcelocation.h"

namespace cxx
{

class CXXAST_API Statement : public Node
{
public:
  Statement() = default;

  std::shared_ptr<Statement> shared_from_this();

  bool isStatement() const override;
};

typedef std::shared_ptr<Statement> StatementPtr;

class CXXAST_API AstStatement : public AstNode
{
public:
  AstStatement() = default;
};

class CXXAST_API UnexposedStatement : public Statement
{
public:
  std::string source; // @TODO: should we remove this ?

public:
  explicit UnexposedStatement(std::string src);

  static constexpr NodeKind ClassNodeKind = NodeKind::UnexposedStatement;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_STATEMENT_H
