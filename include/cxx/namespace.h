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

class CXXAST_API Namespace : public Entity
{
public:
  std::vector<std::shared_ptr<Entity>> entities;

public:
  ~Namespace() = default;

  static constexpr NodeKind ClassNodeKind = NodeKind::Namespace;
  NodeKind node_kind() const override;

  explicit Namespace(std::string name, std::shared_ptr<Entity> parent = nullptr);

  size_t childCount() const override;
  std::shared_ptr<Node> childAt(size_t index) const override;
  void appendChild(std::shared_ptr<Node> n) override;

  std::shared_ptr<Namespace> getOrCreateNamespace(const std::string& name);
  std::shared_ptr<Class> createClass(std::string name);
  std::shared_ptr<Class> getOrCreateClass(const std::string& name);
  std::shared_ptr<Enum> createEnum(std::string name);
  std::shared_ptr<Function> createFunction(std::string name);

  template<typename T, typename...Args>
  std::shared_ptr<T> getOrCreate(const std::string& name, Args&&... args)
  {
    auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<Entity>& e) {
      return e->is<T>() && e->name == name;
      });

    if (it != entities.end())
      return std::static_pointer_cast<T>(*it);

    auto result = std::make_shared<T>(std::forward<Args>(args)..., shared_from_this());
    entities.push_back(result);
    return result;
  }
};

} // namespace cxx

namespace cxx
{

inline Namespace::Namespace(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}

} // namespace cxx

#endif // CXXAST_NAMESPACE_H
