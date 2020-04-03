// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ENUM_H
#define CXXAST_ENUM_H

#include "cxx/entity.h"

#include <vector>

namespace cxx
{

class Enum;

class CXXAST_API EnumValue : public Entity
{
public:
  explicit EnumValue(std::string name, std::shared_ptr<Enum> parent = nullptr);
  EnumValue(std::string name, std::string value, std::shared_ptr<Enum> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::EnumValue;
  NodeKind node_kind() const override;

  std::shared_ptr<Enum> parent() const;

  std::string& value();
  const std::string& value() const;

private:
  std::string m_value;
};


class CXXAST_API Enum : public Entity
{
public:
  ~Enum() = default;

  explicit Enum(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Enum;
  NodeKind node_kind() const override;

  std::vector<std::shared_ptr<EnumValue>>& values();
  const std::vector<std::shared_ptr<EnumValue>>& values() const;

private:
  std::vector<std::shared_ptr<EnumValue>> m_values;
};

} // namespace cxx

namespace cxx
{

inline Enum::Enum(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline std::vector<std::shared_ptr<EnumValue>>& Enum::values()
{
  return m_values;
}

inline const std::vector<std::shared_ptr<EnumValue>>& Enum::values() const
{
  return m_values;
}

} // namespace cxx

#endif // CXXAST_ENUM_H
