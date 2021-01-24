// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/declaration.h"

#include "cxx/entity.h"

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

std::shared_ptr<Entity> Declaration::entity() const
{
  return nullptr;
}

AstDeclaration::AstDeclaration(const std::shared_ptr<Entity>& ent)
{
  node_ptr = ent;
}

} // namespace cxx
