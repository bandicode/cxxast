// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_PARSERS_PARSER_H
#define CXXAST_PARSERS_PARSER_H

#include "cxx/parsers/libclang-parser.h"

#include "cxx/function.h"
#include "cxx/translation-unit.h"

#include <map>
#include <set>
#include <vector>

namespace cxx
{

enum class AccessSpecifier;
class Class;
class Enum;
class Namespace;
class Type;

class FileSystem;

namespace parsers
{

class CXXAST_API Parser : public LibClangParser
{
public:
  Parser();
  ~Parser();

  explicit Parser(cxx::FileSystem& fs);

  std::set<std::string>& includedirs();
  const std::set<std::string>& includedirs() const;

  std::map<std::string, std::string>& defines();
  const std::map<std::string, std::string>& defines() const;

  bool ignoreOutsideDeclarations() const;
  void ignoreOutsideDeclarations(bool on);

  std::shared_ptr<TranslationUnit> parse(const std::string& file);

  const std::vector<std::shared_ptr<File>>& files() const;

  static cxx::AccessSpecifier getAccessSpecifier(CX_CXXAccessSpecifier as);

protected:
  std::shared_ptr<File> getFile(const std::string& path);

  /* Visitor callbacks */
  static CXChildVisitResult visitor_callback(CXCursor cursor, CXCursor parent, CXClientData client_data);
  CXChildVisitResult visit(CXCursor cursor, CXCursor parent);
  CXChildVisitResult visitTU(CXCursor cursor, CXCursor parent, std::shared_ptr<TranslationUnit> tu);
  CXChildVisitResult visitNamespace(CXCursor cursor, CXCursor parent, std::shared_ptr<Namespace> ns);
  CXChildVisitResult visitClass(CXCursor cursor, CXCursor parent, std::shared_ptr<Class> cla);
  CXChildVisitResult visitEnum(CXCursor cursor, CXCursor parent, std::shared_ptr<Enum> en);

  std::shared_ptr<cxx::Function> parseFunction(CXCursor cursor);
  void parseFunctionArgument(cxx::Function& func, CXCursor cursor);
  static  CXChildVisitResult param_decl_visitor(CXCursor cursor, CXCursor parent, CXClientData data);
  CXChildVisitResult visitFunctionParamDecl(CXCursor cursor, CXCursor parent, std::string& param);

  // visitors used for discovery purpose
  static CXChildVisitResult print_visitor_callback(CXCursor cursor, CXCursor parent, CXClientData client_data);
  CXChildVisitResult printVisit(CXCursor cursor, CXCursor parent);

  cxx::Type parseType(CXType t);

  cxx::SourceLocation getCursorLocation(CXCursor cursor);

private:
  cxx::FileSystem& m_filesystem;
  
  CXIndex m_index = nullptr;

  bool m_ignore_outside_decls = false;
  CXTranslationUnit m_tu = nullptr;
  CXFile m_tu_file = nullptr;
  CXFile m_current_cxfile = nullptr;
  std::shared_ptr<File> m_current_file = nullptr;

  std::set<std::string> m_include_dirs;
  std::map<std::string, std::string> m_defines;
  std::vector<std::shared_ptr<File>> m_files;

  std::vector<std::shared_ptr<cxx::Node>> m_stack;
};

} // namespace parsers

} // namespace cxx

namespace cxx
{

namespace parsers
{

inline std::set<std::string>& Parser::includedirs()
{
  return m_include_dirs;
}

inline const std::set<std::string>& Parser::includedirs() const
{
  return m_include_dirs;
}

inline std::map<std::string, std::string>& Parser::defines()
{
  return m_defines;
}

inline const std::map<std::string, std::string>& Parser::defines() const
{
  return m_defines;
}

inline const std::vector<std::shared_ptr<File>>& Parser::files() const
{
  return m_files;
}

inline bool Parser::ignoreOutsideDeclarations() const
{
  return m_ignore_outside_decls;
}

inline void Parser::ignoreOutsideDeclarations(bool on)
{
  m_ignore_outside_decls = on;
}

} // namespace parsers

} // namespace cxx

#endif // CXXAST_PARSERS_PARSER_H
