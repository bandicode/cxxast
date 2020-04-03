// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DOCUMENTATION_H
#define CXXAST_DOCUMENTATION_H

#include "cxx/node.h"

#include <memory>
#include <utility>
#include <vector>

namespace cxx
{

class CXXAST_API Documentation : public Node
{
public:
  Documentation() = default;
  virtual ~Documentation();

  explicit Documentation(const SourceLocation& loc);

  NodeKind node_kind() const override;
  bool isDocumentation() const override;
};

class CXXAST_API MultilineComment : public Documentation
{
private:
  std::string m_text;

public:
  explicit MultilineComment(std::string text, const SourceLocation& loc = {});

  static constexpr NodeKind ClassNodeKind = NodeKind::MultilineComment;
  NodeKind node_kind() const override;

  const std::string& text() const;
  std::string& text();
};

} // namespace cxx

namespace cxx
{

inline Documentation::Documentation(const SourceLocation& loc)
{
  location() = loc;
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
