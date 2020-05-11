// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FILESYSTEM_H
#define CXXAST_FILESYSTEM_H

#include "cxx/file.h"

#include <vector>

namespace cxx
{

class CXXAST_API FileSystem
{
public:
  std::vector<std::shared_ptr<File>> files;

public:
  FileSystem();

  static FileSystem& GlobalInstance();

  std::shared_ptr<File> get(const std::string& path);
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_FILESYSTEM_H
