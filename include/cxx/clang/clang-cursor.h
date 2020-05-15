// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLANG_CURSOR_H
#define CXXAST_CLANG_CURSOR_H

#include "cxx/libclang.h"

#include <functional>

namespace cxx
{

class CXXAST_API ClangCursor
{
public:
  LibClang* libclang;
  CXCursor cursor;

  ClangCursor(LibClang& lib, CXCursor c)
    : libclang(&lib), cursor(c)
  {

  }

  ClangCursor(const ClangCursor&) = default;

  ~ClangCursor()
  {

  }

  ClangCursor& operator=(const ClangCursor&) = default;

  operator CXCursor() const
  {
    return cursor;
  }

  CXCursorKind kind() const
  {
    return libclang->clang_getCursorKind(this->cursor);
  }

  bool isDeclaration() const
  {
    return libclang->clang_isDeclaration(kind());
  }

  bool isExpression() const
  {
    return libclang->clang_isExpression(kind());
  }

  bool isPreprocessing() const
  {
    return libclang->clang_isPreprocessing(kind());
  }

  bool isReference() const
  {
    return libclang->clang_isReference(kind());
  }

  bool isStatement() const
  {
    return libclang->clang_isStatement(kind());
  }

  bool isUnexposed() const
  {
    return libclang->clang_isUnexposed(kind());
  }

  std::string getSpelling() const
  {
    CXString str = libclang->clang_getCursorSpelling(this->cursor);
    std::string result = libclang->clang_getCString(str);
    libclang->clang_disposeString(str);
    return result;
  }

  std::string getCursorKingSpelling() const
  {
    CXString str = libclang->clang_getCursorKindSpelling(kind());
    std::string result = libclang->clang_getCString(str);
    libclang->clang_disposeString(str);
    return result;
  }

  ClangCursor getLexicalParent() const
  {
    CXCursor c = libclang->clang_getCursorLexicalParent(this->cursor);
    return ClangCursor{ *libclang, c };
  }

  ClangCursor getSemanticParent() const
  {
    CXCursor c = libclang->clang_getCursorSemanticParent(this->cursor);
    return ClangCursor{ *libclang, c };
  }

  template<typename T>
  struct VisitorData
  {
    LibClang& libclang;
    T& functor;
  };

  template<typename T>
  static CXChildVisitResult generic_visitor(CXCursor c, CXCursor p, CXClientData client_data)
  {
    VisitorData<T>& data = *static_cast<VisitorData<T>*>(client_data);
    data.functor(ClangCursor{ data.libclang, c });
    return CXChildVisit_Continue;
  }

  template<typename Func>
  void visitChildren(Func&& f) const
  {
    VisitorData<Func> data{ *libclang, f };
    libclang->clang_visitChildren(this->cursor, generic_visitor<Func>, &data);
  }

  size_t childCount() const;
};

inline bool operator==(const ClangCursor& lhs, const ClangCursor& rhs)
{
  return lhs.libclang->clang_equalCursors(lhs.cursor, rhs.cursor);
}

inline bool operator!=(const ClangCursor& lhs, const ClangCursor& rhs)
{
  return !(lhs == rhs);
}

} // namespace cxx

namespace std
{
template<> struct hash<cxx::ClangCursor>
{
  std::size_t operator()(const cxx::ClangCursor& c) const noexcept
  {
    return c.libclang != nullptr ? c.libclang->clang_hashCursor(c.cursor) : 0;
  }
};
}

#endif // CXXAST_CLANG_CURSOR_H
