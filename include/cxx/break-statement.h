// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_BREAKSTATEMENT_H
#define CXXAST_BREAKSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API BreakStatement : public IStatement
{
public:
  BreakStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::BreakStatement;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_BREAKSTATEMENT_H
