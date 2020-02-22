// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/restricted-parser.h"

#include "cxx/name_p.h"

#include <map>

namespace cxx
{

namespace parsers
{

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

class TemplateAngleView : public ParserViewRAII
{
public:
  TemplateAngleView(const std::vector<Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
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

  {
    TemplateAngleView main_view{ m_buffer, m_view, m_index };

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

  read(TokenType::RightAngle);


  return Name{ std::make_shared<details::TemplateName>(base.toString(), std::move(params)) };
}

std::shared_ptr<Function> RestrictedParser::parseFunctionSignature()
{
  int specifiers = FunctionSpecifier::None;

  while (peek() == TokenType::Virtual || peek() == TokenType::Static)
  {
    if (unsafe_peek() == TokenType::Virtual)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Virtual;
    }
    else
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Static;
    }
  }

  Type return_type = parseType();
  Name fun_name = parseName();

  auto ret = std::make_shared<Function>(fun_name.toString());
  ret->specifiers() = specifiers;
  ret->returnType() = return_type;

  read(TokenType::LeftPar);

  {
    ParserParenView parameters_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      {
        ListView param_view{ m_buffer, m_view,  m_index };

        ret->parameters().push_back(parseFunctionParameter());
      }

      if (!atEnd())
        read(TokenType::Comma);
    }

  }

  read(TokenType::RightPar);

  if(atEnd())
    return ret;

  Token tok = read();

  if (tok == TokenType::Semicolon)
    return ret;

  if (tok == TokenType::Const)
    ret->specifiers() |= FunctionSpecifier::Const;

  if (atEnd())
    return ret;

  read(TokenType::Semicolon);

  return ret;
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

Token RestrictedParser::peek()
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

TemplateParameter RestrictedParser::parseDelimitedTemplateParameter()
{
  Token tok = peek();

  if (tok == TokenType::Typename || tok == TokenType::Class)
  {
    unsafe_read();

    if (atEnd())
      return TemplateParameter{ "" };

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return TemplateParameter{ std::move(name) };

    if (peek() != TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    Type default_value = parseType();

    if(!atEnd())
      throw std::runtime_error{ "expected end of input" };

    return TemplateParameter{ std::move(name), default_value };
  }
  else
  {
    Type type = parseType();

    if (atEnd())
      return TemplateParameter{ type, "" };

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return TemplateParameter{ type, std::move(name) };

    if (peek() != TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    unsafe_read();

    std::string default_val = "";

    while (!atEnd())
    {
      default_val += unsafe_read().to_string() + " ";
    }

    default_val.pop_back();

    return TemplateParameter{ type, std::move(name), std::move(default_val) };
  }
}

Function::Parameter RestrictedParser::parseFunctionParameter()
{
  Function::Parameter result;
  result.type = parseType();

  if (atEnd())
    return result;

  if (peek().isIdentifier())
    result.name = read().to_string();

  if (atEnd())
    return result;

  read(TokenType::Eq);

  result.default_value = stringtoend();
  seekEnd();

  return result;
}

} // namespace parsers

} // namespace cxx

