// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ASTNODELIST_P_H
#define CXXAST_ASTNODELIST_P_H

#include "cxx/node.h"

#include <array>

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

template<size_t N>
class AstArrayNodeList : public AstNodeListInterface
{
public:

  AstArrayNodeList() = default;

  size_t size() const
  {
    return m_size;
  }

  std::shared_ptr<AstNode> at(size_t index) const
  {
    return m_array.at(index);
  }

public:
  std::array<std::shared_ptr<AstNode>, N> m_array;
  size_t m_size = 0;
};

namespace priv
{

template<size_t N>
void fill_node_list(AstArrayNodeList<N>&)
{

}

template<size_t N, typename T, typename...Args>
void fill_node_list(AstArrayNodeList<N>& list, T&& val, Args&&... args)
{
  if (!cxx::is_null(val))
    list.m_array[list.m_size++] = cxx::to_ast_node(val);

  fill_node_list(list, std::forward<Args>(args)...);
}

} // namespace priv

template<typename...Args>
AstNodeList make_node_list(Args&&... args)
{
  auto result = std::make_shared<AstArrayNodeList<sizeof...(Args)>>();
  priv::fill_node_list(*result, std::forward<Args>(args)...);
  return AstNodeList{ result };
}

} // namespace cxx

#endif // CXXAST_ASTNODELIST_P_H
