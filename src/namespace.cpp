
#include "cxx/namespace.h"

#include "cxx/class.h"
#include "cxx/enum.h"
#include "cxx/function.h"

#include <stdexcept>

namespace cxx
{

NodeKind Namespace::node_kind() const
{
  return ClassNodeKind;
}

std::shared_ptr<Namespace> Namespace::getOrCreateNamespace(const std::string& name)
{
  auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<IEntity>& e) {
    return e->is<Namespace>() && e->name == name;
    });

  if (it != entities.end())
    return std::static_pointer_cast<Namespace>(*it);

  auto result = std::make_shared<Namespace>(name, shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Class> Namespace::createClass(std::string name)
{
  auto result = std::make_shared<Class>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Class> Namespace::getOrCreateClass(const std::string& name)
{
  auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<IEntity>& e) {
    return e->is<Class>() && e->name == name;
    });

  if (it != entities.end())
    return std::static_pointer_cast<Class>(*it);

  auto result = std::make_shared<Class>(name, shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Enum> Namespace::createEnum(std::string name)
{
  auto result = std::make_shared<Enum>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Function> Namespace::createFunction(std::string name)
{
  auto result = std::make_shared<Function>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}

} // namespace cxx
