// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "cxx/parsers/restricted-parser.h"

TEST_CASE("The parser is able to parse simple types", "[restricted-parser]")
{
  cxx::Type t = cxx::parsers::RestrictedParser::parseType("const int*");
  REQUIRE(t.isPointer());
  REQUIRE(t.pointee().cvQualification() == cxx::CVQualifier::Const);

  t = cxx::parsers::RestrictedParser::parseType("void(int,char)");
  REQUIRE(t.isFunction());
  REQUIRE(t.resultType() == cxx::Type::Void);
  REQUIRE(t.parameters().size() == 2);
}

TEST_CASE("The parser is able to parse simple variable declarations", "[restricted-parser]")
{
  auto variable = cxx::parsers::RestrictedParser::parseVariable("int a = 5;");
  REQUIRE(variable->type().toString() == "int");
  REQUIRE(variable->name() == "a");
  REQUIRE(variable->defaultValue() == "5");

  variable = cxx::parsers::RestrictedParser::parseVariable("inline constexpr std::string text = \"Hello World!\";");
  REQUIRE(variable->type().toString() == "std::string");
  REQUIRE(variable->name() == "text");
  REQUIRE(variable->defaultValue() == "\"Hello World!\"");
  REQUIRE(variable->specifiers() == (cxx::VariableSpecifier::Constexpr | cxx::VariableSpecifier::Inline));
}

TEST_CASE("The parser is able to parse simple function declarations", "[restricted-parser]")
{
  auto func = cxx::parsers::RestrictedParser::parseFunctionSignature("int foo(int n, int = 0);");
  REQUIRE(func->returnType().toString() == "int");
  REQUIRE(func->name() == "foo");
  REQUIRE(func->parameters().size() == 2);

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("std::vector<bool> vec_of_bool() const;");
  REQUIRE(func->returnType().toString() == "std::vector<bool>");
  REQUIRE(func->name() == "vec_of_bool");
  REQUIRE(func->parameters().empty());
  REQUIRE(func->isConst());
}
