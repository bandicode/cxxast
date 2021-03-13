// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_VARIABLE_DECLARATION_H
#define CXXAST_VARIABLE_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/variable.h"

namespace cxx
{

class CXXAST_API VariableDeclaration : public IDeclaration
{
public:
  std::shared_ptr<Variable> variable;

public:
  VariableDeclaration() = default;

  explicit VariableDeclaration(std::shared_ptr<Variable> var);

  static constexpr NodeKind ClassNodeKind = NodeKind::VariableDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

inline VariableDeclaration::VariableDeclaration(std::shared_ptr<Variable> var)
  : IDeclaration(var), 
    variable(var)
{

}

} // namespace cxx

#endif // CXXAST_VARIABLE_DECLARATION_H
