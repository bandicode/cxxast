// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPE_H
#define CXXAST_TYPE_H

#include "cxx/entity.h"

#include <cassert>
#include <memory>

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

namespace type_system
{

class CXXAST_API Type : public std::enable_shared_from_this<Type>
{
public:
  Type() = default;
  virtual ~Type();

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

  virtual const std::string& name() const;

  virtual std::shared_ptr<const Type> withoutPointer() const;
  virtual std::shared_ptr<const Type> withoutReference() const;
  virtual std::shared_ptr<const Type> withoutCV() const;

  virtual std::string toString() const = 0;
};

class CXXAST_API SimpleType : public Type
{
private:
  std::string m_name;

public:
  explicit SimpleType(std::string name);

  bool isSimple() const override;
  const std::string& name() const override;
  std::string toString() const override;
};

class CXXAST_API AutoType : public Type
{
public:
  AutoType();

  bool isAuto() const override;
  std::string toString() const override;
};

class CXXAST_API DecltypeAuto : public Type
{
public:
  DecltypeAuto();

  bool isDecltypeAuto() const override;
  std::string toString() const override;
};

class CXXAST_API CVQualifiedType : public Type
{
private:
  std::shared_ptr<const Type> m_type;
  CVQualifier m_qualifier;

public:
  CVQualifiedType(CVQualifier qual, std::shared_ptr<const Type> type);

  CVQualifier cvqualifier() const { return m_qualifier; }

  bool isConst() const override;
  bool isVolatile() const override;
  bool isCVQualified() const override;

  std::shared_ptr<const Type> withoutCV() const override;
  std::string toString() const override;
};

class CXXAST_API ReferenceType : public Type
{
private:
  std::shared_ptr<const Type> m_type;
  Reference m_reference;

public:
  ReferenceType(Reference ref, std::shared_ptr<const Type> type);

  Reference reference() const { return m_reference; }

  bool isReference() const override;
  bool isLValueReference() const override;
  bool isRValueReference() const override;

  std::shared_ptr<const Type> withoutReference() const override;
  std::string toString() const override;
};

class CXXAST_API PointerType : public Type
{
private:
  std::shared_ptr<const Type> m_type;

public:
  explicit PointerType(std::shared_ptr<const Type> type);

  bool isPointer() const override;

  std::shared_ptr<const Type> withoutPointer() const override;
  std::string toString() const override;
};

} // namespace type_system

class CXXAST_API Type
{
private:
  std::shared_ptr<const type_system::Type> d;

public:
  Type();
  Type(const Type&) = default;
  Type(Type&&) = default;
  ~Type();

  explicit Type(std::shared_ptr<const type_system::Type> t);
  explicit Type(std::string name, CVQualifier cvqual = CVQualifier::None, Reference ref = Reference::None);

  static const Type Int;
  static const Type Void;
  static const Type Auto;
  static const Type DecltypeAuto;

  bool isAuto() const;
  bool isDecltypeAuto() const;
  bool isReference() const;
  bool isPointer() const;
  bool isCVQualified() const;

  Reference reference() const;
  CVQualifier cvQualification() const;

  static Type cvQualified(const Type& t, CVQualifier cvqual);
  static Type reference(const Type& t, Reference ref = Reference::LValue);
  static Type pointer(const Type& t);

  std::string toString() const;

  std::shared_ptr<const type_system::Type> impl() const;

  Type& operator=(const Type&) = default;
  Type& operator=(Type&&) = default;
};

CXXAST_API bool operator==(const Type& lhs, const Type& rhs);
inline bool operator!=(const Type& lhs, const Type& rhs) { return !(lhs == rhs); }

} // namespace cxx

namespace cxx
{

inline Type::Type(std::shared_ptr<const type_system::Type> t)
  : d(t)
{
  assert(d != nullptr);
}

} // namespace cxx

#endif // CXXAST_TYPE_H
