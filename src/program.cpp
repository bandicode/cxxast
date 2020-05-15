// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/program.h"

#include "cxx/class.h"
#include "cxx/name.h"
#include "cxx/namespace.h"
#include "cxx/type.h"

namespace cxx
{

Program::Program()
{
  m_global_namespace = std::make_shared<Namespace>("");
}

Program::~Program()
{

}

static std::shared_ptr<Entity> resolve_impl(const std::string& name, const std::shared_ptr<Entity>& context)
{
  if (!context)
    return nullptr;

  if (context->is<Namespace>())
  {
    const Namespace& ns = static_cast<const Namespace&>(*context);

    for (const auto& e : ns.entities)
    {
      if (e->name == name)
        return e;
    }

    return resolve_impl(name, context->parent());
  }
  else if (context->is<Class>())
  {
    const Class& cla = static_cast<const Class&>(*context);

    for (const auto& mem : cla.members)
    {
      if (mem->name == name)
        return mem;
    }

    return resolve_impl(name, context->parent());
  }

  return nullptr;
}

std::shared_ptr<Entity> Program::resolve(const Name& n)
{
  return resolve(n, globalNamespace());
}

std::shared_ptr<Entity> Program::resolve(const Name& n, const std::shared_ptr<Entity>& context)
{
  if (!context)
    return nullptr;

  std::string name = n.toString();

  return resolve_impl(name, context);
}

} // namespace cxx
