// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAME_H
#define CXXAST_NAME_H

#include "cxx/cxxast-defs.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace cxx
{

namespace details
{

class CXXAST_API IName : public std::enable_shared_from_this<IName>
{
public:
  IName() = default;
  virtual ~IName();

  virtual bool isIdentifier() const;
  virtual bool isOperatorName() const;
  virtual bool isConversionFunctionName() const;
  virtual bool isLiteralOperatorName() const;
  virtual bool isTemplateName() const;
  virtual bool isDestructorName() const;
  virtual bool isQualifiedName() const;

  virtual std::string toString() const = 0;
};

} // namespace details

class CXXAST_API Name
{
private:
  std::shared_ptr<const details::IName> d;

public:
  Name();
  Name(const Name&) = default;
  Name(Name&&) = default;
  ~Name();

  explicit Name(std::shared_ptr<const details::IName> n);

  static const Name Int;

  bool isIdentifier() const;
  bool isOperatorName() const;
  bool isConversionFunctionName() const;
  bool isLiteralOperatorName() const;
  bool isTemplateName() const;
  bool isDestructorName() const;
  bool isQualifiedName() const;

  std::string toString() const;

  static Name fromSimpleIdentifier(std::string name);

  std::shared_ptr<const details::IName> impl() const;

  Name& operator=(const Name&) = default;
  Name& operator=(Name&&) = default;
};

} // namespace cxx

namespace cxx
{
} // namespace cxx

#endif // CXXAST_NAME_H
