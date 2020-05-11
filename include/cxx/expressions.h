// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_EXPRESSIONS_H
#define CXXAST_EXPRESSIONS_H

#include "cxx/expression.h"

namespace cxx
{

class CXXAST_API UnexposedExpression : public IExpression
{
private:
  std::string m_source;

public:
  explicit UnexposedExpression(std::string src);

  bool isUnexposed() const override;
  std::string toString() const override;
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_EXPRESSIONS_H
