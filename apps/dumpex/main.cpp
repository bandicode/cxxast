// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/parser.h"
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

  if (node->node_ptr && node->node_ptr->isEntity())
    std::cout << " " << static_cast<cxx::IEntity*>(node->node_ptr.get())->name;

  std::cout << " [" << cxx::to_string(node->kind) << "]";

  std::cout << std::endl;

  {
    ++depth;

    for (auto n : node->children_vec)
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