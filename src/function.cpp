// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function.h"

#include <stdexcept>

namespace cxx
{

FunctionParameter::FunctionParameter(Type t, std::string name, std::shared_ptr<Function> parent)
  : IEntity(std::move(name), parent),
    type(std::move(t))
{

}

FunctionParameter::FunctionParameter(Type t, std::string name, Expression default_val, std::shared_ptr<Function> parent)
  : IEntity(std::move(name), parent),
    type(std::move(t)),
    default_value(default_val)
{

}

NodeKind FunctionParameter::node_kind() const
{
  return ClassNodeKind;
}

std::shared_ptr<Function> FunctionParameter::parent() const
{
  return std::static_pointer_cast<Function>(IEntity::parent());
}

NodeKind Function::node_kind() const
{
  return ClassNodeKind;
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

static void write_params(const Function& f, std::string& out)
{
  for (auto p : f.parameters)
  {
    out += p->type.toString();

    if (!p->name.empty())
      out += " " + p->name;

    out += ", ";
  }

  if (!f.parameters.empty())
  {
    out.pop_back();
    out.pop_back();
  }
}

std::string Function::signature() const
{
  if (isConstructor())
  {
    std::string result;
    if (isExplicit())
      result += "explicit ";
    result += this->name;
    result += "(";
    write_params(*this, result);
    result += ")";

    if (this->specifiers & cxx::FunctionSpecifier::Delete)
      result += " = delete";

    return result;
  }
  else if (isDestructor())
  {
    std::string result;

    result += this->name;
    result += "()";

    if (this->specifiers & cxx::FunctionSpecifier::Delete)
      result += " = delete";

    return result;
  }

  std::string result;
  if (isExplicit())
    result += "explicit ";
  if (isStatic())
    result += "static ";

  result += this->return_type.toString();
  result += " " + this->name;
  result += "(";
  write_params(*this, result);
  result += ")";

  if (isConst())
    result += " const";

  if (this->specifiers & cxx::FunctionSpecifier::Delete)
    result += " = delete";

  return result;
}

FunctionTemplate::FunctionTemplate(std::vector<std::shared_ptr<TemplateParameter>> tparams, std::string name, std::shared_ptr<IEntity> parent)
  : Function(std::move(name), parent),
    template_parameters(std::move(tparams))
{
}

NodeKind FunctionTemplate::node_kind() const
{
  return FunctionTemplate::ClassNodeKind;
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
