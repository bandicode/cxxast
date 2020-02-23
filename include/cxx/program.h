// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_PROGRAM_H
#define CXXAST_PROGRAM_H

#include "cxx/cxxast-defs.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cxx
{

class Entity;
class File;
class Name;
class Namespace;
class Type;

class CXXAST_API Program
{
public:
  Program();
  ~Program();

  const std::vector<std::shared_ptr<File>>& files() const;

  const std::shared_ptr<Namespace>& globalNamespace() const;

  std::shared_ptr<Entity> resolve(const Name& n);
  std::shared_ptr<Entity> resolve(const Name& n, const std::shared_ptr<Entity>& context);

private:
  std::vector<std::shared_ptr<File>> m_files;
  std::shared_ptr<Namespace> m_global_namespace;
};

} // namespace cxx

namespace cxx
{

inline const std::vector<std::shared_ptr<File>>& Program::files() const
{
  return m_files;
}

inline const std::shared_ptr<Namespace>& Program::globalNamespace() const
{
  return m_global_namespace;
}

} // namespace cxx

#endif // CXXAST_PROGRAM_H
