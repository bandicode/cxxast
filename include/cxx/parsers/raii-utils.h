// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_RAIIUTILS_H
#define CXXAST_RAIIUTILS_H

#include <memory>
#include <vector>

namespace cxx
{

namespace parsers
{

template<typename T>
struct RAIIVectorSharedGuard
{
  std::vector<std::shared_ptr<T>>& target_;

  RAIIVectorSharedGuard(std::vector<std::shared_ptr<T>>& target, std::shared_ptr<T> elem)
    : target_(target)
  {
    target_.push_back(elem);
  }

  ~RAIIVectorSharedGuard()
  {
    target_.pop_back();
  }
};

template<typename T>
struct RAIIGuard
{
  T& target_;
  T value_;

  RAIIGuard(T& target)
    : target_(target), value_(target)
  {

  }

  ~RAIIGuard()
  {
    target_ = value_;
  }
};

} // namespace parsers

} // namespace cxx

#endif // CXXAST_RAIIUTILS_H
