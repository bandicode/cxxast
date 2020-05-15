// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ENTITY_H
#define CXXAST_ENTITY_H

#include "cxx/node.h"

#include "cxx/access-specifier.h"

namespace cxx
{

class Documentation;

class CXXAST_API Entity : public Node
{
public:
  std::string name;
  std::weak_ptr<Entity> weak_parent;
  std::shared_ptr<Documentation> documentation;

public:
  explicit Entity(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::shared_ptr<Entity> shared_from_this();

  bool isEntity() const override;

  std::shared_ptr<Entity> parent() const;

  virtual AccessSpecifier getAccessSpecifier() const;
  virtual void setAccessSpecifier(AccessSpecifier aspec);
};

} // namespace cxx

namespace cxx
{

inline Entity::Entity(std::string n, std::shared_ptr<Entity> parent)
  : name(std::move(n)),
    weak_parent(parent)
{

}

inline std::shared_ptr<Entity> Entity::parent() const
{
  return weak_parent.lock();
}

} // namespace cxx

#endif // CXXAST_ENTITY_H
