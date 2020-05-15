// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_MACRO_H
#define CXXAST_MACRO_H

#include "cxx/entity.h"

#include "cxx/type.h"

namespace cxx
{

class CXXAST_API Macro : public Entity
{
public:
  std::vector<std::string> parameters;

public:
  Macro(std::string name, std::vector<std::string> params, std::shared_ptr<Entity> parent = nullptr);

  static constexpr NodeKind ClassNodeKind = NodeKind::Macro;
  NodeKind node_kind() const override;
};

} // namespace cxx

namespace cxx
{

inline Macro::Macro(std::string name, std::vector<std::string> params, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) },
    parameters(std::move(params))
{

}

} // namespace cxx

#endif // CXXAST_MACRO_H
