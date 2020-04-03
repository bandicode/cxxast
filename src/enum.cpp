// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/enum.h"

namespace cxx
{

const std::string Enum::TypeId = "enum";
const std::string EnumValue::TypeId = "enum-value";

const std::string& Enum::type() const
{
  return TypeId;
}

EnumValue::EnumValue(std::string name, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent)
{

}

EnumValue::EnumValue(std::string name, std::string value, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent),
    m_value(std::move(value))
{

}

const std::string& EnumValue::type() const
{
  return TypeId;
}

std::shared_ptr<Enum> EnumValue::parent() const
{
  return std::static_pointer_cast<Enum>(Entity::parent());
}

std::string& EnumValue::value()
{
  return m_value;
}

const std::string& EnumValue::value() const
{
  return m_value;
}

} // namespace cxx
