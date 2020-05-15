// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function.h"

namespace cxx
{

FunctionParameter::FunctionParameter(Type t, std::string name, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
    type(std::move(t))
{

}

FunctionParameter::FunctionParameter(Type t, std::string name, Expression default_val, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
    type(std::move(t)),
    default_value(std::move(default_val))
{

}

NodeKind FunctionParameter::node_kind() const
{
  return ClassNodeKind;
}

std::shared_ptr<Function> FunctionParameter::parent() const
{
  return std::static_pointer_cast<Function>(Entity::parent());
}

NodeKind Function::node_kind() const
{
  return ClassNodeKind;
}

AccessSpecifier Function::getAccessSpecifier() const
{
  return access_specifier;
}

void Function::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

} // namespace cxx
