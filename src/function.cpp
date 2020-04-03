// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function.h"

namespace cxx
{

const std::string Function::TypeId = "function";
const std::string FunctionParameter::TypeId = "function-parameter";

const std::string& Function::type() const
{
  return TypeId;
}

FunctionParameter::FunctionParameter(Type type, std::string name, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
  m_type(std::move(type))
{

}

FunctionParameter::FunctionParameter(Type type, std::string name, std::string default_falue, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
  m_type(std::move(type)),
  m_default_value(std::move(default_falue))
{

}


const std::string& FunctionParameter::type() const
{
  return TypeId;
}

std::shared_ptr<Function> FunctionParameter::parent() const
{
  return std::static_pointer_cast<Function>(Entity::parent());
}

Type& FunctionParameter::parameterType()
{
  return m_type;
}

const Type& FunctionParameter::parameterType() const
{
  return m_type;
}

std::string& FunctionParameter::defaultValue()
{
  return m_default_value;
}

const std::string& FunctionParameter::defaultValue() const
{
  return m_default_value;
}

} // namespace cxx
