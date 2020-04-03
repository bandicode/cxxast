// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/documentation.h"

namespace cxx
{

Documentation::~Documentation()
{

}

NodeKind Documentation::node_kind() const
{
  return NodeKind::Documentation;
}

bool Documentation::isDocumentation() const
{
  return true;
}

NodeKind MultilineComment::node_kind() const
{
  return ClassNodeKind;
}

} // namespace cxx
