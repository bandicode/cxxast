// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLASS_DECLARATION_H
#define CXXAST_CLASS_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/class.h"

namespace cxx
{

// for class decl inside a function
class CXXAST_API ClassDeclaration : public Declaration
{
public:
  std::shared_ptr<Class> class_;

public:
  ClassDeclaration() = default;

  explicit ClassDeclaration(std::shared_ptr<Class> c);

  static constexpr NodeKind ClassNodeKind = NodeKind::ClassDeclaration;
  NodeKind node_kind() const override;

  std::shared_ptr<Entity> entity() const override;

  // bool isForwardDeclaration() const;
};

} // namespace cxx

namespace cxx
{

inline ClassDeclaration::ClassDeclaration(std::shared_ptr<Class> c)
  : class_(c)
{

}

} // namespace cxx

#endif // CXXAST_CLASS_DECLARATION_H
