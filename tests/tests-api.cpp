// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "catch.hpp"

#include "cxx/while-loop.h"

TEST_CASE("The Handle class can hold a WhileLoop", "[api]")
{
  auto w = std::make_shared<cxx::WhileLoop>();
  w->condition = cxx::Expression("true");

  cxx::Handle<cxx::INode> handle{ w };

  REQUIRE(handle.kind() == cxx::NodeKind::WhileLoop);
  REQUIRE(handle.is<cxx::WhileLoop>());

  cxx::Expression& expr = handle.get<cxx::WhileLoop::Condition>();

  REQUIRE(expr.toString() == "true");

  expr = cxx::Expression("false");

  REQUIRE(w->condition.toString() == "false");
}
