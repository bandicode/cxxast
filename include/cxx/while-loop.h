// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_WHILELOOP_H
#define CXXAST_WHILELOOP_H

#include "cxx/expression.h"
#include "cxx/statement.h"

#include <vector>

namespace cxx
{

class CXXAST_API WhileLoop : public Statement
{
public:
  Expression condition;
  std::shared_ptr<Statement> body;
  
public:
  WhileLoop() = default;

  WhileLoop(const Expression& cond, std::shared_ptr<Statement> b);

  static constexpr NodeKind ClassNodeKind = NodeKind::WhileLoop;
  NodeKind node_kind() const override;

  struct Condition : public priv::Field<WhileLoop, Expression>
  {
    static Expression& get(Node& n)
    {
      return down_cast(n).condition;
    }
    
    static void set(Node& n, const Expression& expr)
    {
      down_cast(n).condition = expr;
    }
  };
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_WHILELOOP_H
