// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/template.h"

namespace cxx
{

const std::string TemplateParameter::TypeId = "template-parameter";

const std::string& TemplateParameter::type() const
{
  return TypeId;
}

TemplateParameter::TemplateParameter()
  : Entity(""),
    m_is_type_parameter(true)
{
  std::get<TemplateNonTypeParameter>(m_data).type = Type::Auto;
}

TemplateParameter::TemplateParameter(std::string name, Type default_value)
  : Entity(std::move(name)), 
    m_is_type_parameter(false)
{
  std::get<TemplateTypeParameter>(m_data).default_value = default_value;
}

TemplateParameter::TemplateParameter(const Type& type, std::string name, std::string default_value)
  : Entity(std::move(name)), 
    m_is_type_parameter(false)
{
  std::get<TemplateNonTypeParameter>(m_data).type = type;
  std::get<TemplateNonTypeParameter>(m_data).default_value = default_value;
}

bool TemplateParameter::isTypeParameter() const
{
  return m_is_type_parameter;
}

bool TemplateParameter::isNonTypeParameter() const
{
  return !m_is_type_parameter;
}

bool TemplateParameter::hasDefaultValue()
{
  return (isTypeParameter() && std::get<TemplateTypeParameter>(m_data).default_value != Type::Auto) 
    || !std::get<TemplateNonTypeParameter>(m_data).default_value.empty();
}

TemplateArgument::TemplateArgument(std::string expr)
{
  std::get<std::string>(m_data) = expr;
}

TemplateArgument::TemplateArgument(const Type& type)
{
  std::get<Type>(m_data) = type;
}

bool TemplateArgument::isType() const
{
  return std::get<std::string>(m_data).empty();
}

std::string TemplateArgument::toString() const
{
  return isType() ? get<Type>().toString() : get<std::string>();
}

} // namespace cxx
