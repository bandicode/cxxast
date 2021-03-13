// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/parser.h"
#include "cxx/declaration.h"
#include "cxx/filesystem.h"

#include <iostream>

using namespace cxx;

int depth = 0;

void visit(const std::shared_ptr<cxx::AstNode>& node)
{
  std::cout << std::string(static_cast<size_t>(2 * depth), ' ');


  std::cout << node->sourcerange.begin.line << ":" << node->sourcerange.begin.column;
  std::cout << " --> ";
  std::cout << node->sourcerange.end.line << ":" << node->sourcerange.end.column;

  if (node->isDeclaration())
  {
    auto decl = std::static_pointer_cast<cxx::IDeclaration>(node);

    if (decl->entity_ptr)
      std::cout << " " << decl->entity_ptr->name;

  }

  if(dynamic_cast<cxx::UnexposedAstNode*>(node.get()))
    std::cout << " [" << cxx::to_string(static_cast<cxx::UnexposedAstNode&>(*node).kind) << "]";
  else if (dynamic_cast<cxx::UnexposedStatement*>(node.get()))
    std::cout << " [" << cxx::to_string(static_cast<cxx::UnexposedStatement&>(*node).kind) << "]";
  else
    std::cout << " [" << cxx::to_string(node->kind()) << "]";

  std::cout << std::endl;

  {
    ++depth;

    for (auto n : node->children())
      visit(n);

    --depth;
  }
}

void work(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "argc must be >= 2" << std::endl;
    return;
  }

  parsers::LibClangParser parser;

  bool result = parser.parse(argv[1]);

  if (!result)
  {
    std::cerr << "failed to parse " << argv[1];
    return;
  }

  auto file = cxx::FileSystem::GlobalInstance().get(argv[1]);
  visit(file->ast);
}

int main(int argc, char *argv[])
{
  try
  {
    work(argc, argv);
  }
  catch (cxx::LibClangError & )
  {
    std::cerr << "could not find libclang" << std::endl;
    return 1;
  }
  catch (std::runtime_error & err)
  {
    std::cerr << err.what() << std::endl;
    return 2;
  }

  return 0;
}