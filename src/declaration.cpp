// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/declaration.h"

#include "cxx/entity.h"

namespace cxx
{

std::shared_ptr<IDeclaration> IDeclaration::shared_from_this()
{
  return std::static_pointer_cast<IDeclaration>(INode::shared_from_this());
}

bool IDeclaration::isDeclaration() const
{
  return true;
}

std::shared_ptr<IEntity> IDeclaration::entity() const
{
  return nullptr;
}

AstDeclaration::AstDeclaration(const std::shared_ptr<IEntity>& ent)
{
  node_ptr = ent;
}

} // namespace cxx
