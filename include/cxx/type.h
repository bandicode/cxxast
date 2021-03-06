// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPE_H
#define CXXAST_TYPE_H

#include "cxx/entity.h"

#include <cassert>
#include <memory>
#include <vector>

namespace cxx
{

enum class CVQualifier
{
  None,
  Const,
  Volatile,
  ConstVolatile = Const | Volatile,
};

inline CVQualifier make_cv_qual(bool is_const, bool is_volatile)
{
  return (is_const && is_volatile) ? CVQualifier::ConstVolatile
    : (is_const ? CVQualifier::Const : 
        (is_volatile ? CVQualifier::Volatile : CVQualifier::None));
}

enum class Reference
{
  None,
  LValue,
  RValue,
};

class CXXAST_API IType : public std::enable_shared_from_this<IType>
{
public:
  IType() = default;
  virtual ~IType();

  virtual bool isSimple() const;
  virtual bool isAuto() const;
  virtual bool isDecltypeAuto() const;
  virtual bool isConst() const;
  virtual bool isVolatile() const;
  virtual bool isCVQualified() const;
  virtual bool isPointer() const;
  virtual bool isReference() const;
  virtual bool isLValueReference() const;
  virtual bool isRValueReference() const;
  virtual bool isFunctionType() const;

  virtual const std::string& name() const;

  virtual std::shared_ptr<const IType> withoutPointer() const;
  virtual std::shared_ptr<const IType> withoutReference() const;
  virtual std::shared_ptr<const IType> withoutCV() const;

  virtual std::string toString() const = 0;
};

class CXXAST_API Type
{
private:
  std::shared_ptr<const IType> d;

public:
  Type();
  Type(const Type&) = default;
  Type(Type&&) = default;
  ~Type();

  explicit Type(std::shared_ptr<const IType> t);
  explicit Type(std::string name, CVQualifier cvqual = CVQualifier::None, Reference ref = Reference::None);

  static const Type Int;
  static const Type Void;
  static const Type Auto;
  static const Type DecltypeAuto;

  bool isAuto() const;
  bool isDecltypeAuto() const;
  bool isReference() const;
  bool isPointer() const;
  bool isFunction() const;
  bool isCVQualified() const;

  Reference reference() const;
  CVQualifier cvQualification() const;

  Type pointee() const;

  static Type cvQualified(const Type& t, CVQualifier cvqual);
  static Type reference(const Type& t, Reference ref = Reference::LValue);
  static Type pointer(const Type& t);
  static Type function(const Type& rt, std::vector<Type> params);

  Type resultType() const;
  const std::vector<Type>& parameters() const;

  std::string toString() const;

  std::shared_ptr<const IType> impl() const;

  Type& operator=(const Type&) = default;
  Type& operator=(Type&&) = default;
};

CXXAST_API bool operator==(const Type& lhs, const Type& rhs);
inline bool operator!=(const Type& lhs, const Type& rhs) { return !(lhs == rhs); }

} // namespace cxx

namespace cxx
{

inline Type::Type(std::shared_ptr<const IType> t)
  : d(t)
{
  assert(d != nullptr);
}

} // namespace cxx

#endif // CXXAST_TYPE_H
