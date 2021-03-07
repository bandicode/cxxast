// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPEDEF_H
#define CXXAST_TYPEDEF_H

#include "cxx/entity.h"

#include "cxx/type.h"

namespace cxx
{

class CXXAST_API Typedef : public IEntity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  Type type;

public:
  Typedef(Type t, std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Typedef;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;
};

} // namespace cxx

namespace cxx
{

inline Typedef::Typedef(Type t, std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{std::move(name), std::move(parent)},
    type(t)
{

}

} // namespace cxx

#endif // CXXAST_TYPEDEF_H
