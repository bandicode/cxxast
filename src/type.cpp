// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/type.h"
#include "cxx/types.h"

#include <stdexcept>

namespace cxx
{

const Type Type::Int = Type{ std::make_shared<SimpleType>("int") };
const Type Type::Void = Type{ std::make_shared<SimpleType>("void") };
const Type Type::Auto = Type{ std::make_shared<AutoType>() };
const Type Type::DecltypeAuto = Type{ std::make_shared<DecltypeAutoType>() };

IType::~IType()
{

}

bool IType::isSimple() const
{
  return false;
}

bool IType::isAuto() const
{
  return false;
}

bool IType::isDecltypeAuto() const
{
  return false;
}

bool IType::isConst() const
{
  return false;
}

bool IType::isVolatile() const
{
  return false;
}

bool IType::isCVQualified() const
{
  return false;
}

bool IType::isPointer() const
{
  return false;
}

bool IType::isReference() const
{
  return false;
}

bool IType::isLValueReference() const
{
  return false;
}

bool IType::isRValueReference() const
{
  return false;
}

bool IType::isFunctionType() const
{
  return false;
}

const std::string& IType::name() const
{
  throw std::runtime_error{ "Type::name() not supported for this instance of Type" };
}

std::shared_ptr<const IType> IType::withoutPointer() const
{
  return shared_from_this();
}

std::shared_ptr<const IType> IType::withoutReference() const
{
  return shared_from_this();
}

std::shared_ptr<const IType> IType::withoutCV() const
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


DecltypeAutoType::DecltypeAutoType()
{

}

bool DecltypeAutoType::isDecltypeAuto() const
{
  return true;
}

std::string DecltypeAutoType::toString() const
{
  return "decltype(auto)";
}


CVQualifiedType::CVQualifiedType(CVQualifier qual, std::shared_ptr<const IType> type)
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

std::shared_ptr<const IType> CVQualifiedType::withoutCV() const
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

ReferenceType::ReferenceType(Reference ref, std::shared_ptr<const IType> type)
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

std::shared_ptr<const IType> ReferenceType::withoutReference() const
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

PointerType::PointerType(std::shared_ptr<const IType> type)
  : m_type(type)
{

}

bool PointerType::isPointer() const
{
  return true;
}

std::shared_ptr<const IType> PointerType::withoutPointer() const
{
  return m_type;
}

std::string PointerType::toString() const
{
  return m_type->toString() + "*";
}

FunctionType::FunctionType(std::shared_ptr<const IType> rt, std::vector<Type> pts)
  : m_result_type(rt), m_parameter_types(std::move(pts))
{

}

Type FunctionType::resultType() const
{
  return Type{ m_result_type };
}

const std::vector<Type>& FunctionType::parameters() const
{
  return m_parameter_types;
}

bool FunctionType::isFunctionType() const
{
  return true;
}

std::string FunctionType::toString() const
{
  std::string ret = m_result_type->toString();

  ret += "(";

  for (const auto& pt : m_parameter_types)
  {
    ret += pt.toString() + ", ";
  }

  if (!m_parameter_types.empty())
  {
    ret.pop_back();
    ret.pop_back();
  }

  ret += ")";

  return ret;
}


Type::Type()
  : d(Type::Int.d)
{

}

Type::~Type()
{

}

Type::Type(std::string name, CVQualifier cvqual, Reference ref)
{
  d = [&name]() -> std::shared_ptr<const IType> {
    if (name == "auto")
      return Type::Auto.impl();
    else if (name == "decltype(auto)")
      return Type::DecltypeAuto.impl();
    return std::make_shared<const SimpleType>(std::move(name));
  }();

  if (cvqual != CVQualifier::None)
    d = std::make_shared<const CVQualifiedType>(cvqual, d);

  if (ref != Reference::None)
    d = std::make_shared<const ReferenceType>(ref, d);
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

bool Type::isFunction() const
{
  return d->isFunctionType();
}

bool Type::isCVQualified() const
{
  return d->isCVQualified();
}

Reference Type::reference() const
{
  return isReference() ? std::static_pointer_cast<const ReferenceType>(d)->reference() : Reference::None;
}

CVQualifier Type::cvQualification() const
{
  return isCVQualified() ? std::static_pointer_cast<const CVQualifiedType>(d)->cvqualifier() : CVQualifier::None;
}

Type Type::pointee() const
{
  assert(isReference() || isPointer());

  if (isReference())
    return Type{ static_cast<const ReferenceType*>(d.get())->withoutReference() };
  else
    return Type{ static_cast<const PointerType*>(d.get())->withoutPointer() };
}

Type Type::cvQualified(const Type& t, CVQualifier cvqual)
{
  if (cvqual == CVQualifier::None)
    return t;

  return Type{ std::make_shared<const CVQualifiedType>(cvqual, t.impl()) };
}

Type Type::reference(const Type& t, Reference ref)
{
  if (ref == Reference::None)
    return t;

  return Type{ std::make_shared<const ReferenceType>(ref, t.impl()) };
}

Type Type::pointer(const Type& t)
{
  return Type{ std::make_shared<const PointerType>(t.impl()) };
}

Type Type::function(const Type& rt, std::vector<Type> params)
{
  return Type{ std::make_shared<FunctionType>(rt.impl(), std::move(params)) };
}

Type Type::resultType() const
{
  assert(isFunction());
  return static_cast<const FunctionType*>(d.get())->resultType();
}

const std::vector<Type>& Type::parameters() const
{
  assert(isFunction());
  return static_cast<const FunctionType*>(d.get())->parameters();
}

std::string Type::toString() const
{
  return d->toString();
}

std::shared_ptr<const IType> Type::impl() const
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