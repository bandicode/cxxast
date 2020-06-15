// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function.h"

#include <stdexcept>

namespace cxx
{

FunctionParameter::FunctionParameter(Type t, std::string name, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
    type(std::move(t))
{

}

FunctionParameter::FunctionParameter(Type t, std::string name, Expression default_val, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
    type(std::move(t)),
    default_value(std::move(default_val))
{

}

NodeKind FunctionParameter::node_kind() const
{
  return ClassNodeKind;
}

std::shared_ptr<Function> FunctionParameter::parent() const
{
  return std::static_pointer_cast<Function>(Entity::parent());
}

NodeKind Function::node_kind() const
{
  return ClassNodeKind;
}

size_t Function::childCount() const
{
  return this->parameters.size();
}

std::shared_ptr<Node> Function::childAt(size_t index) const
{
  return this->parameters.at(index);
}

void Function::appendChild(std::shared_ptr<Node> n)
{
  if (n->is<FunctionParameter>())
    this->parameters.push_back(std::static_pointer_cast<FunctionParameter>(n));
  else
    throw std::runtime_error{ "bad call to Function::appendChild()" };
}

AccessSpecifier Function::getAccessSpecifier() const
{
  return access_specifier;
}

void Function::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

bool Function::isTemplate() const
{
  return false;
}

const std::vector<std::shared_ptr<TemplateParameter>>& Function::templateParameters() const
{
  static const std::vector<std::shared_ptr<TemplateParameter>> static_instance = {};
  return static_instance;
}

FunctionTemplate::FunctionTemplate(std::vector<std::shared_ptr<TemplateParameter>> tparams, std::string name, std::shared_ptr<Entity> parent)
  : Function(std::move(name), parent),
    template_parameters(std::move(tparams))
{
}

NodeKind FunctionTemplate::node_kind() const
{
  return FunctionTemplate::ClassNodeKind;
}

size_t FunctionTemplate::childCount() const
{
  return this->parameters.size() + this->template_parameters.size();
}

std::shared_ptr<Node> FunctionTemplate::childAt(size_t index) const
{
  if (index < this->template_parameters.size())
    return this->template_parameters.at(index);

  index -= this->template_parameters.size();
  return this->parameters.at(index);
}

void FunctionTemplate::appendChild(std::shared_ptr<Node> n)
{
  if (n->is<TemplateParameter>())
    this->template_parameters.push_back(std::static_pointer_cast<TemplateParameter>(n));
  else
    Function::appendChild(n);
}

bool FunctionTemplate::isTemplate() const
{
  return true;
}

const std::vector<std::shared_ptr<TemplateParameter>>& FunctionTemplate::templateParameters() const
{
  return this->template_parameters;
}

} // namespace cxx
