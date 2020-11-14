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

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("inline int max(int a, int b) noexcept;");
  REQUIRE(func->return_type.toString() == "int");
  REQUIRE(func->name == "max");
  REQUIRE(func->parameters.size() == 2);
  REQUIRE(func->isInline());
  REQUIRE(func->isNoexcept());

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

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("std::vector<std::shared_ptr<Foo>> bar() const;");
  REQUIRE(func->return_type.toString() == "std::vector<std::shared_ptr<Foo>>");
  REQUIRE(func->name == "bar");
  REQUIRE(func->parameters.empty());
  REQUIRE(func->isConst());

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("static int foo(vector<shared_ptr<std::variant<bool, float, int>>> a, const float & b);");
  REQUIRE(func->return_type.toString() == "int");
  REQUIRE(func->name == "foo");
  REQUIRE(func->parameters.size() == 2);
  REQUIRE(func->parameters.at(0)->type.toString() == "vector<shared_ptr<std::variant<bool, float, int>>>");
  REQUIRE(func->parameters.at(1)->type.toString() == "const float&");
  REQUIRE(func->isStatic());
}

TEST_CASE("The parser is able to parse simple typedef declarations", "[restricted-parser]")
{
  auto def = cxx::parsers::RestrictedParser::parseTypedef("typedef const int ConstInt;");
  REQUIRE(def->name == "ConstInt");
  REQUIRE(def->type.toString() == "const int");

  def = cxx::parsers::RestrictedParser::parseTypedef("typedef std::vector<bool> vec_of_bool;");
  REQUIRE(def->name == "vec_of_bool");
  REQUIRE(def->type.toString() == "std::vector<bool>");

  // The following is not a valid typedef, but that will do for now...
  def = cxx::parsers::RestrictedParser::parseTypedef("typedef int(void) func_int;");
  REQUIRE(def->name == "func_int");
  REQUIRE(def->type.toString() == "int(void)");
}

TEST_CASE("The parser is able to parse simple macro declarations", "[restricted-parser]")
{
  auto def = cxx::parsers::RestrictedParser::parseMacro("__cpp__");
  REQUIRE(def->name == "__cpp__");
  REQUIRE(def->parameters.empty());

  def = cxx::parsers::RestrictedParser::parseMacro("EXPAND(X)");
  REQUIRE(def->name == "EXPAND");
  REQUIRE(def->parameters.size() == 1);
  REQUIRE(def->parameters.front() == "X");

  def = cxx::parsers::RestrictedParser::parseMacro("EXPAND(X, ...)");
  REQUIRE(def->name == "EXPAND");
  REQUIRE(def->parameters.size() == 2);
  REQUIRE(def->parameters.front() == "X");
  REQUIRE(def->parameters.back() == "...");
}
