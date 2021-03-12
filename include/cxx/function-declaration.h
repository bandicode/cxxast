// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FUNCTION_DECLARATION_H
#define CXXAST_FUNCTION_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/function.h"

namespace cxx
{

class CXXAST_API FunctionDeclaration : public IDeclaration
{
public:
  std::shared_ptr<Function> function;

public:
  FunctionDeclaration() = default;

  explicit FunctionDeclaration(std::shared_ptr<Function> f);

  static constexpr NodeKind ClassNodeKind = NodeKind::FunctionDeclaration;
  NodeKind node_kind() const override;

  // bool isForwardDeclaration() const;
};

} // namespace cxx

namespace cxx
{

inline FunctionDeclaration::FunctionDeclaration(std::shared_ptr<Function> f)
  : IDeclaration(f),
    function(f)
{

}

} // namespace cxx

#endif // CXXAST_FUNCTION_DECLARATION_H
