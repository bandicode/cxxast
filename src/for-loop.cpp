// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/for-loop.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

NodeKind ForLoop::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList ForLoop::children() const
{
  return make_node_list(init, condition, iter, body);
}

NodeKind ForRange::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList ForRange::children() const
{
  return make_node_list(variable, container, body);
}


} // namespace cxx
