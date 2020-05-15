// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLASS_H
#define CXXAST_CLASS_H

#include "cxx/entity.h"

#include <utility>
#include <vector>

namespace cxx
{

class CXXAST_API Class : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::vector<std::shared_ptr<Entity>> members;
  bool is_struct = false;

public:
  ~Class() = default;

  explicit Class(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Class;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;
};

} // namespace cxx

namespace cxx
{

inline Class::Class(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

} // namespace cxx

#endif // CXXAST_CLASS_H
