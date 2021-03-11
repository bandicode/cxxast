// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function-body.h"

#include <stdexcept>

namespace cxx
{

std::shared_ptr<INode> FunctionBody::parent() const
{
  return function_ptr.lock();
}

} // namespace cxx
