// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TEMPLATE_H
#define CXXAST_TEMPLATE_H

#include "cxx/type.h"
#include "cxx/entity.h"

#include <tuple>
#include <vector>

namespace cxx
{

struct TemplateNonTypeParameter
{
  Type type;
  std::string default_value;

  typedef std::string default_value_t;
};

struct TemplateTypeParameter
{
  Type default_value;

  typedef Type default_value_t;
};

class CXXAST_API TemplateParameter : public IEntity
{
private:
  bool m_is_type_parameter;
  std::tuple<TemplateNonTypeParameter, TemplateTypeParameter> m_data;

public:
  TemplateParameter();
  TemplateParameter(const TemplateParameter&) = default;
  TemplateParameter(TemplateParameter&&) = default;
  ~TemplateParameter() = default;

  explicit TemplateParameter(std::string name, Type default_value = {});
  TemplateParameter(const Type& type, std::string name, std::string default_value = {});

  static constexpr NodeKind ClassNodeKind = NodeKind::TemplateParameter;
  NodeKind node_kind() const override;

  bool isTypeParameter() const;
  bool isNonTypeParameter() const;

  bool hasDefaultValue();

  template<typename T>
  typename T::default_value_t defaultValue() const
  {
    return std::get<T>(m_data).default_value;
  }

  TemplateParameter& operator=(const TemplateParameter&) = default;
  TemplateParameter& operator=(TemplateParameter&&) = default;
};

class CXXAST_API TemplateArgument
{
private:
  std::tuple<Type, std::string> m_data;

public:
  TemplateArgument(const TemplateArgument&) = default;
  TemplateArgument(TemplateArgument&&) = default;
  ~TemplateArgument() = default;

  explicit TemplateArgument(std::string expr);
  explicit TemplateArgument(const Type& type);

  bool isType() const;

  template<typename T>
  const T& get() const
  {
    return std::get<T>(m_data);
  }

  std::string toString() const;

  TemplateArgument& operator=(const TemplateArgument&) = default;
  TemplateArgument& operator=(TemplateArgument&&) = default;
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_TEMPLATE_H
