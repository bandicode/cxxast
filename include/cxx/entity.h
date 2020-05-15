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
private:
  std::string m_name;
  std::weak_ptr<Entity> m_parent;
  std::shared_ptr<Documentation> m_documentation;

public:
  explicit Entity(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::shared_ptr<Entity> shared_from_this();

  bool isEntity() const override;

  const std::string& name() const;

  std::shared_ptr<Entity> parent() const;
  void setParent(std::shared_ptr<Entity> e);

  const std::shared_ptr<Documentation>& documentation() const;
  void setDocumentation(std::shared_ptr<Documentation> doc);

  virtual AccessSpecifier getAccessSpecifier() const;
  virtual void setAccessSpecifier(AccessSpecifier aspec);
};

} // namespace cxx

namespace cxx
{

inline Entity::Entity(std::string name, std::shared_ptr<Entity> parent)
  : m_name(std::move(name)),
    m_parent(parent)
{

}

inline const std::string& Entity::name() const
{
  return m_name;
}

inline std::shared_ptr<Entity> Entity::parent() const
{
  return m_parent.lock();
}

inline void Entity::setParent(std::shared_ptr<Entity> e)
{
  m_parent = e;
}

inline const std::shared_ptr<Documentation>& Entity::documentation() const
{
  return m_documentation;
}

inline void Entity::setDocumentation(std::shared_ptr<Documentation> doc)
{
  m_documentation = doc;
}

} // namespace cxx

#endif // CXXAST_ENTITY_H
