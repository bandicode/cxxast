// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DECLARATION_H
#define CXXAST_DECLARATION_H

#include "cxx/statement.h"

namespace cxx
{

class IEntity;

class CXXAST_API IDeclaration : public IStatement
{
public:
  IDeclaration() = default;

  std::shared_ptr<IDeclaration> shared_from_this();

  bool isDeclaration() const override;

  virtual std::shared_ptr<IEntity> entity() const;
};

class CXXAST_API AstDeclaration : public AstStatement
{
public:
  AstDeclaration() = default;
  
  AstDeclaration(const std::shared_ptr<IDeclaration>& decl)
  {
    node_ptr = decl;
  }

  AstDeclaration(const std::shared_ptr<IEntity>& ent);

  NodeKind node_kind() const override { return NodeKind::AstDeclaration; }
};

} // namespace cxx

#endif // CXXAST_DECLARATION_H
