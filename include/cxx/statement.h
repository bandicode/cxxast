// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_STATEMENT_H
#define CXXAST_STATEMENT_H

#include "cxx/node.h"

#include "cxx/sourcelocation.h"

namespace cxx
{

class CXXAST_API Statement : public AstNode
{
public:
  Statement() = default;

  explicit Statement(SourceLocation loc);

  std::shared_ptr<Statement> shared_from_this();

  bool isStatement() const override;
};

typedef std::shared_ptr<Statement> StatementPtr;

} // namespace cxx

namespace cxx
{

inline Statement::Statement(SourceLocation loc)
{
  location() = loc;
}

} // namespace cxx

#endif // CXXAST_STATEMENT_H
