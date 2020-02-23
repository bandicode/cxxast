// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/parser.h"

#include "cxx/parsers/restricted-parser.h"

#include "cxx/name_p.h"

#include "cxx/class.h"
#include "cxx/enum.h"
#include "cxx/function.h"
#include "cxx/namespace.h"

#include <algorithm>
#include <iostream>
#include <map>

namespace cxx
{

namespace parsers
{

struct TranslationUnitGuard
{
  CXTranslationUnit& translation_unit;
  ClangDisposeTranslationUnit clang_disposeTranslationUnit = nullptr;

  TranslationUnitGuard(CXTranslationUnit& tu, ClangDisposeTranslationUnit callback)
    : translation_unit(tu), clang_disposeTranslationUnit(callback) { }

  ~TranslationUnitGuard()
  {
    clang_disposeTranslationUnit(translation_unit);
  }
};

struct StateGuard
{
  std::vector<std::shared_ptr<cxx::Node>>& m_stack;

  StateGuard(std::vector<std::shared_ptr<cxx::Node>>& stack, std::shared_ptr<cxx::Node> node)
    : m_stack(stack)
  {
    m_stack.push_back(node);
  }

  ~StateGuard()
  {
    m_stack.pop_back();
  }
};

Parser::Parser()
{
  m_index = clang_createIndex(0, 0);
}

Parser::~Parser()
{
  clang_disposeIndex(m_index);
}

std::shared_ptr<TranslationUnit> Parser::parse(const std::string& file)
{
  auto result = std::make_shared<TranslationUnit>(getFile(file));

  const int options = CXTranslationUnit_SkipFunctionBodies;

  const char* command_line_args[128] = { nullptr };
  std::vector<std::string> argv{ "-x", "c++", "-Xclang", "-ast-dump", "-fsyntax-only" };
  for (const std::string& f : includedirs())
  {
    argv.push_back("--include-directory");
    argv.push_back(f);
  }
  for (int i(0); i < argv.size(); ++i)
    command_line_args[i] = argv.at(i).data();

  CXErrorCode error = clang_parseTranslationUnit2(m_index, file.data(), command_line_args, static_cast<int>(argv.size()), nullptr, 0, options, &m_tu);

  TranslationUnitGuard tu_guard{ m_tu, clang_disposeTranslationUnit };
  StateGuard stack_guard{ m_stack, result };

  if (error)
    return nullptr;

  m_tu_file = clang_getFile(m_tu, file.data());
  CXCursor cursor = clang_getTranslationUnitCursor(m_tu);
  clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);

  return result;
}

cxx::AccessSpecifier Parser::getAccessSpecifier(CX_CXXAccessSpecifier as)
{
  switch (as)
  {
  case CX_CXXAccessSpecifier::CX_CXXPublic:
    return cxx::AccessSpecifier::PUBLIC;
  case CX_CXXAccessSpecifier::CX_CXXProtected:
    return cxx::AccessSpecifier::PROTECTED;
  case CX_CXXAccessSpecifier::CX_CXXPrivate:
    return cxx::AccessSpecifier::PRIVATE;
  }

  return cxx::AccessSpecifier::PRIVATE;
}

std::shared_ptr<File> Parser::getFile(const std::string& path)
{
  auto it = std::find_if(files().begin(), files().end(), [&path](const std::shared_ptr<File>& file) {
    return file->path() == path;
    });

  if (it != files().end())
    return *it;

  auto file = std::make_shared<File>(path);
  m_files.push_back(file);

  return file;
}

CXChildVisitResult Parser::print_visitor_callback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
  return static_cast<Parser*>(client_data)->printVisit(cursor, parent);
}

CXChildVisitResult Parser::printVisit(CXCursor cursor, CXCursor parent)
{
  std::cout << toStdString(clang_getCursorKindSpelling(cursor.kind)) << ": "<< getCursorSpelling(cursor) << std::endl;
  return CXChildVisit_Recurse;
}

CXChildVisitResult Parser::visitor_callback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
  return static_cast<Parser*>(client_data)->visit(cursor, parent);
}

CXChildVisitResult Parser::visit(CXCursor cursor, CXCursor parent)
{
  if (m_stack.back()->is<cxx::Namespace>())
    return visitNamespace(cursor, parent, std::static_pointer_cast<cxx::Namespace>(m_stack.back()));
  else if (m_stack.back()->is<cxx::Class>())
    return visitClass(cursor, parent, std::static_pointer_cast<cxx::Class>(m_stack.back()));
  else if (m_stack.back()->is<cxx::Enum>())
    return visitEnum(cursor, parent, std::static_pointer_cast<cxx::Enum>(m_stack.back()));
  else if (m_stack.back()->is<cxx::TranslationUnit>())
    return visitTU(cursor, parent, std::static_pointer_cast<cxx::TranslationUnit>(m_stack.back()));

  return CXChildVisitResult::CXChildVisit_Break;
}

CXChildVisitResult Parser::visitTU(CXCursor cursor, CXCursor parent, std::shared_ptr<TranslationUnit> tu)
{
  if (ignoreOutsideDeclarations() && !clang_File_isEqual(getCursorFile(cursor), m_tu_file))
    return CXChildVisit_Continue;

  m_current_cxfile = getCursorFile(cursor);
  m_current_file = getFile(getCursorFilePath(cursor));

  if (clang_getCursorKind(cursor) == CXCursor_Namespace)
  {
    std::shared_ptr<Namespace> inner = std::make_shared<cxx::Namespace>(getCursorSpelling(cursor));
    inner->location() = getCursorLocation(cursor);

    StateGuard guard{ m_stack, inner };
    clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);

    tu->nodes().push_back(inner);
  }
  else if (clang_getCursorKind(cursor) == CXCursor_ClassDecl || clang_getCursorKind(cursor) == CXCursor_StructDecl)
  {
    if (isForwardDeclaration(cursor))
      return CXChildVisit_Continue;

    auto c = std::make_shared<cxx::Class>(getCursorSpelling(cursor));
    c->location() = getCursorLocation(cursor);
    c->isStruct() = (clang_getCursorKind(cursor) == CXCursor_StructDecl);

    {
      StateGuard guard{ m_stack, c };
      clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
    }

    if (!c->members().empty())
    {
      tu->nodes().push_back(c);
    }
  }
  else if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl)
  {
    std::shared_ptr<cxx::Function> func = parseFunction(cursor);
    func->location() = getCursorLocation(cursor);
    tu->nodes().push_back(func);
  }
  else if (clang_getCursorKind(cursor) == CXCursor_EnumDecl)
  {
    if (isForwardDeclaration(cursor))
      return CXChildVisit_Continue;

    std::shared_ptr<Enum> e = std::make_shared<cxx::Enum>(getCursorSpelling(cursor));
    e->location() = getCursorLocation(cursor);

    StateGuard guard{ m_stack, e };
    clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);

    tu->nodes().push_back(e);
  }

  return CXChildVisit_Continue;
}

CXChildVisitResult Parser::visitNamespace(CXCursor cursor, CXCursor parent, std::shared_ptr<Namespace> ns)
{
  if (clang_getCursorKind(cursor) == CXCursor_Namespace)
  {
    std::shared_ptr<Namespace> inner = ns->getOrCreateNamespace(getCursorSpelling(cursor));
    inner->location() = getCursorLocation(cursor);

    StateGuard guard{ m_stack, inner };
    clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
  }
  else if (clang_getCursorKind(cursor) == CXCursor_ClassDecl)
  {
    if (isForwardDeclaration(cursor))
      return CXChildVisit_Continue;

    std::shared_ptr<Class> c = ns->createClass(getCursorSpelling(cursor));
    c->location() = getCursorLocation(cursor);

    {
      StateGuard guard{ m_stack, c };
      clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
    }

    if (c->members().empty())
      ns->entities().pop_back();
  }
  else if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl)
  {
    std::shared_ptr<Function> func = parseFunction(cursor);
    func->location() = getCursorLocation(cursor);
    ns->entities().push_back(func);
  }
  else if (clang_getCursorKind(cursor) == CXCursor_EnumDecl)
  {
    if (isForwardDeclaration(cursor))
      return CXChildVisit_Continue;

    std::shared_ptr<Enum> e = ns->createEnum(getCursorSpelling(cursor));
    e->location() = getCursorLocation(cursor);

    StateGuard guard{ m_stack, e };
    clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
  }

  return CXChildVisit_Continue;
}

CXChildVisitResult Parser::visitClass(CXCursor cursor, CXCursor parent, std::shared_ptr<Class> cla)
{
  cxx::AccessSpecifier access_specifier = getAccessSpecifier(clang_getCXXAccessSpecifier(cursor));

  CXCursorKind kind = clang_getCursorKind(cursor);

  if (kind == CXCursor_CXXMethod || kind == CXCursor_FunctionDecl || kind == CXCursor_Constructor || kind == CXCursor_Destructor)
  {
    std::shared_ptr<cxx::Function> func = parseFunction(cursor);
    func->location() = getCursorLocation(cursor);
    cla->members().push_back(std::make_pair(func, access_specifier));
  }
  else if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
  {
    if (isForwardDeclaration(cursor))
      return CXChildVisit_Continue;

    auto nested_class = std::make_shared<cxx::Class>(getCursorSpelling(cursor), cla);
    nested_class->location() = getCursorLocation(cursor);

    {
      StateGuard nested_class_guard{ m_stack, nested_class };
      clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
    }

    if (!nested_class->members().empty())
    {
      cla->members().push_back(std::make_pair(nested_class, access_specifier));
    }
  }
  else if (clang_getCursorKind(cursor) == CXCursor_EnumDecl)
  {
    auto en = std::make_shared<cxx::Enum>(getCursorSpelling(cursor), cla);
    en->location() = getCursorLocation(cursor);

    {
      StateGuard nested_class_guard{ m_stack, en };
      clang_visitChildren(cursor, &Parser::visitor_callback, (void*)this);
    }

    cla->members().push_back(std::make_pair(en, access_specifier));
  }

  return CXChildVisit_Continue;
}

CXChildVisitResult Parser::visitEnum(CXCursor cursor, CXCursor parent, std::shared_ptr<Enum> en)
{
  CXCursorKind kind = clang_getCursorKind(cursor);

  if (kind == CXCursor_EnumConstantDecl)
  {
    std::string n = getCursorSpelling(cursor);
    en->values().push_back(Enum::Value{std::move(n)});
  }

  return CXChildVisit_Continue;
}

std::shared_ptr<cxx::Function> Parser::parseFunction(CXCursor cursor)
{
  auto func = std::make_shared<cxx::Function>(getCursorSpelling(cursor), std::dynamic_pointer_cast<cxx::Entity>(m_stack.back()));

  CXCursorKind kind = clang_getCursorKind(cursor);

  if (kind == CXCursor_Constructor)
  {
    func->kind() = FunctionKind::Constructor;
  }
  else if (kind == CXCursor_Destructor)
  {
    func->kind() = FunctionKind::Destructor;
  }
  else
  {
    if (clang_CXXMethod_isConst(cursor))
      func->specifiers() |= FunctionSpecifier::Const;
    if (clang_CXXMethod_isStatic(cursor))
      func->specifiers() |= FunctionSpecifier::Static;
  }

  CXType function_type = clang_getCursorType(cursor);

  if (kind != CXCursor_Constructor && kind != CXCursor_Destructor)
  {
    func->returnType() = parseType(clang_getResultType(function_type));
  }
  else
  {
    func->returnType() = Type::Void;
  }

  const int num_args = clang_Cursor_getNumArguments(cursor);

  for (int i = 0; i < num_args; ++i)
  {
    CXCursor arg_cursor = clang_Cursor_getArgument(cursor, i);
    parseFunctionArgument(*func, arg_cursor);
  }

  return func;
}

void Parser::parseFunctionArgument(cxx::Function& func, CXCursor cursor)
{
  cxx::Function::Parameter param;
  param.type = parseType(clang_getCursorType(cursor));
  param.name = getCursorSpelling(cursor);

  // Parse default-argument
  {
    auto data = std::make_pair(this, &param);
    clang_visitChildren(cursor, &Parser::param_decl_visitor, &data);
  }

  func.parameters().push_back(std::move(param));
}

CXChildVisitResult Parser::param_decl_visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
  std::pair<Parser*, Function::Parameter*>* p = (std::pair<Parser*, Function::Parameter*>*) data;
  return p->first->visitFunctionParamDecl(cursor, parent, *(p->second));
}

CXChildVisitResult Parser::visitFunctionParamDecl(CXCursor cursor, CXCursor parent, Function::Parameter& param)
{
  std::string spelling = getCursorSpelling(cursor);

  if (spelling.empty())
  {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXToken* tokens = 0;
    unsigned int nTokens = 0;
    clang_tokenize(m_tu, range, &tokens, &nTokens);
    for (unsigned int i = 0; i < nTokens; i++)
    {
      spelling += getTokenSpelling(m_tu, tokens[i]);
    }
    clang_disposeTokens(m_tu, tokens, nTokens);
  }

  param.default_value += spelling;

  return CXChildVisit_Continue;
}

static void remove_prefix(std::string& str, const std::string& prefix)
{
  if (str.find(prefix) == 0)
    str.erase(0, prefix.size());
}

cxx::Type Parser::parseType(CXType t)
{
  bool is_const = clang_isConstQualifiedType(t);
  bool is_volatile = clang_isVolatileQualifiedType(t);

  cxx::CVQualifier cv_qual = make_cv_qual(is_const, is_volatile);

  if (t.kind == CXTypeKind::CXType_LValueReference || t.kind == CXTypeKind::CXType_RValueReference)
  {
    cxx::Reference ref = t.kind == CXTypeKind::CXType_LValueReference ? cxx::Reference::LValue : cxx::Reference::RValue;

    CXType nested_type = clang_getPointeeType(t);

    return Type::reference(Type::cvQualified(parseType(nested_type), cv_qual), ref);
  }
  else if (t.kind == CXTypeKind::CXType_Pointer)
  {
    CXType nested_type = clang_getPointeeType(t);

    return Type::cvQualified(Type::pointer(parseType(nested_type)), cv_qual);
  }
  else
  {
    if (t.kind == CXTypeKind::CXType_Elaborated)
    {
      std::string spelling = getTypeSpelling(t);
      return RestrictedParser::parseType(spelling);
    }
    else
    {
      std::string spelling = getTypeSpelling(t);
      remove_prefix(spelling, "const ");

      return Type{ std::move(spelling), cv_qual };
    }
  }
}

cxx::SourceLocation Parser::getCursorLocation(CXCursor cursor)
{
  CXSourceLocation location = clang_getCursorLocation(cursor);

  CXFile file;
  unsigned int line, col, offset;
  clang_getSpellingLocation(location, &file, &line, &col, &offset);

  if (clang_File_isEqual(m_current_cxfile, file))
  {
    return cxx::SourceLocation(m_current_file, line, col);
  }
  else
  {
    std::string file_name = toStdString(clang_getFileName(file));

    for (char& c : file_name)
    {
      if (c == '\\')
        c = '/';
    }

    auto result_file = getFile(file_name);

    return cxx::SourceLocation(result_file, line, col);
  }
}

} // namespace parsers

} // namespace cxx

