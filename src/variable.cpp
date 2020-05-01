// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/variable.h"

namespace cxx
{

NodeKind Variable::node_kind() const
{
  return ClassNodeKind;
}

std::string& Variable::defaultValue()
{
  return m_default_value;
}

const std::string& Variable::defaultValue() const
{
  return m_default_value;
}

int& Variable::specifiers()
{
  return m_flags;
}

int Variable::specifiers() const
{
  return m_flags;
}

} // namespace cxx
