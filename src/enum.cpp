// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/enum.h"

namespace cxx
{

const std::string Enum::TypeId = "enum";

const std::string& Enum::type() const
{
  return TypeId;
}

} // namespace cxx
