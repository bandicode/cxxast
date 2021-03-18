// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_BUILTIN_PARSER_H
#define CXXAST_BUILTIN_PARSER_H

#include "cxx/parsers/lexer.h"

#include "cxx/function.h"
#include "cxx/macro.h"
#include "cxx/name.h"
#include "cxx/template.h"
#include "cxx/typedef.h"
#include "cxx/variable.h"

#include <map>
#include <set>
#include <stdexcept>
#include <vector>

namespace cxx
{

class FunctionDeclaration;
class VariableDeclaration;

class FileSystem;
class Program;

namespace parsers
{

class RestrictedParserError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;

  explicit RestrictedParserError(const std::string& text)
    : std::runtime_error(text.data())
  {

  }
};

class CXXAST_API RestrictedParser
{
public:
  std::set<std::string> includedirs;
  std::map<std::string, std::string> defines;
  bool skip_function_bodies = false;

public:

  RestrictedParser();
  ~RestrictedParser() = default;

  bool parse(const std::string& filepath);
  bool parse(const std::string& filepath, const std::string& content);
  std::shared_ptr<AstRootNode> parseSource(const std::string& content);

  std::shared_ptr<Program> program() const;
  void setProgram(std::shared_ptr<Program> p);

  static cxx::Type parseType(const std::string& str);
  static std::shared_ptr<Function> parseFunctionSignature(const std::string& str);
  static std::shared_ptr<Variable> parseVariable(const std::string& str);
  static std::shared_ptr<Typedef> parseTypedef(const std::string& str);
  static std::shared_ptr<Macro> parseMacro(const std::string& str);

protected:
  RestrictedParser(const std::string* src);

  Type parseType();
  Type tryReadFunctionSignature(const Type& result_type);

  Name parseName();

  std::shared_ptr<Function> parseFunctionSignature();
  std::shared_ptr<Variable> parseVariable();
  std::shared_ptr<Typedef> parseTypedef();
  std::shared_ptr<Macro> parseMacro();

protected:
  bool atEnd() const;
  Token read();
  Token unsafe_read();
  Token peek() const;
  Token unsafe_peek() const;
  Token prev() const;
  bool isDiscardable(const Token& t) const;
  Token read(TokenType::Value tokt);
  size_t pos() const;
  size_t pos(const Token& tok) const;
  void seek(size_t pos);
  void seekBegin();
  void seekEnd();

  std::string viewstring() const;
  std::string stringtoend() const;

protected:
  Name readOperatorName();
  Name readUserDefinedName();
  Name readTemplateArguments(const Name base);

protected:
  TemplateArgument parseDelimitedTemplateArgument();
  std::shared_ptr<TemplateParameter> parseDelimitedTemplateParameter();

protected:
  std::shared_ptr<Function::Parameter> parseFunctionParameter();

protected:

  cxx::INode& curNode();

  void astWrite(std::shared_ptr<AstNode> n);
  void localizeParentize(const std::shared_ptr<AstNode>& node, const Token& tok);
  void localizeParentize(const std::shared_ptr<AstNode>& node, const Token& first, const Token& last);
  void parentize(const std::shared_ptr<AstNode>& node);
  void bind(const std::shared_ptr<AstNode>& astnode, const std::shared_ptr<INode>& n);

  Statement parseStatement();
  NodeKind detectStatement();
  Statement parseFunctionBody(std::shared_ptr<cxx::Function> f);
  /* statements */
  std::shared_ptr<cxx::IStatement> parseNullStatement();
  std::shared_ptr<cxx::IStatement> parseBreakStatement();
  std::shared_ptr<cxx::IStatement> parseCaseStatement();
  std::shared_ptr<cxx::IStatement> parseCatchStatement();
  std::shared_ptr<cxx::IStatement> parseContinueStatement();
  std::shared_ptr<cxx::IStatement> parseCompoundStatement();
  std::shared_ptr<cxx::IStatement> parseDefaultStatement();
  std::shared_ptr<cxx::IStatement> parseDoWhileLoop();
  std::shared_ptr<cxx::IStatement> parseForLoop();
  std::shared_ptr<cxx::IStatement> parseIf();
  std::shared_ptr<cxx::IStatement> parseReturnStatement();
  std::shared_ptr<cxx::IStatement> parseSwitchStatement();
  std::shared_ptr<cxx::IStatement> parseTryBlock();
  std::shared_ptr<cxx::IStatement> parseWhile();
  std::shared_ptr<cxx::IStatement> parseUnexposedStatement();
  /* declarations */
  std::shared_ptr<cxx::IStatement> parseDecl();
  std::shared_ptr<cxx::IStatement> parseClassDecl();
  std::shared_ptr<cxx::IStatement> parseEnumDecl();
  std::shared_ptr<cxx::IStatement> parseNamespaceDecl();
  std::shared_ptr<cxx::IStatement> parseUsingDecl();
  std::shared_ptr<cxx::IStatement> parseTypedefDecl();
  std::shared_ptr<cxx::FunctionDeclaration> parseFunctionDecl();
  std::shared_ptr<cxx::VariableDeclaration> parseVarDecl();

  Expression parseExpression();

private:
  cxx::FileSystem* m_filesystem = nullptr;
  std::shared_ptr<Program> m_program;

private:
  Lexer m_lexer;
  std::vector<Token> m_buffer;
  std::pair<size_t, size_t> m_view;
  size_t m_index = 0;

private:
  std::shared_ptr<cxx::File> m_current_file;
  std::vector<std::shared_ptr<cxx::AstNode>> m_ast_stack;
  cxx::AccessSpecifier m_access_specifier = cxx::AccessSpecifier::PUBLIC;
  std::vector<std::shared_ptr<cxx::INode>> m_program_stack;
  bool m_parsing_function_body = false;
};

} // namespace parsers

} // namespace cxx

#endif // CXXAST_BUILTIN_PARSER_H
