// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/variable-declaration.h"

namespace cxx
{

NodeKind VariableDeclaration::node_kind() const
{
  return VariableDeclaration::ClassNodeKind;
}

} // namespace cxx
