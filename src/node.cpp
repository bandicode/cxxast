// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/node.h"

namespace cxx
{

Node::~Node()
{

}

bool Node::isEntity() const
{
  return false;
}

} // name