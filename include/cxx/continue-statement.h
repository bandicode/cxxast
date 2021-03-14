// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CONTINUESTATEMENT_H
#define CXXAST_CONTINUESTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API ContinueStatement : public IStatement
{
public:
  ContinueStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::ContinueStatement;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_CONTINUESTATEMENT_H
