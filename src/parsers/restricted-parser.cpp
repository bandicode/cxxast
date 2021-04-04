// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/restricted-parser.h"

#include "cxx/parsers/raii-utils.h"

#include "cxx/class.h"
#include "cxx/filesystem.h"
#include "cxx/name_p.h"
#include "cxx/namespace.h"
#include "cxx/program.h"
#include "cxx/statements.h"
#include "cxx/function-body.h"
#include "cxx/declarations.h"

#include <fstream>
#include <map>

namespace cxx
{

namespace parsers
{

// RaiiAstLocator will sets a node's parent, push it into a stack
// and sets its location upon exiting.
class RaiiAstLocator
{
public:
  RestrictedParser& parser_;
  Token start_;

  RaiiAstLocator(RestrictedParser* p, std::shared_ptr<AstNode> elem)
    : parser_(*p)
  {
    if (!parser_.m_ast_stack.empty())
      elem->weak_parent = parser_.m_ast_stack.back();

    parser_.m_ast_stack.push_back(elem);

    start_ = parser_.peek();
  }

  ~RaiiAstLocator()
  {
    std::shared_ptr<AstNode> elem = parser_.m_ast_stack.back();

    parser_.localize(elem, start_, parser_.prev());

    parser_.m_ast_stack.pop_back();
  }
};

class ParserViewRAII
{
protected:
  std::pair<size_t, size_t>& m_view;
  const std::pair<size_t, size_t> m_backup;

public:
  explicit ParserViewRAII(std::pair<size_t, size_t>& view)
    : m_view(view),
      m_backup(view)
  {

  }

  ParserViewRAII(std::pair<size_t, size_t>& view, size_t begin, size_t end)
    : m_view(view),
      m_backup(view)
  {
    m_view = std::make_pair(begin, end);
  }

  ~ParserViewRAII()
  {
    m_view = m_backup;
  }
};

class ParserParenView : public ParserViewRAII
{
public:

  ParserParenView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
  {
    size_t par_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == TokenType::RightPar)
      {
        if (par_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
        else
        {
          --par_depth;
        }
      }
      else if (it->type() == TokenType::LeftPar)
      {
        ++par_depth;
      }

      ++it;
    }

    throw RestrictedParserError{ "no matching parenthesis" };
  }
};

class ParserBracketView : public ParserViewRAII
{
public:
  ParserBracketView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
  {
    size_t bracket_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == TokenType::RightBracket)
      {
        if (bracket_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
        else
        {
          --bracket_depth;
        }
      }
      else if (it->type() == TokenType::LeftBracket)
      {
        ++bracket_depth;
      }

      ++it;
    }

    throw RestrictedParserError{ "no matching bracket" };
  }
};

class ParserBraceView : public ParserViewRAII
{
public:
  ParserBraceView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
  {
    size_t brace_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == TokenType::RightBrace)
      {
        if (brace_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
        else
        {
          --brace_depth;
        }
      }
      else if (it->type() == TokenType::LeftBrace)
      {
        ++brace_depth;
      }

      ++it;
    }

    throw RestrictedParserError{ "no matching brace" };
  }
};

class TemplateAngleView : public ParserViewRAII
{
public:
  bool split_right_shift = false;
  std::vector<Token>& tokens;

public:
  TemplateAngleView(std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view),
      tokens(toks)
  {
    size_t bracket_depth = 0;
    size_t paren_depth = 0;
    size_t angle_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == TokenType::GreaterThan)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          if (angle_depth == 0)
          {
            view = std::make_pair(pos, std::distance(toks.begin(), it));
            return;
          }

          --angle_depth;
        }
      }
      else if (it->type() == TokenType::RightShift)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          if (angle_depth == 1)
          {
            Token tok = *it;
            *it = Token(TokenType::RightAngle, StringView(tok.text().data() + 1, 1));
            it = toks.insert(it, Token(TokenType::RightAngle, StringView(tok.text().data(), 1)));
            view = std::make_pair(pos, std::distance(toks.begin(), it) + 1);
            split_right_shift = true;
            return;
          }

          angle_depth -= 2;
        }
      }
      else if (it->type() == TokenType::Less)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          ++angle_depth;
        }
      }
      else if (it->type() == TokenType::RightBracket)
      {
        if (bracket_depth == 0)
          throw RestrictedParserError{ "no matching bracket" };

        --bracket_depth;
      }
      else if (it->type() == TokenType::LeftBracket)
      {
        ++bracket_depth;
      }
      else if (it->type() == TokenType::RightPar)
      {
        if (paren_depth == 0)
          throw RestrictedParserError{ "no matching parenthesis" };

        --paren_depth;
      }
      else if (it->type() == TokenType::LeftPar)
      {
        ++paren_depth;
      }

      ++it;
    }

    throw RestrictedParserError{ "no matching angle bracket" };
  }

  ~TemplateAngleView()
  {
    if (split_right_shift)
    {
      Token tok = tokens.at(m_view.second - 1);
      tokens.erase(tokens.begin() + m_view.second);
      tokens[m_view.second - 1] = Token(TokenType::RightShift, StringView(tok.text().data(), 2));
    }
  }
};

class ListView : public ParserViewRAII
{
public:
  ListView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos, bool ignore_angle = true)
    : ParserViewRAII(view)
  {
    size_t bracket_depth = 0;
    size_t paren_depth = 0;
    size_t brace_depth = 0;
    size_t angle_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == TokenType::Comma)
      {
        if (brace_depth == 0 && paren_depth == 0 && bracket_depth == 0 && angle_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
      }
      else if (it->type() == TokenType::GreaterThan)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          if (angle_depth == 0)
            throw RestrictedParserError{ "no matching angle bracket" };

          --angle_depth;
        }
      }
      else if (it->type() == TokenType::Less)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          ++angle_depth;
        }
      }
      else if (it->type() == TokenType::RightShift)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          angle_depth -= 2;
        }
      }
      else if (it->type() == TokenType::RightBracket)
      {
        if (bracket_depth == 0)
          throw RestrictedParserError{ "no matching bracket" };

        --bracket_depth;
      }
      else if (it->type() == TokenType::LeftBracket)
      {
        ++bracket_depth;
      }
      else if (it->type() == TokenType::RightPar)
      {
        if (paren_depth == 0)
          throw RestrictedParserError{ "no matching parenthesis" };

        --paren_depth;
      }
      else if (it->type() == TokenType::LeftPar)
      {
        ++paren_depth;
      }
      else if (it->type() == TokenType::RightBrace)
      {
        if (brace_depth == 0)
          throw RestrictedParserError{ "no matching brace" };

        --brace_depth;
      }
      else if (it->type() == TokenType::LeftBrace)
      {
        ++brace_depth;
      }


      ++it;
    }

    if (bracket_depth != 0 || paren_depth != 0 || brace_depth != 0)
    {
      throw RestrictedParserError{ "no matching bracket/brace/paren" };
    }
  }
};

class ParserSentinelView : public ParserViewRAII
{
public:

  ParserSentinelView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos, TokenType tt)
    : ParserViewRAII(view)
  {
    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == tt)
      {
        view = std::make_pair(pos, std::distance(toks.begin(), it));
        return;
      }

      ++it;
    }

    throw RestrictedParserError{ "no matching token" };
  }
};

class ParserSemicolonView : public ParserSentinelView
{
public:

  ParserSemicolonView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserSentinelView(toks, view, pos, TokenType::Semicolon)
  {

  }
};

class ParserColonView : public ParserSentinelView
{
public:

  ParserColonView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserSentinelView(toks, view, pos, TokenType::Colon)
  {

  }
};

RestrictedParser::RestrictedParser(const std::string *src)
  : m_lexer(src)
{
  m_lexer.start();

  while (!m_lexer.atEnd())
  {
    const Token t = m_lexer.read();
    if (!isDiscardable(t))
      m_buffer.push_back(t);
  }

  m_view = std::make_pair(0, m_buffer.size());

  m_ast_stack.push_back(std::make_shared<AstRootNode>());
}

RestrictedParser::RestrictedParser()
  : m_filesystem(&cxx::FileSystem::GlobalInstance()),
    m_program(std::make_shared<Program>())
{
  setProgram(m_program);
}

bool RestrictedParser::parse(const std::string& filepath)
{
  std::ifstream stream{ filepath };

  std::string line;
  std::string filecontent;

  while (std::getline(stream, line))
  {
    filecontent += line;
    filecontent.push_back('\n');
  }

  return parse(filepath, filecontent);
}

bool RestrictedParser::parse(const std::string& filepath, const std::string& content)
{
  auto fileobj = m_filesystem->get(filepath);
  m_lexer.reset(&content);

  m_lexer.start();
  m_buffer.clear();

  while (!m_lexer.atEnd())
  {
    const Token t = m_lexer.read();
    if (!isDiscardable(t))
      m_buffer.push_back(t);
  }

  m_index = 0;
  m_view = std::make_pair(size_t(0), m_buffer.size());

  auto astnode = std::make_shared<AstRootNode>();
  fileobj->ast = astnode;

  m_current_file = fileobj;
  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, astnode };

  while (!atEnd())
  {
    Statement stmt = parseStatement();
    astnode->childvec.push_back(cxx::to_ast_node(stmt));
  }

  return false;
}

std::shared_ptr<AstRootNode> RestrictedParser::parseSource(const std::string& content)
{
  m_lexer.reset(&content);

  m_lexer.start();
  m_buffer.clear();

  while (!m_lexer.atEnd())
  {
    const Token t = m_lexer.read();
    if (!isDiscardable(t))
      m_buffer.push_back(t);
  }

  m_index = 0;
  m_view = std::make_pair(size_t(0), m_buffer.size());

  auto astnode = std::make_shared<AstRootNode>();

  RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, astnode };

  while (!atEnd())
  {
    Statement stmt = parseStatement();
    astnode->childvec.push_back(cxx::to_ast_node(stmt));
  }

  return astnode;
}

std::shared_ptr<Program> RestrictedParser::program() const
{
  return m_program;
}

void RestrictedParser::setProgram(std::shared_ptr<Program> p)
{
  m_program = p;

  m_program_stack.clear();
  m_program_stack.push_back(m_program->globalNamespace());
}

cxx::Type RestrictedParser::parseType(const std::string& str)
{
  RestrictedParser p{ &str };
  return p.parseType();
}

std::shared_ptr<Function> RestrictedParser::parseFunctionSignature(const std::string& str)
{
  RestrictedParser p{ &str };
  return p.parseFunctionSignature();
}

std::shared_ptr<Variable> RestrictedParser::parseVariable(const std::string& str)
{
  RestrictedParser p{ &str };
  return p.parseVariable();
}

std::shared_ptr<Typedef> RestrictedParser::parseTypedef(const std::string& str)
{
  RestrictedParser p{ &str };
  return p.parseTypedef();
}

std::shared_ptr<Macro> RestrictedParser::parseMacro(const std::string& str)
{
  RestrictedParser p{ &str };
  return p.parseMacro();
}

bool RestrictedParser::atEnd() const
{
  return m_index == m_view.second;
}

Type RestrictedParser::parseType()
{
  CVQualifier cv_qual = CVQualifier::None;
  Reference ref = Reference::None;

  if (peek() == TokenType::Const)
    unsafe_read(), cv_qual = CVQualifier::Const;

  Name type_name = parseName();

  if (atEnd())
    return Type(type_name.toString(), cv_qual, ref);

  if (unsafe_peek() == TokenType::Const)
  {
    unsafe_read(), cv_qual = CVQualifier::Const;

    if (atEnd())
      return Type(type_name.toString(), cv_qual, ref);

    if (unsafe_peek() == TokenType::Ref || unsafe_peek() == TokenType::RefRef)
    {
      if (unsafe_peek() == TokenType::Ref)
        ref = Reference::LValue;
      else
        ref = Reference::RValue;
    }
  }
  else if (unsafe_peek() == TokenType::Ref || unsafe_peek() == TokenType::RefRef)
  {
    if (unsafe_read() == TokenType::Ref)
      ref = Reference::LValue;
    else
      ref = Reference::RValue;

    if (atEnd())
      return Type(type_name.toString(), cv_qual, ref);

    if (unsafe_peek() == TokenType::Const)
      unsafe_read(), cv_qual = CVQualifier::Const;
  }

  if (atEnd())
    return Type(type_name.toString(), cv_qual, ref);

  if (unsafe_peek() == TokenType::LeftPar) 
  {
    auto save_point = pos();

    try
    {
      auto fsig = tryReadFunctionSignature(Type(type_name.toString(), cv_qual, ref));
      return fsig;
    }
    catch (const std::runtime_error&)
    {
      seek(save_point);
    }
  }
  else if (peek() == TokenType::Star)
  {
    unsafe_read();

    Type t = Type(type_name.toString(), cv_qual, ref);
    t = Type::pointer(t);

    while (!atEnd() && (peek() == TokenType::Const || unsafe_peek() == TokenType::Star))
    {
      Token tok = read();

      if (tok == TokenType::Const)
        t = Type::cvQualified(t, CVQualifier::Const);
      else
        t = Type::pointer(t);
    }

    return t;
  }

  return Type(type_name.toString(), cv_qual, ref);
}

Type RestrictedParser::tryReadFunctionSignature(const Type& result_type)
{
  std::vector<Type> params;

  const Token leftPar = unsafe_read();
  
  {
    ParserParenView paren_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      {
        ListView param_view{ m_buffer, m_view, m_index };
        Type t = parseType();
        params.push_back(t);
      }

      if (!atEnd())
        read(TokenType::Comma);
    }
  }

  read(TokenType::RightPar);

  return Type::function(result_type, std::move(params));
}

Name RestrictedParser::parseName()
{
  Token t = peek();

  switch (t.type().value())
  {
  case TokenType::Void:
  case TokenType::Bool:
  case TokenType::Char:
  case TokenType::Int:
  case TokenType::Float:
  case TokenType::Double:
  case TokenType::Auto:
  case TokenType::This:
    return Name(std::make_shared<details::Identifier>(unsafe_read().to_string()));
  case TokenType::Operator:
    return readOperatorName();
  case TokenType::UserDefinedName:
    return readUserDefinedName();
  default:
    break;
  }

  throw std::runtime_error{ "expected identifier" };
}

Name RestrictedParser::readOperatorName()
{
  //if (!testOption(ParseOperatorName))
  //  throw SyntaxError{ ParserError::UnexpectedToken, errors::UnexpectedToken{peek(), Token::Invalid} };

  Token opkw = read();
  if (atEnd())
    throw std::runtime_error{ "unexpected end of input" };

  Token op = peek();
  if (op.type().value() & TokenCategory::OperatorToken)
  {
    return Name(std::make_shared<details::OverloadedOperatorName>(unsafe_read().to_string()));
  }
  else if (op.type() == TokenType::LeftPar)
  {
    const Token lp = read();
    const Token rp = read(TokenType::RightPar);

    if (pos(lp) + 1 != pos(rp))
      throw std::runtime_error{ "unexpected blank space between '(' and ')'" };

    return Name(std::make_shared<details::OverloadedOperatorName>("()"));
  }
  else if (op.type() == TokenType::LeftBracket)
  {
    const Token lb = read();
    const Token rb = read(TokenType::RightBracket);

    if (pos(lb) + 1 != pos(rb))
      throw std::runtime_error{ "unexpected blank space between '[' and ']'" };

    return Name(std::make_shared<details::OverloadedOperatorName>("[]"));
  }
  else if (op.type() == TokenType::StringLiteral)
  {
    if (op.text().size() != 2)
      throw std::runtime_error{ "unexpected \"\"" };

    unsafe_read();
    auto suffix_name = parseName();

    return Name(std::make_shared<details::LiteralOperatorName>(suffix_name.toString()));
  }
  else if (op.type() == TokenType::UserDefinedLiteral)
  {
    op = unsafe_read();
    const std::string str = op.to_string();

    if (str.find("\"\"") != 0)
      throw std::runtime_error{ "unexpected \"\"" };

    std::string suffix_name{ str.begin() + 2, str.end() };
    return Name(std::make_shared<details::LiteralOperatorName>(std::move(suffix_name)));
  }

  throw std::runtime_error{ "expected operator symbol" };
}

Name RestrictedParser::readUserDefinedName()
{
  const Token base = read();

  if (base.type() != TokenType::UserDefinedName)
    throw std::runtime_error{ "expected identifier" };

  Name ret = Name(std::make_shared<details::Identifier>(base.to_string()));

  if (atEnd())
    return ret;

  Token t = peek();

  if (t.type() == TokenType::LeftAngle)
  {
    const auto savepoint = pos();

    try
    {
      ret = readTemplateArguments(ret);
    }
    catch (const std::runtime_error&)
    {
      seek(savepoint);
      return ret;
    }
  }

  if (atEnd())
    return ret;

  t = peek();

  if (t.type() == TokenType::ScopeResolution)
  {
    std::vector<std::shared_ptr<const details::IName>> identifiers;
    identifiers.push_back(ret.impl());

    while (t.type() == TokenType::ScopeResolution)
    {
      read();

      identifiers.push_back(parseName().impl());

      if (atEnd())
        break;
      else
        t = peek();
    }

    ret = Name(details::QualifiedName::make(identifiers.begin(), identifiers.end()));
  }

  return ret;
}

Name RestrictedParser::readTemplateArguments(const Name base)
{
  const Token leftangle = read();

  std::vector<TemplateArgument> params;

  bool need_read_right_angle = true;

  {
    TemplateAngleView main_view{ m_buffer, m_view, m_index };
    // If a '>>' was splitted in two by the TemplateAngleView, its 
    // destructor will implicitely consume the second '>' so there is no 
    // need to read() it afterward.
    need_read_right_angle = !main_view.split_right_shift;

    while (!atEnd())
    {
      {
        ListView sub_view{ m_buffer, m_view, m_index, false };
        params.push_back(parseDelimitedTemplateArgument());
      }

      if (!atEnd())
        read(TokenType::Comma);
    }
  }

  if(need_read_right_angle)
    read(TokenType::RightAngle);

  return Name{ std::make_shared<details::TemplateName>(base.toString(), std::move(params)) };
}

std::shared_ptr<Function> RestrictedParser::parseFunctionSignature()
{
  int specifiers = FunctionSpecifier::None;

  while (peek() == TokenType::Virtual || peek() == TokenType::Static 
    || peek() == TokenType::Inline || peek() == TokenType::Constexpr
    || peek() == TokenType::Explicit)
  {
    if (unsafe_peek() == TokenType::Virtual)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Virtual;
    }
    else if(unsafe_peek() == TokenType::Static)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Static;
    }
    else if (unsafe_peek() == TokenType::Inline)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Inline;
    }
    else if (unsafe_peek() == TokenType::Constexpr)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Constexpr;
    }
    else if (unsafe_peek() == TokenType::Explicit)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Explicit;
    }
  }

  Type return_type = parseType();
  Name fun_name = parseName();

  auto ret = std::make_shared<Function>(fun_name.toString());
  ret->specifiers = specifiers;
  ret->return_type = return_type;

  read(TokenType::LeftPar);

  {
    ParserParenView parameters_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      {
        constexpr bool ignore_angle = false;
        ListView param_view{ m_buffer, m_view,  m_index, ignore_angle };

        ret->parameters.push_back(parseFunctionParameter());
        ret->parameters.back()->weak_parent = ret;
      }

      if (!atEnd())
        read(TokenType::Comma);
    }
  }

  read(TokenType::RightPar);

  if(atEnd() || unsafe_peek() == TokenType::Semicolon || unsafe_peek() == TokenType::LeftBrace)
    return ret;

  Token tok = read();

  if (tok == TokenType::Const)
  {
    ret->specifiers |= FunctionSpecifier::Const;

    if (atEnd() || unsafe_peek() == TokenType::Semicolon || unsafe_peek() == TokenType::LeftBrace)
      return ret;

    tok = read();
  }

  if (tok == TokenType::Noexcept)
  {
    ret->specifiers |= FunctionSpecifier::Noexcept;

    if (atEnd() || unsafe_peek() == TokenType::Semicolon || unsafe_peek() == TokenType::LeftBrace)
      return ret;

    tok = read();
  }

  if (tok == TokenType::Override)
  {
    ret->specifiers |= FunctionSpecifier::Override;
  }
  else if (tok == TokenType::Final)
  {
    ret->specifiers |= FunctionSpecifier::Final;
  }
  else if (tok == TokenType::Eq)
  {
    tok = read();

    if (tok == TokenType::Zero)
    {
      ret->specifiers |= FunctionSpecifier::Pure;
    }
    else
    {
      throw std::runtime_error{ "expected = 0" };
    }
  }

  if (atEnd() || unsafe_peek() == TokenType::Semicolon || unsafe_peek() == TokenType::LeftBrace)
    return ret;
  else
    throw std::runtime_error{ "unexpected token" };
}

std::shared_ptr<Variable> RestrictedParser::parseVariable()
{
  int specifiers = VariableSpecifier::None;

  while (peek() == TokenType::Inline || peek() == TokenType::Static || peek() == TokenType::Constexpr)
  {
    if (unsafe_peek() == TokenType::Inline)
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Inline;
    }
    else if (unsafe_peek() == TokenType::Static)
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Static;
    }
    else
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Constexpr;
    }
  }

  Type type = parseType();
  Name name = parseName();

  auto ret = std::make_shared<Variable>(type, name.toString());
  ret->specifiers() = specifiers;

  if (atEnd() || peek() == TokenType::Semicolon)
    return ret;

  read(TokenType::Eq);

  ret->defaultValue() = parseExpression();

  return ret;
}

std::shared_ptr<Typedef> RestrictedParser::parseTypedef()
{
  auto decl = parseTypedefDecl();
  return std::static_pointer_cast<Typedef>(static_cast<TypedefDeclaration&>(*decl).entity_ptr);
}

std::shared_ptr<Macro> RestrictedParser::parseMacro()
{
  std::string name = read(TokenType::UserDefinedName).to_string();
  std::vector<std::string> params;

  if (atEnd())
    return std::make_shared<Macro>(name, std::move(params));

  read(TokenType::LeftPar);

  Token tok = read();

  for (;;)
  {
    if (tok.isIdentifier())
    {
      params.push_back(tok.to_string());

      tok = read();

      if (tok.type() == TokenType::Comma)
        tok = read();
      else if (tok.type() == TokenType::RightPar)
        break;
    }
    else if (tok.type() == TokenType::Dot)
    {
      read(TokenType::Dot);
      read(TokenType::Dot);

      params.push_back("...");

      read(TokenType::RightPar);
      break;
    }
    else
    {
      throw std::runtime_error{ "bad input to parseMacro" };
    }
  }

  return std::make_shared<Macro>(name, std::move(params));
}

Token RestrictedParser::read()
{
  if (m_index == m_buffer.size())
    throw std::runtime_error{ "Unexpected end of input" };

  return unsafe_read();
}

Token RestrictedParser::unsafe_read()
{
  assert(m_index < m_buffer.size());
  return m_buffer[m_index++];
}

Token RestrictedParser::peek() const
{
  if (atEnd())
    throw std::runtime_error{ "Unexpected end of input" };

  return unsafe_peek();
}

Token RestrictedParser::unsafe_peek() const
{
  assert(m_index < m_buffer.size()); 
  return m_buffer[m_index];
}

Token RestrictedParser::prev() const
{
  return m_buffer[m_index - 1];
}

bool RestrictedParser::isDiscardable(const Token& t) const
{
  return t == TokenType::MultiLineComment || t == TokenType::SingleLineComment;
}

Token RestrictedParser::read(TokenType::Value tokt)
{
  const Token tok = read();

  if (tok.type().value() != tokt)
    throw std::runtime_error{ "unexpected token" };

  return tok;
}

size_t RestrictedParser::pos() const
{
  return m_index;
}

size_t RestrictedParser::pos(const Token& tok) const
{
  return tok.text().data() - m_lexer.source().data();
}

void RestrictedParser::seek(size_t pos)
{
  m_index = pos;
}

void RestrictedParser::seekBegin()
{
  m_index = m_view.first;
}

void RestrictedParser::seekEnd()
{
  m_index = m_view.second;
}

std::string RestrictedParser::viewstring() const
{
  Token first = *(m_buffer.begin() + m_view.first);
  Token last = *(m_buffer.begin() + m_view.second - 1);

  return std::string(first.text().data(), last.text().data() + last.text().size());
}

std::string RestrictedParser::stringtoend() const
{
  Token first = m_buffer[m_index];
  Token last = *(m_buffer.begin() + m_view.second - 1);

  return std::string(first.text().data(), last.text().data() + last.text().size());
}

TemplateArgument RestrictedParser::parseDelimitedTemplateArgument()
{
  try
  {
    Type type = parseType();

    if (!atEnd())
      return seekEnd(), TemplateArgument{ viewstring() };

    return TemplateArgument{ type };
  }
  catch (const std::runtime_error & )
  {
    return seekEnd(), TemplateArgument{ viewstring() };
  }
}

std::shared_ptr<TemplateParameter> RestrictedParser::parseDelimitedTemplateParameter()
{
  Token tok = peek();

  if (tok == TokenType::Typename || tok == TokenType::Class)
  {
    unsafe_read();

    if (atEnd())
      return std::make_shared<TemplateParameter>("");

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return std::make_shared<TemplateParameter>(std::move(name));

    if (peek() != TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    Type default_value = parseType();

    if(!atEnd())
      throw std::runtime_error{ "expected end of input" };

    return std::make_shared<TemplateParameter>(std::move(name), default_value);
  }
  else
  {
    Type type = parseType();

    if (atEnd())
      return std::make_shared<TemplateParameter>(type, "");

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return std::make_shared<TemplateParameter>(type, std::move(name));

    if (peek() != TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    unsafe_read();

    std::string default_val = "";

    while (!atEnd())
    {
      default_val += unsafe_read().to_string() + " ";
    }

    default_val.pop_back();

    return std::make_shared<TemplateParameter>(type, std::move(name), std::move(default_val));
  }
}

std::shared_ptr<Function::Parameter> RestrictedParser::parseFunctionParameter()
{
  const Type param_type = parseType();

  if (atEnd())
    return std::make_shared<Function::Parameter>(param_type, "");

  std::string name;

  if (peek().isIdentifier())
    name = read().to_string();

  if (atEnd())
    return std::make_shared<Function::Parameter>(param_type, std::move(name));

  read(TokenType::Eq);

  std::string default_value = stringtoend();
  seekEnd();

  return std::make_shared<Function::Parameter>(param_type, std::move(name), Expression{ std::move(default_value) });
}

cxx::INode& RestrictedParser::curNode()
{
  return *m_program_stack.back();
}

void RestrictedParser::astWrite(std::shared_ptr<AstNode> n)
{
  cxx::AstNode& cur = *m_ast_stack.back();
  cur.append(n);
}

void RestrictedParser::localizeParentize(const std::shared_ptr<AstNode>& node, const Token& tok)
{
  node->sourcerange.file = m_current_file;
  node->sourcerange.begin.line = tok.line();
  node->sourcerange.begin.column = tok.col();
  node->sourcerange.end = node->sourcerange.begin;
  node->sourcerange.end.column += static_cast<int>(tok.text().size());

  parentize(node);
}

void RestrictedParser::localizeParentize(const std::shared_ptr<AstNode>& node, const Token& first, const Token& last)
{
  node->sourcerange.file = m_current_file;
  node->sourcerange.begin.line = first.line();
  node->sourcerange.begin.column = first.col();
  node->sourcerange.end.line = last.line();
  node->sourcerange.end.column = last.col() + static_cast<int>(last.text().size());

  parentize(node);
}

void RestrictedParser::localize(const std::shared_ptr<AstNode>& node, const Token& first, const Token& last)
{
  node->sourcerange.file = m_current_file;
  node->sourcerange.begin.line = first.line();
  node->sourcerange.begin.column = first.col();
  node->sourcerange.end.line = last.line();
  node->sourcerange.end.column = last.col() + static_cast<int>(last.text().size());
}

void RestrictedParser::parentize(const std::shared_ptr<AstNode>& node)
{
  node->weak_parent = m_ast_stack.back();
}

void RestrictedParser::bind(const std::shared_ptr<AstNode>& astnode, const std::shared_ptr<INode>& n)
{
  if(program())
    program()->astmap[n.get()] = astnode;
}

Statement RestrictedParser::parseStatement()
{
  Token tok = peek();

  switch (tok.type().value())
  {
  case TokenType::LeftBrace:
    return parseCompoundStatement();
  case TokenType::Semicolon:
    return parseNullStatement();
  case TokenType::Break:
    return parseBreakStatement();
  case TokenType::Class:
  case TokenType::Struct:
    return parseClassDecl();
  case TokenType::Do:
    return parseDoWhileLoop();
  case TokenType::Bool:
  case TokenType::Char:
  case TokenType::Const: 
  case TokenType::Constexpr:
  case TokenType::Double:
  case TokenType::Explicit:
  case TokenType::Float:
  case TokenType::Inline:
  case TokenType::Int:
  case TokenType::Mutable:
  case TokenType::Static:
  case TokenType::Typename:
  case TokenType::Void:
  case TokenType::UserDefinedName:
    /* could be a function-decl, var-decl or even an expression */
    break;
  case TokenType::Continue:
    return parseContinueStatement();
  case TokenType::Default:
  case TokenType::Delete:
  case TokenType::Else:
  case TokenType::False:
  case TokenType::Final:
  case TokenType::Noexcept:
  case TokenType::Override:
  case TokenType::This:
  case TokenType::True:
  case TokenType::Typeid:
    throw std::runtime_error{ "unexpected keyword" };
  case TokenType::Enum:
    return parseEnumDecl();
  case TokenType::For:
    return parseForLoop();
  case TokenType::Export:
  case TokenType::Friend:
    throw std::runtime_error{ "Not implemented" };
  case TokenType::If:
    return parseIf();
  case TokenType::Import:
  case TokenType::Namespace:
    return parseNamespaceDecl();
  case TokenType::Operator:
    throw std::runtime_error{ "Not implemented" };
  case TokenType::Private:
  case TokenType::Protected:
  case TokenType::Public:
    return parseAccessSpecifier();
  case TokenType::Return:
    return parseReturnStatement();
  case TokenType::Template:
    throw std::runtime_error{ "Not implemented" };
  case TokenType::Throw:
    return parseExpressionStatement();
  case TokenType::Try:
    return parseTryBlock();
  case TokenType::Typedef:
    return parseTypedefDecl();
  case TokenType::Using:
    return parseUsingDecl();
  case TokenType::Virtual:
    return Statement(parseFunctionDecl());
  case TokenType::While:
    return parseWhile();
  case TokenType::Catch:
    throw std::runtime_error{ "unexpected token" };
  }

  NodeKind nk = detectStatement();

  if (nk == NodeKind::ExpressionStatement)
  {
    return parseExpressionStatement();
  }
  else if (nk == NodeKind::FunctionDeclaration)
  {
    return Statement(parseFunctionDecl());
  }
  else if (nk == NodeKind::VariableDeclaration)
  {
    return Statement(parseVarDecl());
  }
  else
  {
    throw std::runtime_error{ "not implemented" };
  }
}

static bool is_function_or_var_specifier(cxx::parsers::Token tok)
{
  return tok == TokenType::Inline
    || tok == TokenType::Static
    || tok == TokenType::Mutable;
}

NodeKind RestrictedParser::detectStatement()
{
  RAIIGuard<size_t> index_guard{ m_index };

  if(peek() == TokenType::Virtual || peek() == TokenType::Override || peek() == TokenType::Explicit)
    return NodeKind::FunctionDeclaration;

  bool can_be_expr = m_parsing_function_body;

  while (is_function_or_var_specifier(peek()))
  {
    can_be_expr = false;
    read();
  }

  try 
  {
    Type rt_or_vartype = parseType();
    Name n = parseName();

    if (peek() == TokenType::LeftBrace || peek() == TokenType::Semicolon || peek() == TokenType::Eq)
    {
      return NodeKind::VariableDeclaration;
    }
    else if (peek() == TokenType::LeftPar)
    {
      read();

      // can still be var or fun decl
      size_t right_par_index = 0;
      {
        ParserParenView subview{ m_buffer, m_view, m_index };
        right_par_index = m_view.second;
      }

      if (m_buffer[right_par_index + 1] == TokenType::Semicolon)
      {
        // can still be both

        if (rt_or_vartype.toString() == "void")
          return NodeKind::FunctionDeclaration;

        // @TODO
        throw std::runtime_error{ "not implemented" };
      }
      else
      {
        return NodeKind::FunctionDeclaration;
      }
    }
    else
    {
      if(can_be_expr)
        return NodeKind::ExpressionStatement;
    }
  }
  catch (std::runtime_error&)
  {
    if(can_be_expr)
      return NodeKind::ExpressionStatement;
  }

  return NodeKind::UnexposedStatement;
}

Statement RestrictedParser::parseFunctionBody(std::shared_ptr<cxx::Function> f)
{
  if (skip_function_bodies)
  {
    read(TokenType::LeftBrace);

    {
      ParserBraceView brace_view{ m_buffer, m_view, m_index };
      m_index = m_view.second;
    }

    read(TokenType::RightBrace);

    return Statement();
  }

  Token leftbrace = read(TokenType::LeftBrace);

  auto astnode = std::make_shared<FunctionBody>(f);

  {
    RAIIVectorSharedGuard<cxx::AstNode> guard{ m_ast_stack, astnode };
    ParserBraceView brace_view{ m_buffer, m_view, m_index };
    RAIIGuard<bool> parse_func_guard{ m_parsing_function_body };
    m_parsing_function_body = true;

    while (!atEnd())
    {
      Statement stmt = parseStatement();
      astnode->statements.push_back(stmt);
    }
  }

  Token rightbrace = read(TokenType::RightBrace);

  localizeParentize(astnode, leftbrace, rightbrace);

  return { astnode };
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseNullStatement()
{
  auto result = std::make_shared<NullStatement>();
  localizeParentize(result, read());
  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseAccessSpecifier()
{
  auto result = std::make_shared<AccessSpecifierDeclaration>();

  Token aspec = read();
  Token colon = read();
  localizeParentize(result, aspec, colon);

  if (aspec.type() == TokenType::Public)
    result->value = AccessSpecifier::PUBLIC;
  else if (aspec.type() == TokenType::Protected)
    result->value = AccessSpecifier::PROTECTED;
  else if (aspec.type() == TokenType::Private)
    result->value = AccessSpecifier::PRIVATE;

  m_access_specifier = result->value;

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseBreakStatement()
{
  auto result = std::make_shared<BreakStatement>();
  localizeParentize(result, read());
  read(TokenType::Semicolon);
  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseCaseStatement()
{
  auto result = std::make_shared<CaseStatement>();

  Token casekw = read(TokenType::Case);
  
  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    ParserSentinelView view{ m_buffer, m_view, m_index, TokenType::Colon };

    result->value = parseExpression();
  }

  read(TokenType::Colon);

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    result->stmt = parseStatement();
  }

  localizeParentize(result, casekw, prev());

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseCatchStatement()
{
  auto stmt = std::make_shared<CatchStatement>();
  RaiiAstLocator ast_locator{ this, stmt };

  read(TokenType::Catch);

  read(TokenType::LeftPar);

  {
    ParserParenView paren_view{ m_buffer, m_view, m_index };
    stmt->var = parseParameterDecl();
  }

  read(TokenType::RightPar);

  stmt->body = parseStatement();

  return stmt;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseContinueStatement()
{
  auto result = std::make_shared<ContinueStatement>();
  localizeParentize(result, read());
  read(TokenType::Semicolon);
  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseCompoundStatement()
{
  auto result = std::make_shared<CompoundStatement>();
  
  Token leftbrace = read(TokenType::LeftBrace);

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    ParserBraceView paren_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      result->statements.push_back(parseStatement());
    }
  }

  Token rightbrace = read(TokenType::RightBrace);

  localizeParentize(result, leftbrace, rightbrace);

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseDefaultStatement()
{
  auto result = std::make_shared<DefaultStatement>();

  Token defaultkw = read(TokenType::Default);
  read(TokenType::Colon);

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    result->stmt = parseStatement();
  }

  localizeParentize(result, defaultkw, prev());

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseDoWhileLoop()
{
  auto result = std::make_shared<DoWhileLoop>();

  RaiiAstLocator astguard{ this, result };

  read(TokenType::Do);

  result->body = parseStatement();

  read(TokenType::While);

  read(TokenType::LeftPar);

  {
    ParserParenView parenview{ m_buffer, m_view, m_index };

    result->condition = parseExpression();
  }

  read(TokenType::RightPar);

  read(TokenType::Semicolon);

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseExpressionStatement()
{
  auto result = std::make_shared<ExpressionStatement>();

  RaiiAstLocator astguard{ this, result };

  {
    ParserSemicolonView view{ m_buffer, m_view, m_index };
    result->expr = parseExpression();
  }

  read(TokenType::Semicolon);

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseForLoop()
{
  Token forkw = read(TokenType::For);

  Token leftpar = read(TokenType::LeftPar);

  bool is_for_range = false;

  {
    ParserParenView paren_view{ m_buffer, m_view, m_index };

    size_t nb_semicolon = [this]() -> size_t {
      size_t n = 0;
      for (size_t i = m_index; i < m_view.second; ++i)
      {
        if (m_buffer.at(i) == TokenType::Semicolon)
          n += 1;
      }
      return n;
    }();

    size_t nb_colon = [this]() -> size_t {
      size_t n = 0;
      for (size_t i = m_index; i < m_view.second; ++i)
      {
        if (m_buffer.at(i) == TokenType::Colon)
          n += 1;
      }
      return n;
    }();

    is_for_range = nb_colon >= 1 && nb_semicolon < 2;
  }

  if (is_for_range)
  {
    auto result = std::make_shared<ForRange>();

    {
      ParserColonView colon_view{ m_buffer, m_view, m_index };
      result->variable = parseStatement();
    }

    read(TokenType::Colon);

    result->container = parseExpression();

    read(TokenType::RightPar);

    result->body = parseStatement();

    localizeParentize(result, forkw, prev());

    return result;
  }
  else
  {
    auto result = std::make_shared<ForLoop>();

    {
      ParserSemicolonView semicolon_view{ m_buffer, m_view, m_index };
      m_view.second += 1;
      result->init = parseStatement();
    }
 
    {
      ParserSemicolonView semicolon_view{ m_buffer, m_view, m_index };
      result->condition = parseExpression();
    }

    read(TokenType::Semicolon);

    result->iter = parseExpression();

    read(TokenType::RightPar);

    result->body = parseStatement();

    localizeParentize(result, forkw, prev());

    return result;
  }
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseIf()
{
  Token ifkw = read(TokenType::If);

  Token leftpar = read(TokenType::LeftPar);

  auto result = std::make_shared<IfStatement>();

  {
    ParserParenView paren_view{ m_buffer, m_view, m_index };

    result->condition = parseExpression();
  }

  Token rightpar = read(TokenType::RightPar);

  result->body = parseStatement();

  if (!atEnd() && peek() == TokenType::Else)
  {
    Token elsekw = read(TokenType::Else);
    result->else_clause = parseStatement();
  }

  localizeParentize(result, ifkw, prev());

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseReturnStatement()
{
  Token returnkw = read(TokenType::Return);

  auto result = std::make_shared<ReturnStatement>();

  if (peek() != TokenType::Semicolon)
  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    ParserSemicolonView semicolon_view{ m_buffer, m_view, m_index };

    result->expr = parseExpression();
  }

  Token semicolon = read();

  localizeParentize(result, returnkw, semicolon);

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseSwitchStatement()
{
  auto result = std::make_shared<SwitchStatement>();

  Token switchtok = read();

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    read(TokenType::LeftPar);

    {
      ParserParenView paren_view{ m_buffer, m_view, m_index };
      result->value = parseExpression();
    }

    read(TokenType::RightPar);

    result->body = parseStatement();
  }

  localizeParentize(result, switchtok, prev());

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseTryBlock()
{
  auto stmt = std::make_shared<TryBlock>();

  RaiiAstLocator ast_locator{ this, stmt };

  read(TokenType::Try);

  stmt->body = parseStatement();

  while (!atEnd() && peek() == TokenType::Catch)
  {
    stmt->handlers.push_back(parseCatchStatement());
  }

  return stmt;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseWhile()
{
  Token whilekw = read(TokenType::While);

  Token leftpar = read(TokenType::LeftPar);

  auto result = std::make_shared<WhileLoop>();

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, result };

    {
      ParserParenView paren_view{ m_buffer, m_view, m_index };

      result->condition = parseExpression();
    }

    Token rightpar = read(TokenType::RightPar);

    result->body = parseStatement();
  }

  localizeParentize(result, whilekw, prev());

  return result;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseUnexposedStatement()
{
  throw std::runtime_error{ "not implemented" };
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseDecl()
{
  if (peek() == TokenType::Class || peek() == TokenType::Struct)
    return parseClassDecl();
  else if (unsafe_peek() == TokenType::Enum)
    return parseEnumDecl();
  else if (unsafe_peek() == TokenType::Typedef)
    return parseTypedefDecl();
  else if (unsafe_peek() == TokenType::Using)
    return parseUsingDecl();

  throw std::runtime_error{ "not implemented" };
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseClassDecl()
{
  Token classkw = read(); // read 'class' or 'struct'

  std::string cname = parseName().toString();
  bool is_template = false;

  auto entity = [&]() -> std::shared_ptr<Class> {
    if (curNode().is<Namespace>())
    {
      auto& ns = static_cast<Namespace&>(curNode());

      if (is_template)
        return ns.getOrCreate<ClassTemplate>(cname, std::vector<std::shared_ptr<TemplateParameter>>(), std::move(cname));
      else
        return ns.getOrCreate<Class>(cname, std::move(cname));
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      std::shared_ptr<Class> result = !is_template ? (std::make_shared<Class>(std::move(cname), cla.shared_from_this())) :
        (std::make_shared<ClassTemplate>(std::vector<std::shared_ptr<TemplateParameter>>(), std::move(cname), cla.shared_from_this()));
      result->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(result);
      return result;
    }
  }();

  auto decl = std::make_shared<ClassDeclaration>(entity);

  if (peek() == TokenType::Semicolon)
  {
    localizeParentize(decl, classkw, read());
    return decl;
  }

  m_access_specifier = classkw == TokenType::Class ? cxx::AccessSpecifier::PRIVATE : cxx::AccessSpecifier::PUBLIC;

  read(TokenType::LeftBrace);

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, decl };
    RAIIVectorSharedGuard<cxx::INode> entity_guard{ m_program_stack, entity };

    ParserBraceView brace_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      Statement stmt = parseStatement();
      decl->childvec.push_back(cxx::to_ast_node(stmt));
    }
  }

  read(TokenType::RightBrace);
  Token endingsemicolon = read(TokenType::Semicolon);

  bind(decl, entity);
  localizeParentize(decl, classkw, endingsemicolon);

  return decl;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseEnumDecl()
{
  throw std::runtime_error{ "not implemented" };
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseNamespaceDecl()
{
  Token namespacekw = read(); 

  auto decl = std::make_shared<NamespaceDeclaration>();

  std::string nsname = parseName().toString();

  if (!curNode().is<Namespace>())
    throw std::runtime_error{ "direct parent of namespace must be a namespace" };

  auto ns = static_cast<Namespace&>(curNode()).getOrCreateNamespace(nsname);

  decl->entity_ptr = ns;

  read(TokenType::LeftBrace);

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, decl };
    RAIIVectorSharedGuard<cxx::INode> entity_guard{ m_program_stack, ns };

    ParserBraceView brace_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      Statement stmt = parseStatement();
      decl->childvec.push_back(cxx::to_ast_node(stmt));
    }
  }

  Token rbrace = read(TokenType::RightBrace);

  localizeParentize(decl, namespacekw, rbrace);

  return decl;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseParameterDecl()
{
  auto decl = std::make_shared<ParameterDeclaration>();
  RaiiAstLocator ast_locator{ this, decl };

  parseType();
  
  if (atEnd())
    return decl;

  if (peek() == TokenType::UserDefinedName)
  {
    parseName();

    if (atEnd())
      return decl;
  }

  if (peek() == TokenType::Eq)
  {
    read(TokenType::Eq);

    parseExpression();
  }

  return decl;
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseUsingDecl()
{
  throw std::runtime_error{ "not implemented" };
}

std::shared_ptr<cxx::IStatement> RestrictedParser::parseTypedefDecl()
{
  Token typedefkw = read(TokenType::Typedef);

  auto decl = std::make_shared<TypedefDeclaration>();

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, decl };

    Type t = parseType();

    Token name = read();

    if (!name.isIdentifier())
      throw std::runtime_error{ "Unexpected identifier while parsing typedef" };

    auto entity = std::make_shared<Typedef>(t, name.to_string());
  
    decl->entity_ptr = entity;
    bind(decl, entity);
  }

  Token semicolon = read(TokenType::Semicolon);

  localizeParentize(decl, typedefkw, semicolon);

  return decl;
}

// @TODO: avoid this awful copy/paste
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

static std::shared_ptr<cxx::Function> find_equiv_func(cxx::INode& current_node, const cxx::Function& func)
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
  else if (current_node.is<cxx::Namespace>())
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

  if (func.body.isNull() && !new_one.body.isNull())
  {
    func.body = new_one.body;
  }
}

std::shared_ptr<cxx::FunctionDeclaration> RestrictedParser::parseFunctionDecl()
{
  auto decl = std::make_shared<FunctionDeclaration>();

  Token first_tok = peek();

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, decl };

    std::shared_ptr<Function> fun = parseFunctionSignature();

    std::shared_ptr<Function> equiv = find_equiv_func(curNode(), *fun);

    if (equiv)
    {
      update_func(*equiv, *fun);
      fun = equiv;
    }
    else
    {
      fun->weak_parent = std::static_pointer_cast<cxx::IEntity>(curNode().shared_from_this());

      if (curNode().is<Namespace>())
      {
        static_cast<Namespace&>(curNode()).entities.push_back(fun);
      }
      else
      {
        Class& cla = static_cast<Class&>(curNode());
        fun->setAccessSpecifier(m_access_specifier);
        cla.members.push_back(fun);
      }
    }

    decl->entity_ptr = fun;
    bind(decl, fun);

    if (peek() == TokenType::Semicolon)
    {
      read();
    }
    else
    {
      Statement body = parseFunctionBody(fun);
      decl->childvec.push_back(cxx::to_ast_node(body));
      fun->body = body;
    }
  }

  localizeParentize(decl, first_tok, prev());
  return decl;
}

std::shared_ptr<cxx::VariableDeclaration> RestrictedParser::parseVarDecl()
{
  auto decl = std::make_shared<VariableDeclaration>();

  Token first_tok = peek();

  {
    RAIIVectorSharedGuard<cxx::AstNode> ast_guard{ m_ast_stack, decl };
    ParserSemicolonView semicolon_view{ m_buffer, m_view, m_index };

    std::shared_ptr<Variable> var = parseVariable();

    var->weak_parent = std::static_pointer_cast<cxx::IEntity>(curNode().shared_from_this());

    if (curNode().is<Namespace>())
    {
      static_cast<Namespace&>(curNode()).entities.push_back(var);
    }
    else
    {
      Class& cla = static_cast<Class&>(curNode());
      var->setAccessSpecifier(m_access_specifier);
      cla.members.push_back(var);
    }

    decl->entity_ptr = var;
    bind(decl, var);
  }

  localizeParentize(decl, first_tok, read(TokenType::Semicolon));
  return decl;
}

Expression RestrictedParser::parseExpression()
{
  std::string default_val = stringtoend();

  if (default_val.back() == ';')
    default_val.pop_back();

  Expression result{ std::move(default_val) };

  m_index = m_view.second;

  return result;
}

} // namespace parsers

} // namespace cxx

