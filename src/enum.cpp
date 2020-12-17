// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/enum.h"

#include <stdexcept>

namespace cxx
{

EnumValue::EnumValue(std::string name, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent)
{

}

EnumValue::EnumValue(std::string name, std::string value, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent),
    m_value(std::move(value))
{

}

NodeKind EnumValue::node_kind() const
{
  return ClassNodeKind;
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


NodeKind Enum::node_kind() const
{
  return ClassNodeKind;
}

AccessSpecifier Enum::getAccessSpecifier() const
{
  return access_specifier;
}

void Enum::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

} // namespace cxx
