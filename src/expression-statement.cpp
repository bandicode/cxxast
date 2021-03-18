// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/expression-statement.h"

namespace cxx
{

NodeKind ExpressionStatement::node_kind() const
{
  return ClassNodeKind;
}

} // namespace cxx
