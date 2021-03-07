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

class AstNode;

class File
{
private:
  std::string m_path;

public:
  std::shared_ptr<AstNode> ast;

public:
  explicit File(std::string path);

  const std::string& path() const;

  static void normalizePath(std::string& path);
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

inline void File::normalizePath(std::string& path)
{
  for (char& c : path)
  {
    if (c == '\\')
      c = '/';
  }
}

} // namespace cxx

#endif // CXXAST_FILE_H
