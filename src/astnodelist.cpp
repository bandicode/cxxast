// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/astnodelist_p.h"

#include <stdexcept>

namespace cxx
{

size_t AstEmptyNodeList::size() const
{
  return 0;
}

std::shared_ptr<AstNode> AstEmptyNodeList::at(size_t /* index */) const
{
  return nullptr;
}


AstVectorNodeList::AstVectorNodeList(std::vector<std::shared_ptr<AstNode>> vec)
  : m_vec(std::move(vec))
{

}

size_t AstVectorNodeList::size() const
{
  return m_vec.size();
}

std::shared_ptr<AstNode> AstVectorNodeList::at(size_t index) const
{
  return m_vec.at(index);
}


AstVectorRefNodeList::AstVectorRefNodeList(const std::vector<std::shared_ptr<AstNode>>& vec)
  : m_vec(vec)
{

}

size_t AstVectorRefNodeList::size() const
{
  return m_vec.size();
}

std::shared_ptr<AstNode> AstVectorRefNodeList::at(size_t index) const
{
  return m_vec.at(index);
}

} // namespace cxx