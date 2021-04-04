// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_PARAMETER_DECLARATION_H
#define CXXAST_PARAMETER_DECLARATION_H

#include "cxx/declaration.h"

#include "cxx/expression.h"
#include "cxx/type.h"

namespace cxx
{

class CXXAST_API ParameterDeclaration : public IDeclaration
{
public:
  ParameterDeclaration() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::ParameterDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_PARAMETER_DECLARATION_H
