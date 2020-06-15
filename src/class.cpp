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

size_t Class::childCount() const
{
  return this->members.size();
}

std::shared_ptr<Node> Class::childAt(size_t index) const
{
  return this->members.at(index);
}

void Class::appendChild(std::shared_ptr<Node> n)
{
  if (!n->isEntity())
    throw std::runtime_error{ "bad call Namespace::appendChild()" };

  this->members.push_back(std::static_pointer_cast<Entity>(n));
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

size_t ClassTemplate::childCount() const
{
  return this->members.size() + this->template_parameters.size();
}

std::shared_ptr<Node> ClassTemplate::childAt(size_t index) const
{
  if (index < this->template_parameters.size())
    return this->template_parameters.at(index);
  return Class::childAt(index - this->template_parameters.size());
}

void ClassTemplate::appendChild(std::shared_ptr<Node> n)
{
  if (n->is<TemplateParameter>())
    this->template_parameters.push_back(std::static_pointer_cast<TemplateParameter>(n));
  else
    Class::appendChild(n);
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
