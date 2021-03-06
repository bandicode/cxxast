// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "cxx/parsers/parser.h"

#include "cxx/program.h"

#include "cxx/class.h"
#include "cxx/namespace.h"
#include "cxx/statements.h"
#include "cxx/variable.h"

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
    cxx::LibClang clang_parser;
    std::cout << "libclang parser available, version = " << clang_parser.printableVersion() << std::endl;
    return true;
  }
  catch (const cxx::LibClangError & err)
  {
    std::cout << "libclang error: " << err.what() << std::endl;
    std::cout << "libclang parser tests will be skipped..." << std::endl;
  }

  return false;
}

static bool skipTest()
{
  static bool value = findLibclangParser();
  return !value;
}

TEST_CASE("The parser is able to parse a function", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "const int foo(int& i, int j = 0) { return i+j; }");

  cxx::parsers::LibClangParser parser;
  parser.skip_function_bodies = true;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.isNull());
}

TEST_CASE("The parser is able to parse a simple function body", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "void foo(int n) noexcept {  if(n > 0){} }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  auto& foo = static_cast<cxx::Function&>(*(prog->globalNamespace()->entities.front()));

  REQUIRE(foo.isNoexcept());

  REQUIRE(foo.body.is<cxx::CompoundStatement>());
  REQUIRE(cxx::to_ast_node(foo.body)->children().size() == 1);

  {
    auto& statements = foo.body.get<cxx::CompoundStatement::Statements>();

    REQUIRE(statements.size() == 1);

    REQUIRE(statements.front().is<cxx::IfStatement>());
    
    {
      cxx::Statement if_stmt = statements.front();

      REQUIRE(cxx::to_ast_node(if_stmt)->children().size() == 2);

      REQUIRE(if_stmt.get<cxx::IfStatement::Condition>().toString() == "n > 0");
      REQUIRE(if_stmt.get<cxx::IfStatement::Body>().is<cxx::CompoundStatement>());
    }
  }
}

TEST_CASE("The parser is able to parse a return statement", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "int foo() { return 0; }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().size() == 1);
  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().at(0).is<cxx::ReturnStatement>());
}

TEST_CASE("The parser is able to parse a break & continue statements", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "void foo() { while(true) break; for(int n = 0; n < 5; ++n) continue; }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().size() == 2);

  {
    cxx::Statement stmt = fn->body.get<cxx::CompoundStatement::Statements>().at(0);
    REQUIRE(stmt.is<cxx::WhileLoop>());
    REQUIRE(stmt.get<cxx::WhileLoop::Body>().is<cxx::BreakStatement>());
  }

  {
    cxx::Statement stmt = fn->body.get<cxx::CompoundStatement::Statements>().at(1);
    REQUIRE(stmt.is<cxx::ForLoop>());
    REQUIRE(stmt.get<cxx::ForLoop::Condition>().toString() == "n < 5");
    REQUIRE(stmt.get<cxx::ForLoop::Iter>().toString() == "++n");
    REQUIRE(stmt.get<cxx::ForLoop::Body>().is<cxx::ContinueStatement>());
  }
}

TEST_CASE("The parser is able to parse a switch statement", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "void foo(int n) { switch(n) { case 1: break; default: return; } }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().size() == 1);

  cxx::Statement stmt = fn->body.get<cxx::CompoundStatement::Statements>().at(0);
  REQUIRE(stmt.is<cxx::SwitchStatement>());

  stmt = stmt.get<cxx::SwitchStatement::Body>();
  REQUIRE(stmt.is<cxx::CompoundStatement>());
  REQUIRE(stmt.get<cxx::CompoundStatement::Statements>().size() == 2);
  REQUIRE(stmt.get<cxx::CompoundStatement::Statements>().at(0).is<cxx::CaseStatement>());
  REQUIRE(stmt.get<cxx::CompoundStatement::Statements>().at(1).is<cxx::DefaultStatement>());
}


TEST_CASE("The parser is able to parse a do-while loop", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "void foo(int n) { do { --n; } while(n > 0); }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().size() == 1);

  cxx::Statement stmt = fn->body.get<cxx::CompoundStatement::Statements>().at(0);
  REQUIRE(stmt.is<cxx::DoWhileLoop>());

  REQUIRE(stmt.get<cxx::DoWhileLoop::Condition>().toString() == "n > 0");
}

TEST_CASE("The parser is able to parse a try-catch", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "void foo() { try { throw 5; } catch(int n) { } }");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Function>());

  std::shared_ptr<cxx::Function> fn = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.front());

  REQUIRE(fn->body.get<cxx::CompoundStatement::Statements>().size() == 1);

  cxx::Statement stmt = fn->body.get<cxx::CompoundStatement::Statements>().at(0);
  REQUIRE(stmt.is<cxx::TryBlock>());

  REQUIRE(stmt.get<cxx::TryBlock::Handlers>().size() == 1);
}


TEST_CASE("The parser is able to parse a struct", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("toast.cpp",
    "class Foo\n"
    "{\n"
    "public:"
    "  int n = 0;\n"
    "  static int m = 1;\n"
    "  int bar() const;\n"
    "};\n"
    "\n"
    "int Foo::bar() const { return -1 ; }\n");

  cxx::parsers::LibClangParser parser;
  parser.skip_function_bodies = true;

  bool result = parser.parse("toast.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Class>());

  auto& Foo = static_cast<cxx::Class&>(*(prog->globalNamespace()->entities.front()));

  REQUIRE(Foo.name == "Foo");

  REQUIRE(Foo.members.size() == 3);

  REQUIRE(Foo.members.at(0)->is<cxx::Variable>());

  {
    auto& n = static_cast<cxx::Variable&>(*Foo.members.at(0));

    REQUIRE(n.name == "n");
    REQUIRE(n.type().toString() == "int");
    REQUIRE(n.defaultValue().toString() == "0");
  }

  REQUIRE(Foo.members.at(1)->is<cxx::Variable>());

  {
    auto& m = static_cast<cxx::Variable&>(*Foo.members.at(1));

    REQUIRE(m.name == "m");
    REQUIRE(m.type().toString() == "int");
    REQUIRE(m.defaultValue().toString() == "1");
    REQUIRE(bool(m.specifiers() & cxx::VariableSpecifier::Static));
  }

  REQUIRE(Foo.members.at(2)->is<cxx::Function>());

  {
    auto& bar = static_cast<cxx::Function&>(*Foo.members.at(2));

    REQUIRE(bar.name == "bar");
    REQUIRE(bar.access_specifier == cxx::AccessSpecifier::PUBLIC);
  }
}

TEST_CASE("The parser handles #include <vector>", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("toast.cpp",
    "#include <vector>\n"
    "std::vector<int> foo() { return {}; }\n");

  cxx::parsers::LibClangParser parser;
  parser.skip_function_bodies = true;

  bool result = parser.parse("toast.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.back()->is<cxx::Function>());

  auto foo = std::static_pointer_cast<cxx::Function>(prog->globalNamespace()->entities.back());
  REQUIRE(foo->name == "foo");
}

TEST_CASE("The parser is able to parse a simple class template", "[libclang-parser]")
{
  if (skipTest())
    return;

  write_file("test.cpp",
    "template<typename T> struct Foo { T value; }; ");

  cxx::parsers::LibClangParser parser;

  bool result = parser.parse("test.cpp");

  REQUIRE(result);

  auto prog = parser.program();

  REQUIRE(prog->globalNamespace()->entities.size() == 1);
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::Class>());
  REQUIRE(prog->globalNamespace()->entities.front()->is<cxx::ClassTemplate>());

  auto& Foo = static_cast<cxx::ClassTemplate&>(*(prog->globalNamespace()->entities.front()));

  REQUIRE(Foo.templateParameters().size() == 1);

  REQUIRE(Foo.templateParameters().front()->name == "T");

  REQUIRE(Foo.members.size() == 1);

  REQUIRE(Foo.members.front()->is<cxx::Variable>());
  REQUIRE(Foo.members.front()->name == "value");
  REQUIRE(std::static_pointer_cast<cxx::Variable>(Foo.members.front())->type().toString() == "T");
}