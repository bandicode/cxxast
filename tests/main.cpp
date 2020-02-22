// Copyright (C) 2018 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/restricted-parser.h"
#include "cxx/parsers/libclang-parser.h"

#include <iostream>

#define ASSERT(cond) do { if(!(cond)) return 1; } while(false)

int main(int argc, char *argv[])
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

  try
  {
    cxx::parsers::LibClangParser clang_parser;
    std::cout << clang_parser.printableVersion() << std::endl;
  }
  catch (const cxx::parsers::LibClangParserError & err)
  {
    std::cout << "libclang error: " << err.what() << std::endl;
  }

  return 0;
}
