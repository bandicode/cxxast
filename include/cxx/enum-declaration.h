// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ENUM_DECLARATION_H
#define CXXAST_ENUM_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/enum.h"

namespace cxx
{

// for enum decl inside a function
class CXXAST_API EnumDeclaration : public IDeclaration
{
public:
  EnumDeclaration() = default;

  explicit EnumDeclaration(std::shared_ptr<Enum> e);

  static constexpr NodeKind ClassNodeKind = NodeKind::EnumDeclaration;
  NodeKind node_kind() const override;
};

class CXXAST_API EnumeratorDeclaration : public IDeclaration
{
public:
  EnumeratorDeclaration() = default;

  explicit EnumeratorDeclaration(std::shared_ptr<EnumValue> e);

  static constexpr NodeKind ClassNodeKind = NodeKind::EnumeratorDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

inline EnumDeclaration::EnumDeclaration(std::shared_ptr<Enum> e)
  : IDeclaration(e)
{

}

inline EnumeratorDeclaration::EnumeratorDeclaration(std::shared_ptr<EnumValue> e)
  : IDeclaration(e)
{

}

} // namespace cxx

#endif // CXXAST_ENUM_DECLARATION_H
