
#include "cxx/namespace.h"

#include "cxx/class.h"
#include "cxx/enum.h"
#include "cxx/function.h"

namespace cxx
{

const std::string Namespace::TypeId = "namespace";

const std::string& Namespace::type() const
{
  return TypeId;
}

std::shared_ptr<Namespace> Namespace::getOrCreateNamespace(const std::string& name)
{
  auto it = std::find_if(entities().begin(), entities().end(), [&name](const std::shared_ptr<Entity>& e) {
    return e->is<Namespace>() && e->name() == name;
    });

  if (it != entities().end())
    return std::static_pointer_cast<Namespace>(*it);

  auto result = std::make_shared< Namespace>(name, shared_from_this());
  entities().push_back(result);
  return result;
}

std::shared_ptr<Class> Namespace::createClass(std::string name)
{
  auto result = std::make_shared<Class>(std::move(name), shared_from_this());
  entities().push_back(result);
  return result;
}

std::shared_ptr<Enum> Namespace::createEnum(std::string name)
{
  auto result = std::make_shared<Enum>(std::move(name), shared_from_this());
  entities().push_back(result);
  return result;
}

std::shared_ptr<Function> Namespace::createFunction(std::string name)
{
  auto result = std::make_shared<Function>(std::move(name), shared_from_this());
  entities().push_back(result);
  return result;
}

} // namespace cxx
