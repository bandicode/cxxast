// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/do-while-loop.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

NodeKind DoWhileLoop::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList DoWhileLoop::children() const
{
  return make_node_list(body, condition);
}

} // namespace cxx
