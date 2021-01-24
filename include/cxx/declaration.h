// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DECLARATION_H
#define CXXAST_DECLARATION_H

#include "cxx/statement.h"

namespace cxx
{

class Entity;

class CXXAST_API Declaration : public Statement
{
public:
  std::vector<std::shared_ptr<Declaration>> declarations; // @TODO: remove this ?

public:
  Declaration() = default;

  std::shared_ptr<Declaration> shared_from_this();

  bool isDeclaration() const override;

  virtual std::shared_ptr<Entity> entity() const;
};

class CXXAST_API AstDeclaration : public AstStatement
{
public:
  AstDeclaration() = default;
  
  AstDeclaration(const std::shared_ptr<Declaration>& decl)
  {
    node_ptr = decl;
  }

  AstDeclaration(const std::shared_ptr<Entity>& ent);

  NodeKind node_kind() const override { return NodeKind::AstDeclaration; }
};

} // namespace cxx

#endif // CXXAST_DECLARATION_H
