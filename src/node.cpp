// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/node.h"

#include <stdexcept>

namespace cxx
{

Node::~Node()
{

}

bool Node::isEntity() const
{
  return false;
}

bool Node::isDocumentation() const
{
  return false;
}

bool Node::isStatement() const
{
  return false;
}

bool Node::isDeclaration() const
{
  return false;
}

} // namespace cxx