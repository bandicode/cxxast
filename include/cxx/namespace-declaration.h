// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAMESPACE_DECLARATION_H
#define CXXAST_NAMESPACE_DECLARATION_H

#include "cxx/declaration.h"
#include "cxx/namespace.h"

namespace cxx
{

class CXXAST_API NamespaceDeclaration : public IDeclaration
{
public:
  std::shared_ptr<Namespace> namespace_;

public:
  NamespaceDeclaration() = default;

  explicit NamespaceDeclaration(std::shared_ptr<Namespace> ns);

  static constexpr NodeKind ClassNodeKind = NodeKind::NamespaceDeclaration;
  NodeKind node_kind() const override;

};

} // namespace cxx

namespace cxx
{

inline NamespaceDeclaration::NamespaceDeclaration(std::shared_ptr<Namespace> ns)
  : IDeclaration(ns),
    namespace_(ns)
{

}

} // namespace cxx

#endif // CXXAST_NAMESPACE_DECLARATION_H
