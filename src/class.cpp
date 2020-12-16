// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/class.h"

#include <stdexcept>

namespace cxx
{

NodeKind Class::node_kind() const
{
  return ClassNodeKind;
}

AccessSpecifier Class::getAccessSpecifier() const
{
  return access_specifier;
}

void Class::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

bool Class::isTemplate() const
{
  return false;
}

const std::vector<std::shared_ptr<TemplateParameter>>& Class::templateParameters() const
{
  static const std::vector<std::shared_ptr<TemplateParameter>> static_instance = {};
  return static_instance;
}

ClassTemplate::ClassTemplate(std::vector<std::shared_ptr<TemplateParameter>> tparams, std::string name, std::shared_ptr<Entity> parent)
  : Class(std::move(name), parent),
    template_parameters(std::move(tparams))
{
}

NodeKind ClassTemplate::node_kind() const
{
  return ClassTemplate::ClassNodeKind;
}

bool ClassTemplate::isTemplate() const
{
  return true;
}

const std::vector<std::shared_ptr<TemplateParameter>>& ClassTemplate::templateParameters() const
{
  return this->template_parameters;
}

} // namespace cxx
