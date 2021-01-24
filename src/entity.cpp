// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/entity.h"

#include <stdexcept>

namespace cxx
{

std::shared_ptr<IEntity> IEntity::shared_from_this()
{
  return std::static_pointer_cast<IEntity>(INode::shared_from_this());
}

std::shared_ptr<const IEntity> IEntity::shared_from_this() const
{
  return std::static_pointer_cast<const IEntity>(INode::shared_from_this());
}

bool IEntity::isEntity() const
{
  return true;
}

AccessSpecifier IEntity::getAccessSpecifier() const
{
  return AccessSpecifier::PUBLIC;
}

void IEntity::setAccessSpecifier(AccessSpecifier aspec)
{
  if (aspec != AccessSpecifier::PUBLIC)
    throw std::runtime_error{ "bad call to IEntity::setAccessSpecifier()" };
}

} // namespace cxx
