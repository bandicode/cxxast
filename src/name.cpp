// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/name.h"
#include "cxx/name_p.h"

#include <stdexcept>

namespace cxx
{

const Name Name::Int = Name{ std::make_shared<details::Identifier>("int") };

namespace details
{

IName::~IName()
{

}

bool IName::isIdentifier() const
{
  return false;
}

bool IName::isOperatorName() const
{
  return false;
}

bool IName::isConversionFunctionName() const
{
  return false;
}

bool IName::isLiteralOperatorName() const
{
  return false;
}

bool IName::isTemplateName() const
{
  return false;
}

bool IName::isDestructorName() const
{
  return false;
}

bool IName::isQualifiedName() const
{
  return false;
}


Identifier::Identifier(std::string name)
  : m_name(std::move(name))
{

}

bool Identifier::isIdentifier() const
{
  return true;
}

std::string Identifier::toString() const
{
  return m_name;
}

OverloadedOperatorName::OverloadedOperatorName(std::string opid)
  : m_op(std::move(opid))
{

}

bool OverloadedOperatorName::isOperatorName() const
{
  return true;
}

std::string OverloadedOperatorName::toString() const
{
  return std::string("operator") + m_op;
}

ConversionFunctionName::ConversionFunctionName(const Type& t)
  : m_type(t)
{

}

bool ConversionFunctionName::isConversionFunctionName() const
{
  return true;
}

std::string ConversionFunctionName::toString() const
{
  return "operator " + m_type.toString();
}

LiteralOperatorName::LiteralOperatorName(std::string suffix)
  : m_suffix(std::move(suffix))
{

}

bool LiteralOperatorName::isLiteralOperatorName() const
{
  return true;
}

std::string LiteralOperatorName::toString() const
{
  return "operator \"\"" + m_suffix;
}

TemplateName::TemplateName(std::string name, std::vector<TemplateArgument> targ)
  : m_name(std::move(name)),
  m_targs(std::move(targ))
{

}

bool TemplateName::isTemplateName() const
{
  return true;
}

std::string TemplateName::toString() const
{
  std::string result = m_name;

  result += "<";

  for (const auto& t : m_targs)
  {
    result += t.toString() + ", ";
  }

  if (!m_targs.empty())
  {
    result.pop_back();
    result.pop_back();
  }

  result += ">";

  return result;
}

DestructorName::DestructorName(std::shared_ptr<const IName> type)
  : m_name(type)
{

}

bool DestructorName::isDestructorName() const
{
  return true;
}

std::string DestructorName::toString() const
{
  return "~" + m_name->toString();
}

QualifiedName::QualifiedName(std::shared_ptr<const IName> lhs, std::shared_ptr<const IName> rhs)
  : m_lhs(lhs), m_rhs(rhs)
{

}

bool QualifiedName::isQualifiedName() const
{
  return true;
}

std::string QualifiedName::toString() const
{
  return m_lhs->toString() + "::" + m_rhs->toString();
}

} // namespace details

Name::Name()
  : d(Name::Int.d)
{

}

Name::~Name()
{

}

Name::Name(std::shared_ptr<const details::IName> n)
  : d(n)
{
  assert(n != nullptr);
}

bool Name::isIdentifier() const
{
  return d->isIdentifier();
}

bool Name::isOperatorName() const
{
  return d->isOperatorName();
}

bool Name::isConversionFunctionName() const
{
  return d->isConversionFunctionName();
}

bool Name::isLiteralOperatorName() const
{
  return d->isLiteralOperatorName();
}

bool Name::isTemplateName() const
{
  return d->isTemplateName();
}

bool Name::isDestructorName() const
{
  return d->isDestructorName();
}

bool Name::isQualifiedName() const
{
  return d->isQualifiedName();
}

std::string Name::toString() const
{
  return d->toString();
}

std::shared_ptr<const details::IName> Name::impl() const
{
  return d;
}

} // namespace cxx