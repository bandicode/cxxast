// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_BUILTIN_PARSER_H
#define CXXAST_BUILTIN_PARSER_H

#include "cxx/parsers/lexer.h"

#include "cxx/function.h"
#include "cxx/name.h"
#include "cxx/template.h"
#include "cxx/type.h"

#include <stdexcept>
#include <vector>

namespace cxx
{

namespace parsers
{

class CXXAST_API RestrictedParserError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;

  explicit RestrictedParserError(const std::string& text)
    : std::runtime_error(text.data())
  {

  }
};

class CXXAST_API RestrictedParser
{
public:

  static cxx::Type parseType(const std::string& str);
  static std::shared_ptr<Function> parseFunctionSignature(const std::string& str);

protected:
  RestrictedParser(const std::string* src);
  ~RestrictedParser() = default;

  Type parseType();
  Type tryReadFunctionSignature(const Type& result_type);

  Name parseName();

  std::shared_ptr<Function> parseFunctionSignature();

protected:
  bool atEnd() const;
  Token read();
  Token unsafe_read();
  Token peek();
  Token unsafe_peek() const;
  bool isDiscardable(const Token& t) const;
  Token read(TokenType::Value tokt);
  size_t pos() const;
  size_t pos(const Token& tok) const;
  void seek(size_t pos);
  void seekBegin();
  void seekEnd();

  std::string viewstring() const;
  std::string stringtoend() const;

protected:
  Name readOperatorName();
  Name readUserDefinedName();
  Name readTemplateArguments(const Name base);

protected:
  TemplateArgument parseDelimitedTemplateArgument();
  TemplateParameter parseDelimitedTemplateParameter();

protected:
  Function::Parameter parseFunctionParameter();

private:
  Lexer m_lexer;
  std::vector<Token> m_buffer;
  std::pair<size_t, size_t> m_view;
  size_t m_index = 0;
};

} // namespace parsers

} // namespace cxx

#endif // CXXAST_BUILTIN_PARSER_H
