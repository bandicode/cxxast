// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_PARSERS_PARSER_H
#define CXXAST_PARSERS_PARSER_H

#include "cxx/clang/clang-cursor.h"
#include "cxx/clang/clang-index.h"
#include "cxx/clang/clang-translation-unit.h"

#include <cxx/access-specifier.h>
#include "cxx/function.h"

#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace cxx
{

class Enum;
class Namespace;
class Type;

class FileSystem;
class Program;

namespace parsers
{

class CXXAST_API LibClangParser : protected LibClang
{
public:
  std::set<std::string> includedirs;
  std::map<std::string, std::string> defines;

public:
  LibClangParser();
  ~LibClangParser();

  explicit LibClangParser(cxx::FileSystem& fs);
  explicit LibClangParser(std::shared_ptr<Program> prog);
  LibClangParser(std::shared_ptr<Program> prog, cxx::FileSystem& fs);

  std::shared_ptr<Program> program() const;

  bool parse(const std::string& file);

  static cxx::AccessSpecifier getAccessSpecifier(CX_CXXAccessSpecifier as);

protected:
  std::shared_ptr<File> getFile(const std::string& path);
  cxx::Node& curNode();

  /* Visitor callbacks */
  void visit(const ClangCursor& cursor);
  void visit_tu(const ClangCursor& cursor);

  void visit_namespace(const ClangCursor& cursor);
  void visit_class(const ClangCursor& cursor);
  void visit_enum(const ClangCursor& cursor);
  void visit_enumconstant(const ClangCursor& cursor);
  void visit_function(const ClangCursor& cursor);

  std::shared_ptr<cxx::Function> parseFunction(CXCursor cursor);
  void parseFunctionArgument(cxx::Function& func, CXCursor cursor);
  static  CXChildVisitResult param_decl_visitor(CXCursor cursor, CXCursor parent, CXClientData data);
  CXChildVisitResult visitFunctionParamDecl(CXCursor cursor, CXCursor parent, std::string& param);

  cxx::Type parseType(CXType t);

  cxx::SourceLocation getCursorLocation(CXCursor cursor);

private:
  cxx::FileSystem& m_filesystem;
  std::shared_ptr<Program> m_program;

  ClangIndex m_index;
  ClangTranslationUnit m_tu;

  CXFile m_tu_file = nullptr;
  CXFile m_current_cxfile = nullptr;
  std::shared_ptr<File> m_current_file = nullptr;

  std::set<std::shared_ptr<File>> m_parsed_files;

  std::unordered_map<ClangCursor, std::shared_ptr<Entity>> m_cursor_entity_map;

  cxx::AccessSpecifier m_access_specifier = cxx::AccessSpecifier::PUBLIC;
  std::vector<std::shared_ptr<cxx::Node>> m_program_stack;
  std::vector<std::shared_ptr<cxx::AstNode>> m_ast_stack;
};

} // namespace parsers

} // namespace cxx

namespace cxx
{

namespace parsers
{

} // namespace parsers

} // namespace cxx

#endif // CXXAST_PARSERS_PARSER_H
