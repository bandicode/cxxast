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

bool UnexposedExpression::isUnexposed() const
{
  return true;
}

std::string UnexposedExpression::toString() const
{
  return m_source;
}
  

Expression::Expression()
  : d(Expression::Zero.d)
{

}

Expression::Expression(std::string content)
  : d(std::make_shared<UnexposedExpression>(std::move(content)))
{

}

bool Expression::isUnexposed() const
{
  return d->isUnexposed();
}

Expression::~Expression()
{

}

std::string Expression::toString() const
{
  return d->toString();
}

std::shared_ptr<const IExpression> Expression::impl() const
{
  return d;
}

bool operator==(const Expression& lhs, const Expression& rhs)
{
  if (lhs.impl() == rhs.impl())
    return true;

  if (typeid(*lhs.impl().get()) != typeid(*rhs.impl().get()))
    return false;

  return lhs.toString() == rhs.toString();
}

} // namespace cxx