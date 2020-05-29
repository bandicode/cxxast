// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/declaration.h"

#include <stdexcept>

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

size_t Declaration::childCount() const
{
  return this->declarations.size();
}

std::shared_ptr<Node> Declaration::childAt(size_t index) const
{
  return this->declarations.at(index);
}

void Declaration::appendChild(std::shared_ptr<Node> n)
{
  if (!n->isDeclaration())
    throw std::runtime_error{ "bad call to Declaration::appendChild()" };

  this->declarations.push_back(std::static_pointer_cast<Declaration>(n));
}

} // namespace cxx
