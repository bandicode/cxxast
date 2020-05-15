// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DECLARATION_H
#define CXXAST_DECLARATION_H

#include "cxx/statement.h"

namespace cxx
{

class CXXAST_API Declaration : public Statement
{
public:
  std::vector<std::shared_ptr<Declaration>> declarations;

public:
  Declaration() = default;

  explicit Declaration(SourceLocation loc);

  std::shared_ptr<Declaration> shared_from_this();

  bool isDeclaration() const override;

  size_t childCount() const override;
  std::shared_ptr<Node> childAt(size_t index) const override;
  void appendChild(std::shared_ptr<Node> n) override;
};

} // namespace cxx

namespace cxx
{

inline Declaration::Declaration(SourceLocation loc)
  : Statement(loc)
{

}

} // namespace cxx

#endif // CXXAST_DECLARATION_H