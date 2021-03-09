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
  std::shared_ptr<IEntity> entity_ptr;

public:
  IDeclaration() = default;
  explicit IDeclaration(std::shared_ptr<IEntity> e);

  std::shared_ptr<IDeclaration> shared_from_this();

  bool isDeclaration() const override;
};

} // namespace cxx

#endif // CXXAST_DECLARATION_H
