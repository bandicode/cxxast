// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FUNCTION_H
#define CXXAST_FUNCTION_H

#include "cxx/entity.h"

#include "cxx/expression.h"
#include "cxx/statement.h"
#include "cxx/type.h"
#include "cxx/template.h"

#include <vector>

namespace cxx
{

class CXXAST_API FunctionSpecifier
{
public:

  enum Value
  {
    None = 0,
    Inline = 1,
    Static = 2,
    Constexpr = 4,
    Virtual = 8,
    Override = 16,
    Final = 32,
    Const = 64,
    Pure = 128,
    Noexcept = 256,
    Explicit = 512,
    Default = 1024,
    Delete = 2048,
  };
};

class CXXAST_API FunctionKind
{
public:
  enum Value
  {
    None = 0,
    Constructor = 1,
    Destructor = 2,
    OperatorOverload,
    ConversionFunction,
  };
};

class Function;

class CXXAST_API FunctionParameter : public IEntity
{
public:
  Type type;
  Expression default_value;

public:
  FunctionParameter(Type type, std::string name, std::shared_ptr<Function> parent = nullptr);
  FunctionParameter(Type type, std::string name, Expression default_value, std::shared_ptr<Function> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::FunctionParameter;
  NodeKind node_kind() const override;

  std::shared_ptr<Function> parent() const;
};

class CXXAST_API Function : public IEntity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  Type return_type;
  std::vector<std::shared_ptr<FunctionParameter>> parameters;
  int specifiers = FunctionSpecifier::None;
  FunctionKind::Value kind = FunctionKind::None;
  Statement body;

public:
  ~Function() = default;

  explicit Function(std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Function;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  typedef FunctionParameter Parameter;

  virtual bool isTemplate() const;
  virtual const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const;

  bool isInline() const;
  bool isStatic() const;
  bool isConstexpr() const;
  bool isVirtual() const;
  bool isVirtualPure() const;
  bool isOverride() const;
  bool isFinal() const;
  bool isConst() const;
  bool isNoexcept() const;
  bool isExplicit() const;

  bool isConstructor() const;
  bool isDestructor() const;

  std::string signature() const;

  struct ReturnType : public priv::Field<Function, Type>
  {
    static field_type& get(INode& n)
    {
      return down_cast(n).return_type;
    }

    static void set(INode& n, field_type val)
    {
      down_cast(n).return_type = std::move(val);
    }
  };

  struct Parameters : public priv::Field<Function, std::vector<std::shared_ptr<FunctionParameter>>>
  {
    static field_type& get(INode& n)
    {
      return down_cast(n).parameters;
    }

    static void set(INode& n, field_type val)
    {
      down_cast(n).parameters = std::move(val);
    }
  };

  struct Body : priv::FieldEx<Function, Statement, &Function::body> { };
};

class CXXAST_API FunctionTemplate : public Function
{
public:
  std::vector<std::shared_ptr<TemplateParameter>> template_parameters;

public:
  FunctionTemplate(std::vector<std::shared_ptr<TemplateParameter>> tparams, std::string name, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::FunctionTemplate;
  NodeKind node_kind() const override;

  bool isTemplate() const override;
  const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const override;
};

} // namespace cxx

namespace cxx
{

inline Function::Function(std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{std::move(name), std::move(parent)}
{

}

inline bool Function::isInline() const
{
  return specifiers & FunctionSpecifier::Inline;
}

inline bool Function::isStatic() const
{
  return specifiers & FunctionSpecifier::Static;
}

inline bool Function::isConstexpr() const
{
  return specifiers & FunctionSpecifier::Constexpr;
}

inline bool Function::isVirtual() const
{
  return specifiers & FunctionSpecifier::Virtual;
}

inline bool Function::isVirtualPure() const
{
  return specifiers & FunctionSpecifier::Pure;
}

inline bool Function::isOverride() const
{
  return specifiers & FunctionSpecifier::Override;
}

inline bool Function::isFinal() const
{
  return specifiers & FunctionSpecifier::Final;
}

inline bool Function::isConst() const
{
  return specifiers & FunctionSpecifier::Const;
}

inline bool Function::isNoexcept() const
{
  return specifiers & FunctionSpecifier::Noexcept;
}

inline bool Function::isExplicit() const
{
  return specifiers & FunctionSpecifier::Explicit;
}

inline bool Function::isConstructor() const
{
  return kind == FunctionKind::Constructor;
}

inline bool Function::isDestructor() const
{
  return kind == FunctionKind::Destructor;
}

} // namespace cxx

#endif // CXXAST_FUNCTION_H
