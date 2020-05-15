// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/macro.h"

#include <stdexcept>

namespace cxx
{

NodeKind Macro::node_kind() const
{
  return Macro::ClassNodeKind;
}

} // namespace cxx
