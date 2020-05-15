// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/declaration.h"

namespace cxx
{

std::shared_ptr<Declaration> Declaration::shared_from_this()
{
  return std::static_pointer_cast<Declaration>(Node::shared_from_this());
}

bool Declaration::isDeclaration() const
{
  return true;
}

} // namespace cxx
