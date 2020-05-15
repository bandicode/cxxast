// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FILE_H
#define CXXAST_FILE_H

#include "cxx/cxxast-defs.h"

#include <memory>
#include <string>
#include <vector>

namespace cxx
{

class Node;

class File
{
private:
  std::string m_path;

public:
  std::vector<std::shared_ptr<Node>> nodes;

public:
  explicit File(std::string path);

  const std::string& path() const;
};

} // namespace cxx

namespace cxx
{

inline File::File(std::string path)
  : m_path(std::move(path))
{

}

inline const std::string& File::path() const
{
  return m_path;
}

} // namespace cxx

#endif // CXXAST_FILE_H
