// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TEMPLATE_DECLARATION_H
#define CXXAST_TEMPLATE_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/function.h"

namespace cxx
{

class CXXAST_API TemplateParameterDeclaration : public IDeclaration
{
public:
  TemplateParameterDeclaration() = default;

  explicit TemplateParameterDeclaration(std::shared_ptr<TemplateParameter> tp);

  static constexpr NodeKind ClassNodeKind = NodeKind::TemplateParameterDeclaration;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

inline TemplateParameterDeclaration::TemplateParameterDeclaration(std::shared_ptr<TemplateParameter> tp)
  : IDeclaration(tp)
{

}

} // namespace cxx

#endif // CXXAST_TEMPLATE_DECLARATION_H
