// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TRANSLATION_UNIT_H
#define CXXAST_TRANSLATION_UNIT_H

#include "cxx/node.h"

#include <vector>

namespace cxx
{

class CXXAST_API TranslationUnit : public Node
{
public:
  TranslationUnit(std::shared_ptr<File> file);
  ~TranslationUnit();

  static const std::string TypeId;
  const std::string& type() const override;

  const std::shared_ptr<File>& file() const;

  std::vector<std::shared_ptr<File>>& includes();
  const std::vector<std::shared_ptr<File>>& includes() const;

  std::vector<std::shared_ptr<Node>>& nodes();
  const std::vector<std::shared_ptr<Node>>& nodes() const;

private:
  std::shared_ptr<File> m_file;
  std::vector<std::shared_ptr<File>> m_includes;
  std::vector<std::shared_ptr<Node>> m_nodes;
};

} // namespace cxx

namespace cxx
{

inline const std::shared_ptr<File>& TranslationUnit::file() const
{
  return m_file;
}

inline std::vector<std::shared_ptr<File>>& TranslationUnit::includes()
{
  return m_includes;
}

inline const std::vector<std::shared_ptr<File>>& TranslationUnit::includes() const
{
  return m_includes;
}

inline std::vector<std::shared_ptr<Node>>& TranslationUnit::nodes()
{
  return m_nodes;
}

inline const std::vector<std::shared_ptr<Node>>& TranslationUnit::nodes() const
{
  return m_nodes;
}

} // namespace cxx

#endif // CXXAST_TRANSLATION_UNIT_H
