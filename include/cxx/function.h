// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FUNCTION_H
#define CXXAST_FUNCTION_H

#include "cxx/entity.h"

#include "cxx/expression.h"
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

class CXXAST_API FunctionParameter : public Entity
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

class CXXAST_API Function : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  Type return_type;
  std::vector<std::shared_ptr<FunctionParameter>> parameters;
  std::vector<std::shared_ptr<TemplateParameter>> template_parameters;
  int specifiers = FunctionSpecifier::None;
  FunctionKind::Value kind = FunctionKind::None;
  std::shared_ptr<Node> body;

public:
  ~Function() = default;

  explicit Function(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Function;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  typedef FunctionParameter Parameter;

  bool isTemplate() const;

  bool isInline() const;
  bool isStatic() const;
  bool isConstexpr() const;
  bool isVirtual() const;
  bool isVirtualPure() const;
  bool isOverride() const;
  bool isFinal() const;
  bool isConst() const;

  bool isConstructor() const;
  bool isDestructor() const;
};

} // namespace cxx

namespace cxx
{

inline Function::Function(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline bool Function::isTemplate() const
{
  return !template_parameters.empty();
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
