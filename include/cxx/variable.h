// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_VARIABLE_H
#define CXXAST_VARIABLE_H

#include "cxx/entity.h"

#include "cxx/expression.h"
#include "cxx/type.h"
#include "cxx/template.h"

#include <vector>

namespace cxx
{

class CXXAST_API VariableSpecifier
{
public:

  enum Value
  {
    None = 0,
    Inline = 1,
    Static = 2,
    Constexpr = 4,
  };
};

class CXXAST_API Variable : public IEntity
{
public:
  Variable(Type type, std::string name, std::shared_ptr<IEntity> parent = nullptr);
  Variable(Type type, std::string name, Expression default_value, std::shared_ptr<IEntity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Variable;
  NodeKind node_kind() const override;

  Type& type();
  const Type& type() const;

  Expression& defaultValue();
  const Expression& defaultValue() const;

  int& specifiers();
  int specifiers() const;

private:
  Type m_type;
  int m_flags = VariableSpecifier::None;
  Expression m_default_value;
};

} // namespace cxx

namespace cxx
{

inline Variable::Variable(Type type, std::string name, std::shared_ptr<IEntity> parent)
  : IEntity{std::move(name), std::move(parent)},
    m_type{type}
{

}

inline Variable::Variable(Type type, std::string name, Expression default_value, std::shared_ptr<IEntity> parent)
  : IEntity{ std::move(name), std::move(parent) },
    m_type{ type },
    m_default_value{ std::move(default_value) }
{

}

inline Type& Variable::type()
{
  return m_type;
}

inline const Type& Variable::type() const
{
  return m_type;
}

} // namespace cxx

#endif // CXXAST_VARIABLE_H
