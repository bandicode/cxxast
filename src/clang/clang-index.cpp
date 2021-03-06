// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/clang/clang-index.h"

#include "cxx/clang/clang-translation-unit.h"

namespace cxx
{

ClangTranslationUnit ClangIndex::parseTranslationUnit(const std::string& file, const std::set<std::string>& includedirs, int options)
{
  const char* command_line_args[128] = { nullptr };
  std::vector<std::string> argv{ "-x", "c++", "-Xclang", "-ast-dump", "-fsyntax-only" };
  for (const std::string& f : includedirs)
  {
    argv.push_back("--include-directory");
    argv.push_back(f);
  }
  for (int i(0); i < argv.size(); ++i)
    command_line_args[i] = argv.at(i).data();

  CXTranslationUnit tu = nullptr;

  CXErrorCode error = libclang.clang_parseTranslationUnit2(this->index, file.data(), command_line_args, static_cast<int>(argv.size()), nullptr, 0, options, &tu);

  if (error)
    throw std::runtime_error{ "Could not parse translation unit" };

  return ClangTranslationUnit{ libclang, tu };
}

} // namespace cxx

