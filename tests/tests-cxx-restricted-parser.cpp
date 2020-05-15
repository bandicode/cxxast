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
  REQUIRE(variable->name == "a");
  REQUIRE(variable->defaultValue() == cxx::Expression("5"));

  variable = cxx::parsers::RestrictedParser::parseVariable("inline constexpr std::string text = \"Hello World!\";");
  REQUIRE(variable->type().toString() == "std::string");
  REQUIRE(variable->name == "text");
  REQUIRE(variable->defaultValue() == cxx::Expression("\"Hello World!\""));
  REQUIRE(variable->specifiers() == (cxx::VariableSpecifier::Constexpr | cxx::VariableSpecifier::Inline));
}

TEST_CASE("The parser is able to parse simple function declarations", "[restricted-parser]")
{
  auto func = cxx::parsers::RestrictedParser::parseFunctionSignature("int foo(int n, int = 0);");
  REQUIRE(func->return_type.toString() == "int");
  REQUIRE(func->name == "foo");
  REQUIRE(func->parameters.size() == 2);

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("std::vector<bool> vec_of_bool() const;");
  REQUIRE(func->return_type.toString() == "std::vector<bool>");
  REQUIRE(func->name == "vec_of_bool");
  REQUIRE(func->parameters.empty());
  REQUIRE(func->isConst());

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("inline int max(int a, int b);");
  REQUIRE(func->return_type.toString() == "int");
  REQUIRE(func->name == "max");
  REQUIRE(func->parameters.size() == 2);
  REQUIRE(func->isInline());

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("virtual void draw() = 0;");
  REQUIRE(func->return_type.toString() == "void");
  REQUIRE(func->name == "draw");
  REQUIRE(func->parameters.empty());
  REQUIRE(func->isVirtual());
  REQUIRE(func->isVirtualPure());

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("void draw() override;");
  REQUIRE(func->return_type.toString() == "void");
  REQUIRE(func->name == "draw");
  REQUIRE(func->parameters.empty());
  REQUIRE(func->isOverride());

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("void draw() final;");
  REQUIRE(func->return_type.toString() == "void");
  REQUIRE(func->name == "draw");
  REQUIRE(func->parameters.empty());
  REQUIRE(func->isFinal());
}
