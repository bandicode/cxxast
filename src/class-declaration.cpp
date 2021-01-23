// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/class-declaration.h"

namespace cxx
{

NodeKind ClassDeclaration::node_kind() const
{
  return ClassDeclaration::ClassNodeKind;
}

std::shared_ptr<Entity> ClassDeclaration::entity() const
{
  return this->class_;
}

//bool ClassDeclaration::isForwardDeclaration() const
//{
//  return this->class_->location != this->location;
//}

} // namespace cxx
