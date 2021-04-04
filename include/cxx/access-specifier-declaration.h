// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ACCESSSPECIFIERDECL_H
#define CXXAST_ACCESSSPECIFIERDECL_H

#include "cxx/access-specifier.h"
#include "cxx/declaration.h"

namespace cxx
{

class CXXAST_API AccessSpecifierDeclaration : public IDeclaration
{
public:
  AccessSpecifier value = AccessSpecifier::PUBLIC;

public:
  AccessSpecifierDeclaration() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::AccessSpecifierDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

#endif // CXXAST_ACCESSSPECIFIERDECL_H
