// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAME_P_H
#define CXXAST_NAME_P_H

#include "cxx/name.h"
#include "cxx/template.h"
#include "cxx/type.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace cxx
{

namespace details
{

class CXXAST_API Identifier : public IName
{
private:
  std::string m_name;

public:
  explicit Identifier(std::string name);

  bool isIdentifier() const override;
  std::string toString() const override;
};

class CXXAST_API OverloadedOperatorName : public IName
{
private:
  std::string m_op;

public:
  explicit OverloadedOperatorName(std::string opid);

  bool isOperatorName() const override;
  std::string toString() const override;
};

class CXXAST_API ConversionFunctionName : public IName
{
public:
  Type m_type;

public:
  explicit ConversionFunctionName(const Type& t);

  bool isConversionFunctionName() const override;
  std::string toString() const override;
};

class CXXAST_API LiteralOperatorName : public IName
{
private:
  std::string m_suffix;

public:
  explicit LiteralOperatorName(std::string suffix);

  bool isLiteralOperatorName() const override;

  std::string toString() const override;
};

class CXXAST_API TemplateName : public IName
{
private:
  std::string m_name;
  std::vector<TemplateArgument> m_targs;

public:
  explicit TemplateName(std::string name, std::vector<TemplateArgument> targ = {});

  bool isTemplateName() const override;

  std::string toString() const override;
};

class CXXAST_API DestructorName : public IName
{
private:
  std::shared_ptr<const IName> m_name;

public:
  explicit DestructorName(std::shared_ptr<const IName> type);

  bool isDestructorName() const override;

  std::string toString() const override;
};

class CXXAST_API QualifiedName : public IName
{
private:
  std::shared_ptr<const IName> m_lhs;
  std::shared_ptr<const IName> m_rhs;

public:
  explicit QualifiedName(std::shared_ptr<const IName> lhs, std::shared_ptr<const IName> rhs);

  template<typename It>
  static std::shared_ptr<QualifiedName> make(It begin, It end)
  {
    auto d = std::distance(begin, end);

    if (d == 2)
      return std::make_shared<QualifiedName>(*begin, *(begin + 1));

    auto lhs = make(begin, begin + (d - 1));
    return std::make_shared<QualifiedName>(lhs, *(begin + (d - 1)));
  }

  bool isQualifiedName() const override;

  std::string toString() const override;
};

} // namespace details

} // namespace cxx

#endif // CXXAST_NAME_P_H
