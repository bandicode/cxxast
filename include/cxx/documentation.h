// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DOCUMENTATION_H
#define CXXAST_DOCUMENTATION_H

#include "cxx/cxxast-defs.h"

#include "cxx/sourcelocation.h"

#include <memory>
#include <utility>
#include <vector>

namespace cxx
{

class CXXAST_API Documentation
{
private:
  SourceLocation m_location;

public:
  Documentation() = default;
  virtual ~Documentation();

  explicit Documentation(const SourceLocation& loc);

  virtual const std::string& type() const = 0;

  const SourceLocation& location() const;
  SourceLocation& location();
};

class CXXAST_API MultilineComment : public Documentation
{
private:
  std::string m_text;

public:
  explicit MultilineComment(std::string text, const SourceLocation& loc = {});

  static const std::string TypeId;
  const std::string& type() const override;

  const std::string& text() const;
  std::string& text();
};

} // namespace cxx

namespace cxx
{

inline Documentation::Documentation(const SourceLocation& loc)
  : m_location(loc)
{

}

inline const SourceLocation& Documentation::location() const
{
  return m_location;
}

inline SourceLocation& Documentation::location()
{
  return m_location;
}

inline MultilineComment::MultilineComment(std::string text, const SourceLocation& loc)
  : Documentation(loc),
  m_text(std::move(text))
{

}

inline const std::string& MultilineComment::text() const
{
  return m_text;
}

inline std::string& MultilineComment::text()
{
  return m_text;
}

} // namespace cxx

#endif // CXXAST_DOCUMENTATION_H
