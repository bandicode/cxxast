// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAMESPACE_H
#define CXXAST_NAMESPACE_H

#include "cxx/entity.h"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace cxx
{

class Class;
class Enum;
class Function;

class CXXAST_API Namespace : public IEntity
{
public:
  std::vector<std::shared_ptr<IEntity>> entities;

public:
  ~Namespace() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::Namespace;
  NodeKind node_kind() const override;

  explicit Namespace(std::string name, std::shared_ptr<IEntity> parent = nullptr);

  std::shared_ptr<Namespace> getOrCreateNamespace(const std::string& name);
  std::shared_ptr<Class> createClass(std::string name);
  std::shared_ptr<Class> getOrCreateClass(const std::string& name);
  std::shared_ptr<Enum> createEnum(std::string name);
  std::shared_ptr<Function> createFunction(std::string name);

  template<typename T, typename...Args>
  std::shared_ptr<T> getOrCreate(const std::string& name, Args&&... args)
  {
    auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<IEntity>& e) {
      return e->is<T>() && e->name == name;
      });

    if (it != entities.end())
      return std::static_pointer_cast<T>(*it);

    auto result = std::make_shared<T>(std::forward<Args>(args)..., shared_from_this());
    entities.push_back(result);
    return result;
  }

  struct Entities : public priv::Field<Namespace, std::vector<std::shared_ptr<IEntity>>>
  {
    static field_type& get(INode& n)
    {
      return down_cast(n).entities;
    }

    static void set(INode& n, field_type entities)
    {
      down_cast(n).entities = std::move(entities);
    }
  };
};

} // namespace cxx

namespace cxx
{

inline Namespace::Namespace(std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{ std::move(name), std::move(parent) }
{

}

} // namespace cxx

#endif // CXXAST_NAMESPACE_H
