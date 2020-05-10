// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "cxx/translation-unit.h"

#include "cxx/parsers/libclang-parser.h"
#include "cxx/parsers/parser.h"

#include "cxx/class.h"

#include <iostream>
#include <fstream>

static void write_file(const char* filename, const char* content)
{
  std::ofstream file{ filename };
  file << content << std::endl;
}

static bool findLibclangParser()
{
  try
  {
    cxx::parsers::LibClangParser clang_parser;
    std::cout << "libclang parser available, version = " << clang_parser.printableVersion() << std::endl;
    return true;
  }
  catch (const cxx::parsers::LibClangParserError & err)
  {
    std::cout << "libclang error: " << err.what() << std::endl;
    std::cout << "libclang parser tests will be skipped..." << std::endl;
  }

  return false;
}

static bool skipTest()
{
  static bool value = findLibclangParser();
  return value;
}

TEST_CASE("The parser is able to parse a function", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "const int foo(int& i, int j = 0) { return i+j; }");

  cxx::parsers::Parser parser;
  std::shared_ptr<cxx::TranslationUnit> tu;

  tu = parser.parse("test.cpp");

  REQUIRE(tu->nodes().size() == 1);
  REQUIRE(tu->nodes().front()->is<cxx::Function>());
}

TEST_CASE("The parser is able to parse a struct", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("toast.cpp",
    "struct Foo\n"
    "{\n"
    "  int bar() const;\n"
    "};\n"
    "\n"
    "int Foo::bar() const { return -1 ; }\n");

  cxx::parsers::Parser parser;
  std::shared_ptr<cxx::TranslationUnit> tu;

  tu = parser.parse("toast.cpp");

  REQUIRE(tu->nodes().size() == 1);
  REQUIRE(tu->nodes().front()->is<cxx::Class>());
}

TEST_CASE("The parser handles #include <vector>", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("toast.cpp",
    "#include <vector>\n"
    "std::vector<int> foo() { return {}; }\n");

  cxx::parsers::Parser parser;
  std::shared_ptr<cxx::TranslationUnit> tu;

  parser.ignoreOutsideDeclarations(true);
  tu = parser.parse("toast.cpp");

  REQUIRE(tu->nodes().size() == 1);
  REQUIRE(tu->nodes().front()->is<cxx::Function>());
}
