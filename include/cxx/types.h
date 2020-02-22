// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPES_H
#define CXXAST_TYPES_H

#include "cxx/type.h"

namespace cxx
{

class CXXAST_API SimpleType : public IType
{
private:
  std::string m_name;

public:
  explicit SimpleType(std::string name);

  bool isSimple() const override;
  const std::string& name() const override;
  std::string toString() const override;
};

class CXXAST_API AutoType : public IType
{
public:
  AutoType();

  bool isAuto() const override;
  std::string toString() const override;
};

class CXXAST_API DecltypeAutoType : public IType
{
public:
  DecltypeAutoType();

  bool isDecltypeAuto() const override;
  std::string toString() const override;
};

class CXXAST_API CVQualifiedType : public IType
{
private:
  std::shared_ptr<const IType> m_type;
  CVQualifier m_qualifier;

public:
  CVQualifiedType(CVQualifier qual, std::shared_ptr<const IType> type);

  CVQualifier cvqualifier() const { return m_qualifier; }

  bool isConst() const override;
  bool isVolatile() const override;
  bool isCVQualified() const override;

  std::shared_ptr<const IType> withoutCV() const override;
  std::string toString() const override;
};

class CXXAST_API ReferenceType : public IType
{
private:
  std::shared_ptr<const IType> m_type;
  Reference m_reference;

public:
  ReferenceType(Reference ref, std::shared_ptr<const IType> type);

  Reference reference() const { return m_reference; }

  bool isReference() const override;
  bool isLValueReference() const override;
  bool isRValueReference() const override;

  std::shared_ptr<const IType> withoutReference() const override;
  std::string toString() const override;
};

class CXXAST_API PointerType : public IType
{
private:
  std::shared_ptr<const IType> m_type;

public:
  explicit PointerType(std::shared_ptr<const IType> type);

  bool isPointer() const override;

  std::shared_ptr<const IType> withoutPointer() const override;
  std::string toString() const override;
};

class CXXAST_API FunctionType : public IType
{
private:
  std::shared_ptr<const IType> m_result_type;
  std::vector<Type> m_parameter_types;

public:
  FunctionType(std::shared_ptr<const IType> rt, std::vector<Type> pts);

  Type resultType() const;
  const std::vector<Type>& parameters() const;

  bool isFunctionType() const override;

  std::string toString() const override;
};

} // namespace cxx

namespace cxx
{

} // namespace cxx

#endif // CXXAST_TYPES_H
