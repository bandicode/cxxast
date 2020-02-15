// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ENUM_H
#define CXXAST_ENUM_H

#include "cxx/entity.h"

#include <vector>

namespace cxx
{

class CXXAST_API Enum : public Entity
{
public:
  ~Enum() = default;

  explicit Enum(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static const std::string TypeId;
  const std::string& type() const override;

  struct Value
  {
    std::string name;
  };

  std::vector<Value>& values();
  const std::vector<Value>& values() const;

private:
  std::vector<Value> m_values;
};

} // namespace cxx

namespace cxx
{

inline Enum::Enum(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline std::vector<Enum::Value>& Enum::values()
{
  return m_values;
}

inline const std::vector<Enum::Value>& Enum::values() const
{
  return m_values;
}

} // namespace cxx

#endif // CXXAST_ENUM_H
