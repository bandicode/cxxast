// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/enum-declaration.h"

namespace cxx
{

NodeKind EnumDeclaration::node_kind() const
{
  return EnumDeclaration::ClassNodeKind;
}

} // namespace cxx
