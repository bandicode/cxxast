// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NULLSTATEMENT_H
#define CXXAST_NULLSTATEMENT_H

#include "cxx/expression.h"
#include "cxx/statement.h"

#include <vector>

namespace cxx
{

class CXXAST_API NullStatement : public IStatement
{  
public:
  NullStatement() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::NullStatement;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_NULLSTATEMENT_H
