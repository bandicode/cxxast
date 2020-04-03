// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/translation-unit.h"


namespace cxx
{

TranslationUnit::TranslationUnit(std::shared_ptr<File> file)
  : Node(),
  m_file(file)
{

}

TranslationUnit::~TranslationUnit()
{

}

NodeKind TranslationUnit::node_kind() const
{
  return ClassNodeKind;
}

} // namespace cxx
