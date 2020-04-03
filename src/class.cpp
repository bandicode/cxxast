// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/class.h"

namespace cxx
{

NodeKind Class::node_kind() const
{
  return ClassNodeKind;
}

} // namespace cxx
