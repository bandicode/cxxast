// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/return-statement.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

ReturnStatement::ReturnStatement(const Expression& expr_)
  : expr(expr_)
{

}

NodeKind ReturnStatement::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList ReturnStatement::children() const
{
  return make_node_list(expr);
}

} // namespace cxx
