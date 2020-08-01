// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/entity.h"

#include <stdexcept>

namespace cxx
{

std::shared_ptr<Entity> Entity::shared_from_this()
{
  return std::static_pointer_cast<Entity>(Node::shared_from_this());
}

std::shared_ptr<const Entity> Entity::shared_from_this() const
{
  return std::static_pointer_cast<const Entity>(Node::shared_from_this());
}

bool Entity::isEntity() const
{
  return true;
}

AccessSpecifier Entity::getAccessSpecifier() const
{
  return AccessSpecifier::PUBLIC;
}

void Entity::setAccessSpecifier(AccessSpecifier aspec)
{
  if (aspec != AccessSpecifier::PUBLIC)
    throw std::runtime_error{ "bad call to Entity::setAccessSpecifier()" };
}

} // namespace cxx
