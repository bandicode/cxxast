// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/documentation.h"

namespace cxx
{

Documentation::~Documentation()
{

}

const std::string MultilineComment::TypeId = "multiline-comment";

const std::string& MultilineComment::type() const
{
  return TypeId;
}

} // namespace cxx
