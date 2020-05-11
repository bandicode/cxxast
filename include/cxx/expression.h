// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_EXPRESSION_H
#define CXXAST_EXPRESSION_H

#include "cxx/node.h"

#include <cassert>
#include <memory>
#include <string>

namespace cxx
{

class CXXAST_API IExpression : public std::enable_shared_from_this<IExpression>
{
public:
  IExpression() = default;
  virtual ~IExpression();

  virtual bool isUnexposed() const;

  virtual std::string toString() const = 0;
};

class CXXAST_API Expression
{
private:
  std::shared_ptr<const IExpression> d;

public:
  Expression();
  Expression(const Expression&) = default;
  Expression(Expression&&) = default;
  ~Expression();

  explicit Expression(std::shared_ptr<const IExpression> e);
  explicit Expression(std::string content);

  static const Expression Zero;

  bool isUnexposed() const;

  std::string toString() const;

  std::shared_ptr<const IExpression> impl() const;

  Expression& operator=(const Expression&) = default;
  Expression& operator=(Expression&&) = default;
};

CXXAST_API bool operator==(const Expression& lhs, const Expression& rhs);
inline bool operator!=(const Expression& lhs, const Expression& rhs) { return !(lhs == rhs); }

} // namespace cxx

namespace cxx
{

inline Expression::Expression(std::shared_ptr<const IExpression> e)
  : d(e)
{
  assert(d != nullptr);
}

} // namespace cxx

#endif // CXXAST_EXPRESSION_H
