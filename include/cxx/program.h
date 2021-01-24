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

class AstNode;
class IEntity;
class File;
class Macro;
class Name;
class Namespace;
class INode;
class Type;

class CXXAST_API Program
{
public:
  std::vector<std::shared_ptr<Macro>> macros;
  std::map<INode*, std::shared_ptr<AstNode>> astmap;

public:
  Program();
  ~Program();

  const std::vector<std::shared_ptr<File>>& files() const;

  const std::shared_ptr<Namespace>& globalNamespace() const;

  std::shared_ptr<IEntity> resolve(const Name& n);
  std::shared_ptr<IEntity> resolve(const Name& n, const std::shared_ptr<IEntity>& context);

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
