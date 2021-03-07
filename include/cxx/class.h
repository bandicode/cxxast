// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLASS_H
#define CXXAST_CLASS_H

#include "cxx/entity.h"
#include "cxx/template.h"

#include <utility>
#include <vector>

namespace cxx
{

class Class;

struct BaseClass
{
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::shared_ptr<Class> base;

  bool isPublicBase() const
  {
    return access_specifier == AccessSpecifier::PUBLIC;
  }

  bool isProtectedBase() const
  {
    return access_specifier == AccessSpecifier::PROTECTED;
  }

  bool isPrivateBase() const
  {
    return access_specifier == AccessSpecifier::PRIVATE;
  }
};

class CXXAST_API Class : public IEntity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::vector<BaseClass> bases;
  std::vector<std::shared_ptr<IEntity>> members;
  bool is_struct = false;
  bool is_final = false;

public:
  ~Class() = default;

  explicit Class(std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Class;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  virtual bool isTemplate() const;
  virtual const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const;

  struct Members : public priv::Field<Class, std::vector<std::shared_ptr<IEntity>>>
  {
    static field_type& get(INode& n)
    {
      return down_cast(n).members;
    }

    static void set(INode& n, field_type members)
    {
      down_cast(n).members = std::move(members);
    }
  };
};

class CXXAST_API ClassTemplate : public Class
{
public:
  std::vector<std::shared_ptr<TemplateParameter>> template_parameters;

public:
  ClassTemplate(std::vector<std::shared_ptr<TemplateParameter>> tparams, std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::ClassTemplate;
  NodeKind node_kind() const override;

  bool isTemplate() const override;
  const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const override;
};

} // namespace cxx

namespace cxx
{

inline Class::Class(std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{std::move(name), std::move(parent)}
{

}

} // namespace cxx

#endif // CXXAST_CLASS_H
