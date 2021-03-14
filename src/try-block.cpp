// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/try-block.h"

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

NodeKind TryBlock::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList TryBlock::children() const
{
  std::vector<std::shared_ptr<AstNode>> nodes;
  
  if (!cxx::is_null(body))
    nodes.push_back(cxx::to_ast_node(body));

  for (auto h : handlers)
  {
    if (!cxx::is_null(h))
      nodes.push_back(cxx::to_ast_node(h));
  }

  return AstNodeList{ std::make_shared<AstVectorNodeList>(std::move(nodes)) };
}

NodeKind CatchStatement::node_kind() const
{
  return ClassNodeKind;
}

AstNodeList CatchStatement::children() const
{
  return cxx::make_node_list(var, body);
}


} // namespace cxx
