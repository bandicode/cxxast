// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/filesystem.h"

namespace cxx
{

FileSystem::FileSystem()
{

}

FileSystem& FileSystem::GlobalInstance()
{
  static FileSystem fs = {};
  return fs;
}

std::shared_ptr<File> FileSystem::get(const std::string& path)
{
  auto it = std::find_if(files.begin(), files.end(), [&path](const std::shared_ptr<File>& file) {
    return file->path() == path;
    });

  if (it != files.end())
    return *it;

  auto file = std::make_shared<File>(path);
  files.push_back(file);

  return file;
}

} // namespace cxx
