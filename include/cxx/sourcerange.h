// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_SOURCERANGE_H
#define CXXAST_SOURCERANGE_H

#include "cxx/sourcelocation.h"

#include <cassert>

namespace cxx
{

class SourceRange
{
public:
  std::weak_ptr<File> file;
  
  struct Position
  {
    int line = -1;
    int column = -1;
  };

  Position begin;
  Position end;

public:
  SourceRange() = default;
  ~SourceRange() = default;

  SourceRange(std::shared_ptr<File> f, Position b, Position e);
  SourceRange(SourceLocation b, SourceLocation e);

  SourceLocation locbegin() const;
  SourceLocation locend() const;
};

} // namespace cxx

namespace cxx
{

inline SourceRange::SourceRange(std::shared_ptr<File> f, Position b, Position e)
  : file(f),
    begin(b),
    end(e)
{

}

inline SourceRange::SourceRange(SourceLocation b, SourceLocation e)
  : file(b.file()),
    begin{ b.line(), b.column() },
    end{ e.line(), e.column() }
{
  assert(e.file() == b.file());
}

inline SourceLocation SourceRange::locbegin() const
{
  return SourceLocation{ file.lock(), begin.line, begin.column };
}

inline SourceLocation SourceRange::locend() const
{
  return SourceLocation{ file.lock(), end.line, end.column };
}

} // namespace cxx

#endif // CXXAST_SOURCERANGE_H
