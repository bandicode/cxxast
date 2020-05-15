// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/typedef.h"

#include <stdexcept>

namespace cxx
{

NodeKind Typedef::node_kind() const
{
  return Typedef::ClassNodeKind;
}

AccessSpecifier Typedef::getAccessSpecifier() const
{
  return access_specifier;
}

void Typedef::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

} // namespace cxx
