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

public:
  ~Class() = default;

  explicit Class(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Class;
  NodeKind node_kind() const override;

  bool& isStruct();
  bool isStruct() const;

  std::vector<std::shared_ptr<Entity>>& members();
  const std::vector<std::shared_ptr<Entity>>& members() const;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

private:
  bool m_is_struct = false;
  std::vector<std::shared_ptr<Entity>> m_members;
};

} // namespace cxx

namespace cxx
{

inline Class::Class(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline bool& Class::isStruct()
{
  return m_is_struct;
}

inline bool Class::isStruct() const
{
  return m_is_struct;
}

inline std::vector<std::shared_ptr<Entity>>& Class::members()
{
  return m_members;
}

inline const std::vector<std::shared_ptr<Entity>>& Class::members() const
{
  return m_members;
}

} // namespace cxx

#endif // CXXAST_CLASS_H
