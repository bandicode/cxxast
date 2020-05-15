// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/class.h"

#include <stdexcept>

namespace cxx
{

NodeKind Class::node_kind() const
{
  return ClassNodeKind;
}

size_t Class::childCount() const
{
  return this->members.size();
}

std::shared_ptr<Node> Class::childAt(size_t index) const
{
  return this->members.at(index);
}

void Class::appendChild(std::shared_ptr<Node> n)
{
  if (!n->isEntity())
    throw std::runtime_error{ "bad call Namespace::appendChild()" };

  this->members.push_back(std::static_pointer_cast<Entity>(n));
}

AccessSpecifier Class::getAccessSpecifier() const
{
  return access_specifier;
}

void Class::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

} // namespace cxx
