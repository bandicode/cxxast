// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/declaration.h"

#include "cxx/astnodelist_p.h"
#include "cxx/entity.h"

namespace cxx
{

IDeclaration::IDeclaration(std::shared_ptr<IEntity> e)
  : entity_ptr(e)
{

}

std::shared_ptr<IDeclaration> IDeclaration::shared_from_this()
{
  return std::static_pointer_cast<IDeclaration>(INode::shared_from_this());
}

bool IDeclaration::isDeclaration() const
{
  return true;
}

void IDeclaration::append(std::shared_ptr<AstNode> n)
{
  childvec.push_back(n);
  n->weak_parent = std::static_pointer_cast<AstNode>(shared_from_this());
}

AstNodeList IDeclaration::children() const
{
  return AstNodeList(std::make_shared<AstVectorRefNodeList>(childvec));
}

} // namespace cxx
