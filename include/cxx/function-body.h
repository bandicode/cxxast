// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FUNCTIONBODY_H
#define CXXAST_FUNCTIONBODY_H

#include "cxx/compound-statement.h"
#include "cxx/function.h"

#include <vector>

namespace cxx
{

class CXXAST_API FunctionBody : public CompoundStatement
{
public:
  std::weak_ptr<Function> function_ptr;

public:
  explicit FunctionBody(std::shared_ptr<Function> f);

  std::shared_ptr<INode> parent() const override;
};

} // namespace cxx

namespace cxx
{

inline FunctionBody::FunctionBody(std::shared_ptr<Function> f)
  : function_ptr(f)
{

}

} // namespace cxx

#endif // CXXAST_FUNCTIONBODY_H
