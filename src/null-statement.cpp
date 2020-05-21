// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/null-statement.h"

#include <stdexcept>

namespace cxx
{

NodeKind NullStatement::node_kind() const
{
  return NullStatement::ClassNodeKind;
}

} // namespace cxx
