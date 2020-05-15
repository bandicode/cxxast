// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLANG_INDEX_H
#define CXXAST_CLANG_INDEX_H

#include "cxx/libclang.h"

#include <set>

namespace cxx
{

class ClangTranslationUnit;

class CXXAST_API ClangIndex
{
public:
  LibClang& libclang;
  CXIndex index;

  explicit ClangIndex(LibClang& lib)
    : libclang(lib)
  {
    index = libclang.clang_createIndex(0, 0);
  }

  ClangIndex(const ClangIndex&) = delete;
  
  ClangIndex(ClangIndex&& other) noexcept
    : libclang(other.libclang),
      index(other.index)
  {
    other.index = nullptr;
  }

  ~ClangIndex()
  {
    if (this->index)
    {
      libclang.clang_disposeIndex(this->index);
    }
  }

  ClangTranslationUnit parseTranslationUnit(const std::string& file, const std::set<std::string>& includedirs, int options = 0);
};

} // namespace cxx

#endif // CXXAST_CLANG_INDEX_H
