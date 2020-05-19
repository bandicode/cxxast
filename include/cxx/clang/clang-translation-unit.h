// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLANG_TRANSLATION_UNIT_H
#define CXXAST_CLANG_TRANSLATION_UNIT_H

#include "cxx/libclang.h"

namespace cxx
{

class ClangCursor;
class ClangTokenSet;

class CXXAST_API ClangTranslationUnit
{
public:
  LibClang* libclang = nullptr;
  CXTranslationUnit translation_unit = nullptr;

  ClangTranslationUnit() = default;

  ClangTranslationUnit(LibClang& lib, CXTranslationUnit tu)
    : libclang(&lib), translation_unit(tu)
  {

  }

  ClangTranslationUnit(const ClangTranslationUnit&) = delete;

  ClangTranslationUnit(ClangTranslationUnit&& other) noexcept
    : libclang(other.libclang), translation_unit(other.translation_unit)
  {
    other.translation_unit = nullptr;
  }

  ~ClangTranslationUnit();

  ClangTranslationUnit& operator=(const ClangTranslationUnit&) = delete;

  ClangTranslationUnit& operator=(ClangTranslationUnit&& other);

  operator CXTranslationUnit() const
  {
    return translation_unit;
  }

  ClangCursor getCursor() const;

  ClangTokenSet tokenize(CXSourceRange range) const;
};

} // namespace cxx

#endif // CXXAST_CLANG_TRANSLATION_UNIT_H
