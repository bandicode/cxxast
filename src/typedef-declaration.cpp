// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/typedef-declaration.h"

namespace cxx
{

NodeKind TypedefDeclaration::node_kind() const
{
  return ClassNodeKind;
}

} // namespace cxx
