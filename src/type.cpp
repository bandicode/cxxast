// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/type.h"

#include <stdexcept>

namespace cxx
{

const Type Type::Int = Type{ std::make_shared<type_system::SimpleType>("int") };
const Type Type::Auto = Type{ std::make_shared<type_system::AutoType>() };
const Type Type::DecltypeAuto = Type{ std::make_shared<type_system::DecltypeAuto>() };

namespace type_system
{

Type::~Type()
{

}

bool Type::isSimple() const
{
  return false;
}

bool Type::isAuto() const
{
  return false;
}

bool Type::isDecltypeAuto() const
{
  return false;
}

bool Type::isConst() const
{
  return false;
}

bool Type::isVolatile() const
{
  return false;
}

bool Type::isCVQualified() const
{
  return false;
}

bool Type::isPointer() const
{
  return false;
}

bool Type::isReference() const
{
  return false;
}

bool Type::isLValueReference() const
{
  return false;
}

bool Type::isRValueReference() const
{
  return false;
}

const std::string& Type::name() const
{
  throw std::runtime_error{ "Type::name() not supported for this instance of Type" };
}

std::shared_ptr<const Type> Type::withoutPointer() const
{
  return shared_from_this();
}

std::shared_ptr<const Type> Type::withoutReference() const
{
  return shared_from_this();
}

std::shared_ptr<const Type> Type::withoutCV() const
{
  return shared_from_this();
}


SimpleType::SimpleType(std::string name)
  : m_name(std::move(name))
{

}

bool SimpleType::isSimple() const
{
  return true;
}

const std::string& SimpleType::name() const
{
  return m_name;
}

std::string SimpleType::toString() const
{
  return m_name;
}


AutoType::AutoType()
{

}

bool AutoType::isAuto() const
{
  return true;
}

std::string AutoType::toString() const
{
  return "auto";
}


DecltypeAuto::DecltypeAuto()
{

}

bool DecltypeAuto::isDecltypeAuto() const
{
  return true;
}

std::string DecltypeAuto::toString() const
{
  return "decltype(auto)";
}


CVQualifiedType::CVQualifiedType(CVQualifier qual, std::shared_ptr<const Type> type)
  : m_type(type), m_qualifier(qual)
{
  assert(m_qualifier != CVQualifier::None);
}

bool CVQualifiedType::isConst() const
{
  return (static_cast<int>(m_qualifier) & static_cast<int>(CVQualifier::Const));
}

bool CVQualifiedType::isVolatile() const
{
  return (static_cast<int>(m_qualifier)& static_cast<int>(CVQualifier::Volatile));
}

bool CVQualifiedType::isCVQualified() const
{
  return true;
}

std::shared_ptr<const Type> CVQualifiedType::withoutCV() const
{
  return m_type;
}

std::string CVQualifiedType::toString() const
{
  std::string result = isConst() ? "const " : "";
  result += isVolatile() ? "volatile " : "";
  result += m_type->toString();
  return result;
}

ReferenceType::ReferenceType(Reference ref, std::shared_ptr<const Type> type)
  : m_type(type), m_reference(ref)
{
  assert(ref != Reference::None);
}

bool ReferenceType::isReference() const
{
  return true;
}

bool ReferenceType::isLValueReference() const
{
  return m_reference == Reference::LValue;
}

bool ReferenceType::isRValueReference() const
{
  return m_reference == Reference::RValue;
}

std::shared_ptr<const Type> ReferenceType::withoutReference() const
{
  return m_type;
}

std::string ReferenceType::toString() const
{
  std::string result = m_type->toString();
  result += (isLValueReference() ? "&" : "");
  result += (isRValueReference() ? "&&" : "");
  return result;
}

PointerType::PointerType(std::shared_ptr<const Type> type)
  : m_type(type)
{

}

bool PointerType::isPointer() const
{
  return true;
}

std::shared_ptr<const Type> PointerType::withoutPointer() const
{
  return m_type;
}

std::string PointerType::toString() const
{
  return m_type->toString() + "*";
}

} // namespace type_system

Type::Type()
  : d(Type::Int.d)
{

}

Type::~Type()
{

}

Type::Type(std::string name, CVQualifier cvqual, Reference ref)
{
  d = [&name]() -> std::shared_ptr<const type_system::Type> {
    if (name == "auto")
      return Type::Auto.impl();
    else if (name == "decltype(auto)")
      return Type::DecltypeAuto.impl();
    return std::make_shared<const type_system::SimpleType>(std::move(name));
  }();

  if (cvqual != CVQualifier::None)
    d = std::make_shared<const type_system::CVQualifiedType>(cvqual, d);

  if (ref != Reference::None)
    d = std::make_shared<const type_system::ReferenceType>(ref, d);
}

bool Type::isAuto() const
{
  return d->isAuto();
}

bool Type::isDecltypeAuto() const
{
  return d->isConst();
}

bool Type::isReference() const
{
  return d->isReference();
}

bool Type::isPointer() const
{
  return d->isPointer();
}

bool Type::isCVQualified() const
{
  return d->isCVQualified();
}

Reference Type::reference() const
{
  return isReference() ? std::static_pointer_cast<const type_system::ReferenceType>(d)->reference() : Reference::None;
}

CVQualifier Type::cvQualification() const
{
  return isCVQualified() ? std::static_pointer_cast<const type_system::CVQualifiedType>(d)->cvqualifier() : CVQualifier::None;
}

std::string Type::toString() const
{
  return d->toString();
}

std::shared_ptr<const type_system::Type> Type::impl() const
{
  return d;
}

bool operator==(const Type& lhs, const Type& rhs)
{
  if (lhs.impl() == rhs.impl())
    return true;

  if (typeid(*lhs.impl().get()) != typeid(*rhs.impl().get()))
    return false;

  return lhs.toString() == rhs.toString();
}

} // namespace cxx