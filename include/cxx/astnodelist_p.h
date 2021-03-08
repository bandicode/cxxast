// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ASTNODELIST_P_H
#define CXXAST_ASTNODELIST_P_H

#include "cxx/node.h"

namespace cxx
{

class CXXAST_API AstEmptyNodeList : public AstNodeListInterface
{
public:
  using AstNodeListInterface::AstNodeListInterface;

  size_t size() const override;
  std::shared_ptr<AstNode> at(size_t index) const override;
};

class CXXAST_API AstVectorNodeList : public AstNodeListInterface
{
public:
  explicit AstVectorNodeList(std::vector<std::shared_ptr<AstNode>> vec);

  size_t size() const override;
  std::shared_ptr<AstNode> at(size_t index) const override;

private:
  std::vector<std::shared_ptr<AstNode>> m_vec;
};

class CXXAST_API AstVectorRefNodeList : public AstNodeListInterface
{
public:
  explicit AstVectorRefNodeList(const std::vector<std::shared_ptr<AstNode>>& vec);

  size_t size() const override;
  std::shared_ptr<AstNode> at(size_t index) const override;

private:
  const std::vector<std::shared_ptr<AstNode>>& m_vec;
};

} // namespace cxx

#endif // CXXAST_ASTNODELIST_P_H
