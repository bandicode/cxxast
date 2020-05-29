// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function-declaration.h"

namespace cxx
{

NodeKind FunctionDeclaration::node_kind() const
{
  return FunctionDeclaration::ClassNodeKind;
}

std::shared_ptr<Entity> FunctionDeclaration::entity() const
{
  return this->function;
}

} // namespace cxx
