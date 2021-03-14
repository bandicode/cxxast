// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/switch-statement.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

NodeKind SwitchStatement::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList SwitchStatement::children() const
{
  return make_node_list(value, body);
}

NodeKind CaseStatement::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList CaseStatement::children() const
{
  return make_node_list(value, stmt);
}

NodeKind DefaultStatement::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList DefaultStatement::children() const
{
  return make_node_list(stmt);
}

} // namespace cxx
