// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/parser.h"

#include "cxx/parsers/restricted-parser.h"

#include "cxx/clang/clang-translation-unit.h"
#include "cxx/clang/clang-token.h"

#include "cxx/name_p.h"

#include "cxx/class.h"
#include "cxx/enum.h"
#include "cxx/function.h"
#include "cxx/namespace.h"
#include "cxx/statements.h"

#include "cxx/class-declaration.h"
#include "cxx/enum-declaration.h"
#include "cxx/function-declaration.h"
#include "cxx/namespace-declaration.h"

#include "cxx/filesystem.h"
#include "cxx/program.h"

#include <algorithm>
#include <iostream>
#include <map>

namespace cxx
{

namespace parsers
{

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

struct StacksGuard
{
  RAIIVectorSharedGuard<cxx::Node> prog_;
  RAIIVectorSharedGuard<cxx::AstNode> ast_;

  StacksGuard(std::vector<std::shared_ptr<cxx::Node>>& p, std::vector<std::shared_ptr<cxx::AstNode>>& a, 
    std::shared_ptr<cxx::Node> p_elem, std::shared_ptr<cxx::AstNode> a_elem)
    : prog_(p, p_elem),
      ast_(a, a_elem)
  {

  }
};

LibClangParser::LibClangParser()
  : m_filesystem(FileSystem::GlobalInstance()),
    m_index{*this}
{
  m_program = std::make_shared<Program>();
}

LibClangParser::~LibClangParser()
{
}

LibClangParser::LibClangParser(FileSystem& fs)
  : m_filesystem(fs),
    m_index{ *this }
{
  m_program = std::make_shared<Program>();
}

LibClangParser::LibClangParser(std::shared_ptr<Program> prog)
  : m_filesystem(FileSystem::GlobalInstance()),
    m_index{ *this }
{
  m_program = prog;
}

LibClangParser::LibClangParser(std::shared_ptr<Program> prog, cxx::FileSystem& fs)
  : m_filesystem(fs),
    m_index{ *this }
{
  m_program = prog;
}

std::shared_ptr<Program> LibClangParser::program() const
{
  return m_program;
}

bool LibClangParser::parse(const std::string& file)
{
  this->skipped_declarations.clear();

  try
  {
    m_tu = m_index.parseTranslationUnit(file, includedirs, skip_function_bodies ? CXTranslationUnit_SkipFunctionBodies : CXTranslationUnit_None);
  }
  catch (...)
  {
    return false;
  }

  StateGuard stack_guard{ m_program_stack, m_program->globalNamespace() };

  m_tu_file = clang_getFile(m_tu, file.data());

  ClangCursor c = m_tu.getCursor();

  c.visitChildren([this](ClangCursor c) {
    visit_tu(c);
    });

  return true;
}

cxx::AccessSpecifier LibClangParser::getAccessSpecifier(CX_CXXAccessSpecifier as)
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

std::shared_ptr<File> LibClangParser::getFile(const std::string& path)
{
  return m_filesystem.get(path);
}

cxx::Node& LibClangParser::curNode()
{
  return *m_program_stack.back();
}

void LibClangParser::astWrite(std::shared_ptr<Declaration> n)
{
  if (m_ast_stack.empty())
  {
    m_current_file->nodes.push_back(n);
  }
  else
  {
    cxx::AstNode& cur = *m_ast_stack.back();

    switch (cur.kind())
    {
    case NodeKind::NamespaceDeclaration:
      static_cast<cxx::NamespaceDeclaration&>(cur).declarations.push_back(n);
      break;
    case NodeKind::ClassDeclaration:
      static_cast<cxx::ClassDeclaration&>(cur).declarations.push_back(n);
      break;
    default:
      throw std::runtime_error{ "LibClangParser::astWrite() failed" };
    }
  }
}

void LibClangParser::write(std::shared_ptr<Entity> e)
{
  switch (curNode().kind())
  {
  case NodeKind::Namespace:
    static_cast<cxx::Namespace&>(curNode()).entities.push_back(e);
    break;
  case NodeKind::Class:
  case NodeKind::ClassTemplate:
    static_cast<cxx::Class&>(curNode()).members.push_back(e);
    break;
  case NodeKind::Enum:
    static_cast<cxx::Enum&>(curNode()).values.push_back(std::static_pointer_cast<cxx::EnumValue>(e));
    break;
  default:
    throw std::runtime_error{ "LibClangParser::write() failed" };
  }

  e->weak_parent = std::static_pointer_cast<cxx::Entity>(curNode().shared_from_this());
}

void LibClangParser::visit(const ClangCursor& cursor)
{
  CXCursorKind kind = cursor.kind();

  switch (kind)
  {
  case CXCursor_Namespace:
    return visit_namespace(cursor);
  case CXCursor_ClassDecl:
  case CXCursor_StructDecl:
  case CXCursor_ClassTemplate:
    return visit_class(cursor);
  case CXCursor_EnumDecl:
    return visit_enum(cursor);
  case CXCursor_CXXMethod:
  case CXCursor_FunctionDecl:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return visit_function(cursor);
  case CXCursor_EnumConstantDecl:
    return visit_enumconstant(cursor);
  case CXCursor_VarDecl:
    return visit_vardecl(cursor);
  case CXCursor_FieldDecl:
    return visit_fielddecl(cursor);
  case CXCursor_CXXAccessSpecifier:
    return visit_accessspecifier(cursor);
  case CXCursor_TemplateTypeParameter:
    return visit_template_type_parameter(cursor);
  default:
    return;
  }
}

void LibClangParser::visit_tu(const ClangCursor& cursor)
{
  assert(m_ast_stack.empty());

  // We are working at translation-unit level

  auto cursor_file = getCursorFile(cursor);

  if (!clang_File_isEqual(m_current_cxfile, cursor_file))
  {
    // We have reached another file

    m_parsed_files.insert(m_current_file);

    auto file = getFile(getCursorFilePath(cursor));

    if (m_parsed_files.find(file) != m_parsed_files.end())
    {
      // The file has already been parsed, we skip until we reach a non-parsed file
      return;
    }

    m_current_cxfile = cursor_file;
    m_current_file = file;
  }

  return visit(cursor);
}

void LibClangParser::visit_namespace(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();
  auto entity = static_cast<Namespace*>(m_program_stack.back().get())->getOrCreateNamespace(name);
  auto decl = std::make_shared<NamespaceDeclaration>(entity);

  decl->location = getCursorLocation(cursor);

  astWrite(decl);

  {
    StacksGuard guard{ m_program_stack, m_ast_stack, entity, decl };

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_class(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();

  const bool is_template = cursor.kind() == CXCursor_ClassTemplate;

  auto entity = [&]() -> std::shared_ptr<Class> {
    if (curNode().is<Namespace>())
    {
      auto& ns = static_cast<Namespace&>(curNode());

      if (is_template)
        return ns.getOrCreate<ClassTemplate>(name, std::vector<std::shared_ptr<TemplateParameter>>(), std::move(name));
      else
        return ns.getOrCreate<Class>(name, std::move(name));
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      std::shared_ptr<Class> result = !is_template ? (std::make_shared<Class>(std::move(name), cla.shared_from_this())) :
        (std::make_shared<ClassTemplate>(std::vector<std::shared_ptr<TemplateParameter>>(), std::move(name), cla.shared_from_this()));
      result->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(result);
      return result;
    }
  }();

  auto decl = std::make_shared<ClassDeclaration>(entity);
  decl->location = getCursorLocation(cursor);

  astWrite(decl);

  if (isForwardDeclaration(cursor))
    return;

  entity->location = getCursorLocation(cursor);
  m_cursor_entity_map[cursor] = entity;

  cxx::AccessSpecifier default_access = [&]() {
    if (clang_getCursorKind(cursor) == CXCursor_StructDecl)
      return cxx::AccessSpecifier::PUBLIC;
    else
      return cxx::AccessSpecifier::PRIVATE;
  }();

  {
    StacksGuard guard{ m_program_stack, m_ast_stack, entity, decl };
    RAIIGuard<cxx::AccessSpecifier> access_guard{ m_access_specifier };

    m_access_specifier = default_access;

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_enum(const ClangCursor& cursor)
{
  std::string name = getCursorSpelling(cursor);

  std::shared_ptr<Enum> entity = [&]() {
    if (curNode().is<Namespace>())
      return static_cast<Namespace&>(curNode()).createEnum(name);

    Class& cla = static_cast<Class&>(curNode());
    auto result = std::make_shared<Enum>(std::move(name), cla.shared_from_this());
    result->enum_class = clang_EnumDecl_isScoped(cursor);
    result->setAccessSpecifier(m_access_specifier);
    cla.members.push_back(result);
    return result;
  }();

  auto decl = std::make_shared<EnumDeclaration>(entity);
  decl->location = getCursorLocation(cursor);

  astWrite(decl);

  entity->location = getCursorLocation(cursor);

  {
    StacksGuard guard{ m_program_stack, m_ast_stack, entity, decl };

    cursor.visitChildren([this](ClangCursor c) {
      this->visit(c);
      });
  }
}

void LibClangParser::visit_enumconstant(const ClangCursor& cursor)
{
  std::string n = cursor.getSpelling();

  auto& en = static_cast<Enum&>(curNode());
  auto val = std::make_shared<EnumValue>(std::move(n), std::static_pointer_cast<cxx::Enum>(en.shared_from_this()));
  val->location = getCursorLocation(cursor);
  en.values.push_back(val);
}

static bool are_equiv_param(const cxx::FunctionParameter& a, const cxx::FunctionParameter& b)
{
  return a.type == b.type;
}

static bool are_equiv_func(const cxx::Function& a, const cxx::Function& b)
{
  if (a.name != b.name)
    return false;

  if (a.parameters.size() != b.parameters.size())
    return false;

  if (a.return_type != b.return_type)
    return false;

  for (size_t i(0); i < a.parameters.size(); ++i)
  {
    if (!are_equiv_param(*a.parameters.at(i), *b.parameters.at(i)))
      return false;
  }

  return true;
}

static std::shared_ptr<cxx::Function> find_equiv_func(cxx::Node& current_node, const cxx::Function& func)
{
  if (current_node.is<cxx::Class>())
  {
    for (const auto& m : static_cast<cxx::Class&>(current_node).members)
    {
      if (m->is<cxx::Function>())
      {
        if (are_equiv_func(static_cast<cxx::Function&>(*m), func))
          return std::static_pointer_cast<cxx::Function>(m);
      }
    }
  }
  else if(current_node.is<cxx::Namespace>())
  { 
    for (const auto& m : static_cast<cxx::Namespace&>(current_node).entities)
    {
      if (m->is<cxx::Function>())
      {
        if (are_equiv_func(static_cast<cxx::Function&>(*m), func))
          return std::static_pointer_cast<cxx::Function>(m);
      }
    }
  }

  return nullptr;
}

static void update_func(cxx::Function& func, const cxx::Function& new_one)
{
  for (size_t i(0); i < func.parameters.size(); ++i)
  {
    if (func.parameters.at(i)->default_value == cxx::Expression())
    {
      if (new_one.parameters.at(i)->default_value != cxx::Expression())
        func.parameters.at(i)->default_value = new_one.parameters.at(i)->default_value;
    }
  }

  if (func.body == nullptr && new_one.body != nullptr)
  {
    func.body = new_one.body;
  }
}

void LibClangParser::visit_function(const ClangCursor& cursor)
{
  // Tricky:
  // We may be dealing with a forward declaration, and the body may not be available.
  // We must create the Function nonetheless, even without the body.
  // Further declarations may provide the body or additional default parameters.

  std::shared_ptr<Function> entity;

  try
  {
    entity = parseFunction(cursor);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
    return;
  }
  

  auto decl = std::make_shared<FunctionDeclaration>(entity);
  decl->location = getCursorLocation(cursor);

  astWrite(decl);

  {
    StacksGuard guard{ m_program_stack, m_ast_stack, entity, decl };
    // TODO: parse func body
  }

  bool is_member = [&]() {
    switch (cursor.kind())
    {
    case CXCursor_CXXMethod:
    case CXCursor_Constructor:
    case CXCursor_Destructor:
      return true;
    default:
      return false;
    }
  }();

  auto semantic_parent = [&]() -> std::shared_ptr<cxx::Node> {
    if (is_member)
    {
      ClangCursor parent = cursor.getSemanticParent();
      auto it = m_cursor_entity_map.find(parent);

      if (it == m_cursor_entity_map.end())
        return nullptr;

      return it->second;
    }
    else
    {
      return curNode().shared_from_this();
    }
  }();

  if (semantic_parent == nullptr)
  {
    // @TODO: do not silently ignore this declaration
    return;
  }

  auto func = find_equiv_func(*semantic_parent, *entity);

  if (!func)
  {
    entity->weak_parent = std::static_pointer_cast<cxx::Entity>(curNode().shared_from_this());

    if (curNode().is<Namespace>())
    {
      static_cast<Namespace&>(curNode()).entities.push_back(entity);
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      entity->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(entity);
    }

    func = entity;
  }
  else
  {
    decl->function = func;
    update_func(*func, *entity);
  }

  if (!isForwardDeclaration(cursor))
    entity->location = decl->location;
}

void LibClangParser::visit_vardecl(const ClangCursor& cursor)
{
  std::shared_ptr<Variable> var;

  try
  {
    var = parseVariable(cursor);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    // Example of declaration that fails:
    // const typename _Iosb<_Dummy>::_Fmtflags count_ = 8;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
    return;
  }

  if (curNode().is<cxx::Class>())
  {
    // @TODO: allow access specifier on Variable
    // var->setAccessSpecifier(m_access_specifier);

    var->specifiers() |= VariableSpecifier::Static;

    write(var);
  }
  else if (curNode().is<cxx::Namespace>())
  {
    write(var);
  }
  else
  {
    // @TODO ?
  }
}

void LibClangParser::visit_fielddecl(const ClangCursor& cursor)
{
  assert(curNode().is<cxx::Class>());

  try
  {
    auto var = parseVariable(cursor);
    write(var);
  }
  catch (std::runtime_error & err)
  {
    (void)err;
    // Example of field that will fail:
    // struct { int n; } my_field;
    SkippedDeclaration decl;
    decl.loc = getCursorLocation(cursor);
    decl.content = cursor.getSpelling();
    this->skipped_declarations.push_back(std::move(decl));
  }
}

void LibClangParser::visit_accessspecifier(const ClangCursor& cursor)
{
  ClangTokenSet tokens = m_tu.tokenize(cursor.getExtent());

  CX_CXXAccessSpecifier aspec = cursor.getCXXAccessSpecifier();

  m_access_specifier = getAccessSpecifier(aspec);
}

void LibClangParser::visit_template_type_parameter(const ClangCursor& cursor)
{
  if (!curNode().is<ClassTemplate>())
    throw std::runtime_error{ "Not implemented" };

  ClassTemplate& ct = static_cast<ClassTemplate&>(curNode());

  auto ttparam = std::make_shared<TemplateParameter>(cursor.getSpelling());
  ttparam->weak_parent = ct.shared_from_this();
  ct.template_parameters.push_back(ttparam);
}

std::shared_ptr<cxx::Variable> LibClangParser::parseVariable(const ClangCursor& cursor)
{
  std::string name = cursor.getSpelling();
  Type type = parseType(cursor.getType());

  auto parent = std::static_pointer_cast<cxx::Entity>(curNode().shared_from_this());
  auto var = std::make_shared<Variable>(type, std::move(name), parent);
  var->location = getCursorLocation(cursor);

  if (cursor.childCount() == 1)
    var->defaultValue() = parseExpression(cursor.childAt(0));

  return var;
}

std::shared_ptr<cxx::Function> LibClangParser::parseFunction(const ClangCursor& cursor)
{
  auto func = std::make_shared<cxx::Function>(getCursorSpelling(cursor), std::dynamic_pointer_cast<cxx::Entity>(m_program_stack.back()));

  CXCursorKind kind = clang_getCursorKind(cursor);

  if (kind == CXCursor_Constructor)
  {
    func->kind = FunctionKind::Constructor;

    // @TODO: handle 'explicit' specifier
  }
  else if (kind == CXCursor_Destructor)
  {
    func->kind = FunctionKind::Destructor;
  }
  else
  {
    if (clang_CXXMethod_isConst(cursor))
      func->specifiers |= FunctionSpecifier::Const;
    if (clang_CXXMethod_isStatic(cursor))
      func->specifiers |= FunctionSpecifier::Static;
    if (clang_CXXMethod_isVirtual(cursor))
      func->specifiers |= FunctionSpecifier::Virtual;
    if (clang_CXXMethod_isPureVirtual(cursor))
      func->specifiers |= FunctionSpecifier::Pure;
  }

  int exception_spec_type = cursor.getExceptionSpecificationType();

  if (exception_spec_type == CXCursor_ExceptionSpecificationKind_BasicNoexcept)
    func->specifiers |= FunctionSpecifier::Noexcept;

  CXType function_type = clang_getCursorType(cursor);

  if (kind != CXCursor_Constructor && kind != CXCursor_Destructor)
  {
    func->return_type = parseType(clang_getResultType(function_type));
  }
  else
  {
    func->return_type = Type::Void;
  }

  const int num_args = clang_Cursor_getNumArguments(cursor);

  for (int i = 0; i < num_args; ++i)
  {
    CXCursor arg_cursor = clang_Cursor_getArgument(cursor, i);
    parseFunctionArgument(*func, arg_cursor);
  }

  // The children of a function seems to appear in the following order:
  // - return type (optional)
  // - parameters (CXCursor_ParmDecl)
  // - body (CXCursor_CompoundStmt)

  cursor.visitChildren([&](const ClangCursor& c) {

    if (c.kind() == CXCursor_CompoundStmt)
      func->body = parseStatement(c);

    });
  
  return func;
}

void LibClangParser::parseFunctionArgument(cxx::Function& func, CXCursor cursor)
{
  Type t = parseType(clang_getCursorType(cursor));
  std::string name = getCursorSpelling(cursor);

  auto param = std::make_shared<cxx::FunctionParameter>(t, std::move(name), std::static_pointer_cast<cxx::Function>(func.shared_from_this()));

  // Parse default-argument
  {
    std::string default_value_expr;

    auto data = std::make_pair(this, &default_value_expr);
    clang_visitChildren(cursor, &LibClangParser::param_decl_visitor, &data);

    if (!default_value_expr.empty())
      param->default_value = Expression{ std::move(default_value_expr) };
  }

  func.parameters.push_back(param);
}

CXChildVisitResult LibClangParser::param_decl_visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
  std::pair<LibClangParser*, std::string*>* p = (std::pair<LibClangParser*, std::string*>*) data;
  return p->first->visitFunctionParamDecl(cursor, parent, *(p->second));
}

CXChildVisitResult LibClangParser::visitFunctionParamDecl(CXCursor cursor, CXCursor parent, std::string& param)
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

  param += spelling;

  return CXChildVisit_Continue;
}

std::shared_ptr<cxx::Statement> LibClangParser::parseStatement(const ClangCursor& c)
{
  CXCursorKind k = c.kind();

  switch (k)
  {
  case CXCursor_NullStmt:
    return parseNullStatement(c);
  case CXCursor_CompoundStmt:
    return parseCompoundStatement(c);
  case CXCursor_IfStmt:
    return parseIf(c);
  case CXCursor_WhileStmt:
    return parseWhile(c);
  default:
    return parseUnexposedStatement(c);
  }
}

std::shared_ptr<cxx::Statement> LibClangParser::parseNullStatement(const ClangCursor& c)
{
  auto result = std::make_shared<NullStatement>();
  result->location = getCursorLocation(c);
  return result;
}

std::shared_ptr<cxx::Statement> LibClangParser::parseCompoundStatement(const ClangCursor& c)
{
  auto result = std::make_shared<CompoundStatement>();

  c.visitChildren([&](const ClangCursor& child) {
    result->statements.push_back(parseStatement(child));
    });

  return result;
}

std::shared_ptr<cxx::Statement> LibClangParser::parseIf(const ClangCursor& c)
{
  auto result = std::make_shared<IfStatement>();

  // The children if a CXCursor_IfStmt seems to appear in the following order:
  // - condition
  // - body
  // - else-clause (optional)

  enum IfParsingState
  {
    IfParsing_Cond,
    IfParsing_Body,
    IfParsing_Else,
  };

  IfParsingState state = IfParsing_Cond;

  c.visitChildren([&](const ClangCursor& child) {

    if (state == IfParsing_Cond)
    {
      result->condition = parseExpression(child);
      state = IfParsing_Body;
    }
    else if (state == IfParsing_Body)
    {
      result->body = parseStatement(child);
      state = IfParsing_Else;
    }
    else if (state == IfParsing_Else)
    {
      result->else_clause = parseStatement(child);
    }

    });

  return result;
}

std::shared_ptr<cxx::Statement> LibClangParser::parseWhile(const ClangCursor& c)
{
  auto result = std::make_shared<WhileLoop>();

  // The children if a CXCursor_WhileStmt seems to appear in the following order:
  // - condition
  // - body

  enum WhileParsingState
  {
    WhileParsing_Cond,
    WhileParsing_Body,
  };

  WhileParsingState state = WhileParsing_Cond;

  c.visitChildren([&](const ClangCursor& child) {

    if (state == WhileParsing_Cond)
    {
      result->condition = parseExpression(child);
    }
    else if (state == WhileParsing_Body)
    {
      result->body = parseStatement(child);
    }

    });

  return result;
}

std::shared_ptr<cxx::Statement> LibClangParser::parseUnexposedStatement(const ClangCursor& c)
{
  auto result = std::make_shared<UnexposedStatement>(std::string());

  CXSourceRange range = c.getExtent();
  ClangTokenSet tokens = m_tu.tokenize(range);

  std::string spelling;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    spelling += tokens.at(i).getSpelling();
  }

  result->source = spelling;

  return result;
}

cxx::Expression LibClangParser::parseExpression(const ClangCursor& c)
{
  std::string spelling = c.getSpelling();

  if(!spelling.empty())
    return Expression{ std::move(spelling) };

  CXSourceRange range = c.getExtent();
  ClangTokenSet tokens = m_tu.tokenize(range);

  for (size_t i = 0; i < tokens.size(); i++)
  {
    spelling += tokens.at(i).getSpelling();
  }

  return Expression{ std::move(spelling) };
}

static void remove_prefix(std::string& str, const std::string& prefix)
{
  if (str.find(prefix) == 0)
    str.erase(0, prefix.size());
}

cxx::Type LibClangParser::parseType(CXType t)
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

cxx::SourceLocation LibClangParser::getCursorLocation(CXCursor cursor)
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
    File::normalizePath(file_name);
    auto result_file = getFile(file_name);

    return cxx::SourceLocation(result_file, line, col);
  }
}

} // namespace parsers

} // namespace cxx

