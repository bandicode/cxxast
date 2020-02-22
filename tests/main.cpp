// Copyright (C) 2018 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/translation-unit.h"

#include "cxx/parsers/restricted-parser.h"
#include "cxx/parsers/libclang-parser.h"
#include "cxx/parsers/parser.h"

#include <iostream>
#include <fstream>

#define ASSERT(cond) do { if(!(cond)) return 1; } while(false)

int test_parser()
{
  {
    std::ofstream file{ "test.cpp" };

    file << "const int foo(int& i, int j = 0) { return i+j; }" << std::endl;
  }

  cxx::parsers::Parser parser;
  std::shared_ptr<cxx::TranslationUnit> tu;

  //tu = parser.parse("test.cpp");

  //ASSERT(tu->nodes().size() == 1);
  //ASSERT(tu->nodes().front()->is<cxx::Function>());

  {
    std::ofstream file{ "toast.cpp" };

    file << "struct Foo" << std::endl;
    file << "{" << std::endl;
    file << "  int bar() const;" << std::endl;
    file << "};" << std::endl;
    file << "" << std::endl;
    file << "int Foo::bar() const { return -1 ; }" << std::endl;
  }

  //tu = parser.parse("toast.cpp");

  //ASSERT(tu->nodes().size() == 1);
  //ASSERT(tu->nodes().front()->is<cxx::Function>());

  {
    std::ofstream file{ "toast.cpp" };

    file << "#include <vector>" << std::endl;
    file << "std::vector<int> foo() { return {}; }" << std::endl;
  }

  parser.ignoreOutsideDeclarations(true);
  tu = parser.parse("toast.cpp");

  ASSERT(tu->nodes().size() == 1);
  ASSERT(tu->nodes().front()->is<cxx::Function>());

  return 0;
}

int test_restricted_parser()
{
  auto func = cxx::parsers::RestrictedParser::parseFunctionSignature("int foo(int n, int = 0);");
  ASSERT(func->returnType().toString() == "int");
  ASSERT(func->name() == "foo");
  ASSERT(func->parameters().size() == 2);

  func = cxx::parsers::RestrictedParser::parseFunctionSignature("std::vector<bool> vec_of_bool() const;");
  ASSERT(func->returnType().toString() == "std::vector<bool>");
  ASSERT(func->name() == "vec_of_bool");
  ASSERT(func->parameters().empty());
  ASSERT(func->isConst());

  return 0;
}

int main(int argc, char *argv[])
{
  int ret = 0;

  ret += test_restricted_parser();

  try
  {
    cxx::parsers::LibClangParser clang_parser;
    std::cout << clang_parser.printableVersion() << std::endl;

    ret += test_parser();
  }
  catch (const cxx::parsers::LibClangParserError & err)
  {
    std::cout << "libclang error: " << err.what() << std::endl;
    ret += 1;
  }

  return ret;
}
