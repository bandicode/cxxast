// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAMESPACE_H
#define CXXAST_NAMESPACE_H

#include "cxx/entity.h"

#include <map>
#include <memory>
#include <vector>

namespace cxx
{

class Class;
class Enum;
class Function;

class CXXAST_API Namespace : public Entity
{
public:
  ~Namespace() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::Namespace;
  NodeKind node_kind() const override;

  explicit Namespace(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::vector<std::shared_ptr<Entity>>& entities();
  const std::vector<std::shared_ptr<Entity>>& entities() const;

  std::shared_ptr<Namespace> getOrCreateNamespace(const std::string& name);
  std::shared_ptr<Class> createClass(std::string name);
  std::shared_ptr<Enum> createEnum(std::string name);
  std::shared_ptr<Function> createFunction(std::string name);

private:
  std::vector<std::shared_ptr<Entity>> m_entities;
};

} // namespace cxx

namespace cxx
{

inline Namespace::Namespace(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}

inline std::vector<std::shared_ptr<Entity>>& Namespace::entities()
{
  return m_entities;
}

inline const std::vector<std::shared_ptr<Entity>>& Namespace::entities() const
{
  return m_entities;
}

} // namespace cxx

#endif // CXXAST_NAMESPACE_H
