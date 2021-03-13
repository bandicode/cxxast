// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/expression.h"
#include "cxx/expressions.h"

#include <stdexcept>

namespace cxx
{

const Expression Expression::Zero = Expression{ std::make_shared<UnexposedExpression>("0") };

IExpression::~IExpression()
{

}

bool IExpression::isUnexposed() const
{
  return false;
}


UnexposedExpression::UnexposedExpression(std::string src)
  : m_source(std::move(src))
{

}

NodeKind UnexposedExpression::node_kind() const
{
  return NodeKind::UnexposedExpression;
}

bool UnexposedExpression::isUnexposed() const
{
  return true;
}

std::string UnexposedExpression::toString() const
{
  return m_source;
}
  

Expression::Expression(std::string content)
  : Handle<IExpression>(std::make_shared<UnexposedExpression>(std::move(content)))
{

}

bool Expression::isUnexposed() const
{
  return d->isUnexposed();
}

std::string Expression::toString() const
{
  return d->toString();
}

bool operator==(const Expression& lhs, const Expression& rhs)
{
  if (lhs.impl() == rhs.impl())
    return true;

  if (lhs.isNull() || rhs.isNull())
    return false;

  if (typeid(*lhs.impl().get()) != typeid(*rhs.impl().get()))
    return false;

  return lhs.toString() == rhs.toString();
}

} // namespace cxx