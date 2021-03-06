// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_SOURCELOCATION_H
#define CXXAST_SOURCELOCATION_H

#include "cxx/file.h"

#include <memory>

namespace cxx
{

class SourceLocation
{
private:
  std::weak_ptr<File> m_file;
  int m_line = -1;
  int m_column = -1;

public:
  SourceLocation() = default;
  ~SourceLocation() = default;

  SourceLocation(std::shared_ptr<File> file, int line, int col);

  std::shared_ptr<File> file() const;
  int line() const;
  int column() const;
};

inline bool operator==(const SourceLocation& lhs, const SourceLocation& rhs)
{
  return lhs.file() == rhs.file() && lhs.line() == rhs.line() && lhs.column() == rhs.column();
}

inline bool operator!=(const SourceLocation& lhs, const SourceLocation& rhs)
{
  return !(lhs == rhs);
}

} // namespace cxx

namespace cxx
{

inline SourceLocation::SourceLocation(std::shared_ptr<File> file, int line, int col)
  : m_file(file),
  m_line(line),
  m_column(col)
{

}

inline std::shared_ptr<File> SourceLocation::file() const
{
  return m_file.lock();
}

inline int SourceLocation::line() const
{
  return m_line;
}

inline int SourceLocation::column() const
{
  return m_column;
}

} // namespace cxx

#endif // CXXAST_SOURCELOCATION_H
