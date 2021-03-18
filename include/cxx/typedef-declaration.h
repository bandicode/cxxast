// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPEDEF_DECLARATION_H
#define CXXAST_TYPEDEF_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/typedef.h"

namespace cxx
{

class CXXAST_API TypedefDeclaration : public IDeclaration
{
public:
  TypedefDeclaration() = default;

  explicit TypedefDeclaration(std::shared_ptr<Typedef> e);

  static constexpr NodeKind ClassNodeKind = NodeKind::TypedefDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

inline TypedefDeclaration::TypedefDeclaration(std::shared_ptr<Typedef> e)
  : IDeclaration(e)
{

}

} // namespace cxx

#endif // CXXAST_TYPEDEF_DECLARATION_H
