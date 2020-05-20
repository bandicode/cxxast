// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/clang/clang-cursor.h"

namespace cxx
{

size_t ClangCursor::childCount() const
{
  size_t counter = 0;

  visitChildren([&counter](const ClangCursor&) {
    ++counter;
    });

  return counter;
}

ClangCursor ClangCursor::childAt(size_t index) const
{
  ClangCursor result{ *libclang, this->cursor };

  visitChildren([&result, &index](const ClangCursor& c) {
    if (index == 0)
      result = c;
    --index;
    });

  return result;
}

} // namespace cxx

