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

class CXXAST_API IExpression : public AstNode
{
public:
  IExpression() = default;
  ~IExpression();

  virtual bool isUnexposed() const;

  virtual std::string toString() const = 0;
};

class CXXAST_API Expression : public Handle<IExpression>
{
public:
  using Handle<IExpression>::Handle;

  explicit Expression(std::string content);

  static const Expression Zero;

  bool isUnexposed() const;

  std::string toString() const;

  Expression& operator=(const Expression&) = default;
};

CXXAST_API bool operator==(const Expression& lhs, const Expression& rhs);
inline bool operator!=(const Expression& lhs, const Expression& rhs) { return !(lhs == rhs); }

inline bool is_null(const Expression& expr)
{
  return expr.isNull();
}

inline std::shared_ptr<AstNode> to_ast_node(const Expression& expr)
{
  return expr.impl();
}

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_EXPRESSION_H
