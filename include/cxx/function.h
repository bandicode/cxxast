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
  FunctionParameter(Type type, std::string name, std::shared_ptr<Function> parent = nullptr);
  FunctionParameter(Type type, std::string name, Expression default_value, std::shared_ptr<Function> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::FunctionParameter;
  NodeKind node_kind() const override;

  std::shared_ptr<Function> parent() const;

  Type& parameterType();
  const Type& parameterType() const;

  Expression& defaultValue();
  const Expression& defaultValue() const;

private:
  Type m_type;
  Expression m_default_value;
};

class CXXAST_API Function : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;

public:
  ~Function() = default;

  explicit Function(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Function;
  NodeKind node_kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  typedef FunctionParameter Parameter;

  Type& returnType();
  const Type& returnType() const;

  std::vector<std::shared_ptr<Parameter>>& parameters();
  const std::vector<std::shared_ptr<Parameter>>& parameters() const;

  std::vector<std::shared_ptr<TemplateParameter>>& templateParameters();
  const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const;

  std::shared_ptr<Node> body() const;
  void setBody(std::shared_ptr<Node> b);

  bool isTemplate() const;

  bool isInline() const;
  bool isStatic() const;
  bool isConstexpr() const;
  bool isVirtual() const;
  bool isVirtualPure() const;
  bool isOverride() const;
  bool isFinal() const;
  bool isConst() const;

  int& specifiers();
  int specifiers() const;

  bool isConstructor() const;
  bool isDestructor() const;

  int& kind();
  int kind() const;

private:
  Type m_rtype;
  std::vector<std::shared_ptr<Parameter>> m_params;
  std::vector<std::shared_ptr<TemplateParameter>> m_tparams;
  int m_flags = FunctionSpecifier::None;
  int m_kind = FunctionKind::None;
  std::shared_ptr<Node> m_body;
};

} // namespace cxx

namespace cxx
{

inline Function::Function(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline Type& Function::returnType()
{
  return m_rtype;
}

inline const Type& Function::returnType() const
{
  return m_rtype;
}

inline std::vector<std::shared_ptr<FunctionParameter>>& Function::parameters()
{
  return m_params;
}

inline const std::vector<std::shared_ptr<FunctionParameter>>& Function::parameters() const
{
  return m_params;
}

inline std::vector<std::shared_ptr<TemplateParameter>>& Function::templateParameters()
{
  return m_tparams;
}

inline const std::vector<std::shared_ptr<TemplateParameter>>& Function::templateParameters() const
{
  return m_tparams;
}

inline std::shared_ptr<Node> Function::body() const
{
  return m_body;
}

inline void Function::setBody(std::shared_ptr<Node> b)
{
  m_body = b;
}

inline bool Function::isTemplate() const
{
  return !m_tparams.empty();
}

inline bool Function::isInline() const
{
  return m_flags & FunctionSpecifier::Inline;
}

inline bool Function::isStatic() const
{
  return m_flags & FunctionSpecifier::Static;
}

inline bool Function::isConstexpr() const
{
  return m_flags & FunctionSpecifier::Constexpr;
}

inline bool Function::isVirtual() const
{
  return m_flags & FunctionSpecifier::Virtual;
}

inline bool Function::isVirtualPure() const
{
  return m_flags & FunctionSpecifier::Pure;
}

inline bool Function::isOverride() const
{
  return m_flags & FunctionSpecifier::Override;
}

inline bool Function::isFinal() const
{
  return m_flags & FunctionSpecifier::Final;
}

inline bool Function::isConst() const
{
  return m_flags & FunctionSpecifier::Const;
}

inline int& Function::specifiers()
{
  return m_flags;
}

inline int Function::specifiers() const
{
  return m_flags;
}

inline bool Function::isConstructor() const
{
  return kind() == FunctionKind::Constructor;
}

inline bool Function::isDestructor() const
{
  return kind() == FunctionKind::Destructor;
}

inline int& Function::kind()
{
  return m_kind;
}

inline int Function::kind() const
{
  return m_kind;
}

} // namespace cxx

#endif // CXXAST_FUNCTION_H
