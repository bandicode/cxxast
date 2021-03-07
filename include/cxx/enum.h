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

class CXXAST_API EnumValue : public IEntity
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


class CXXAST_API Enum : public IEntity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::vector<std::shared_ptr<EnumValue>> values;
  bool enum_class = false;

public:
  ~Enum() = default;

  explicit Enum(std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Enum;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  struct Values : public priv::Field<Enum, std::vector<std::shared_ptr<EnumValue>>>
  {
    static field_type& get(INode& n)
    {
      return down_cast(n).values;
    }

    static void set(INode& n, field_type values)
    {
      down_cast(n).values = std::move(values);
    }
  };
};

} // namespace cxx

namespace cxx
{

inline Enum::Enum(std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{std::move(name), std::move(parent)}
{

}

} // namespace cxx

#endif // CXXAST_ENUM_H
