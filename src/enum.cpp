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

size_t Enum::childCount() const
{
  return this->values.size();
}

std::shared_ptr<Node> Enum::childAt(size_t index) const
{
  return this->values.at(index);
}

void Enum::appendChild(std::shared_ptr<Node> n)
{
  if (!n->is<EnumValue>())
    throw std::runtime_error{ "bad call Namespace::appendChild()" };

  this->values.push_back(std::static_pointer_cast<EnumValue>(n));
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
