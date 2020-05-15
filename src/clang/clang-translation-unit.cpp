// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/clang/clang-translation-unit.h"

#include "cxx/clang/clang-cursor.h"

namespace cxx
{

ClangTranslationUnit::~ClangTranslationUnit()
{
  if(translation_unit)
    libclang->clang_disposeTranslationUnit(translation_unit);
}

ClangTranslationUnit& ClangTranslationUnit::operator=(ClangTranslationUnit&& other)
{
  this->libclang = other.libclang;

  if (this->translation_unit)
    libclang->clang_disposeTranslationUnit(this->translation_unit);

  this->translation_unit = other.translation_unit;
  other.translation_unit = nullptr;

  return *(this);
}

ClangCursor ClangTranslationUnit::getCursor() const
{
  CXCursor c = libclang->clang_getTranslationUnitCursor(this->translation_unit);
  return ClangCursor{ *libclang, c };
}

} // namespace cxx

