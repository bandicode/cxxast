// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/parsers/lexer.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace cxx
{

namespace parsers
{

class LexerGuard
{
private:
  Lexer& m_lex;
  size_t m_pos;
  int m_line;
  int m_col;

public:
  LexerGuard(Lexer* lex)
    : m_lex(*lex),
      m_pos(lex->pos()),
      m_line(lex->line()),
      m_col(lex->col())
  {

  }

  ~LexerGuard()
  {
    if (std::uncaught_exception())
    {
      m_lex.m_pos = m_pos;
      m_lex.m_line = m_line;
      m_lex.m_col = m_col;
    }
  }
};

Lexer::Lexer(const std::string* src)
  : m_source(src),
    m_chars(src->data())
{

}

const std::string& Lexer::source() const
{
  return *m_source;
}

void Lexer::start()
{
  consumeDiscardable();
}

Token Lexer::read()
{
  if (this->atEnd())
    throw std::runtime_error{ "Lexer::read() : reached end of input" };

  LexerGuard guard{ this };

  size_t p = pos();

  char c = readChar();
  auto ct = ctype(c);
  
  Token result;
  switch (ct)
  {
  case Digit:
    result = readNumericLiteral(p);
    break;
  case DoubleQuote:
    result = readStringLiteral(p);
    break;
  case SingleQuote:
    result = readCharLiteral(p);
    break;
  case Letter:
  case Underscore:
    result = readIdentifier(p);
    break;
  case LeftPar:
    result = create(p, 1, TokenType::LeftPar);
    break;
  case RightPar:
    result = create(p, 1, TokenType::RightPar);
    break;
  case LeftBrace:
    result = create(p, 1, TokenType::LeftBrace);
    break;
  case RightBrace:
    result = create(p, 1, TokenType::RightBrace);
    break;
  case LeftBracket:
    result = create(p, 1, TokenType::LeftBracket);
    break;
  case RightBracket:
    result = create(p, 1, TokenType::RightBracket);
    break;
  case Semicolon:
    result = create(p, 1, TokenType::Semicolon);
    break;
  case Colon:
    result = readColonOrColonColon(p);
    break;
  case QuestionMark:
    result = create(p, 1, TokenType::QuestionMark);
    break;
  case Comma:
    result = create(p, 1, TokenType::Comma);
    break;
  case Dot:
    result = create(p, 1, TokenType::Dot);
    break;
  case Punctuator:
    result = readFromPunctuator(p);
    break;
  default:
    throw std::runtime_error{ "Lexer::read() : Unexpected input char" };
  }

  consumeDiscardable();
  return result;
}

bool Lexer::atEnd() const
{
  return m_pos == m_source->length();
}

size_t Lexer::pos() const
{
  return m_pos;
}

int Lexer::line() const
{
  return m_line;
}

int Lexer::col() const
{
  return m_col;
}

void Lexer::seek(size_t pos)
{
  if (pos > m_source->length())
  {
    seek(m_source->length());
  }
  else if (pos < m_pos)
  {
    // @TODO: there is a more efficient way to do that!
    m_pos = 0;
    m_line = 0;
    m_col = 0;
    seek(pos);
  }
  else if (pos > m_pos)
  {
    while (m_pos < pos)
      discardChar();

    consumeDiscardable();
  }
}

void Lexer::reset(const std::string* src)
{
  m_source = src;
  m_chars = src->data();
  m_pos = 0;
  m_line = 0;
  m_col = 0;
  consumeDiscardable();
}

char Lexer::readChar()
{
  if (atEnd())
    throw std::runtime_error{ "Lexer::readChar() : end of input" };

  ++m_col;
  return *(m_chars + m_pos++);
}

void Lexer::discardChar() noexcept
{
  char c = *(m_chars + m_pos++);

  if (c == '\n')
  {
    ++m_line;
    m_col = 0;
  }
  else
  {
    ++m_col;
  }
}

char Lexer::charAt(size_t pos)
{
  return m_chars[pos];
}

char Lexer::currentChar() const
{
  return *(m_chars + m_pos);
}

void Lexer::consumeDiscardable()
{
  while (!atEnd() && isDiscardable(peekChar()))
    discardChar();
}

Token Lexer::create(size_t pos, size_t length, TokenType type)
{
  return Token{ type, StringView(m_chars + pos, length), m_line, static_cast<int>(m_col - length) };
}

Token Lexer::create(size_t pos, TokenType type)
{
  return Token{ type, StringView(m_chars + pos, this->pos() - pos), m_line, static_cast<int>(m_col - (this->pos() - pos)) };
}

Lexer::CharacterType Lexer::ctype(char c)
{
  static const CharacterType map[] = {
    Invalid, // NUL    (Null char.)
    Invalid, // SOH    (Start of Header)
    Invalid, // STX    (Start of Text)
    Invalid, // ETX    (End of Text)
    Invalid, // EOT    (End of Transmission)
    Invalid, // ENQ    (Enquiry)
    Invalid, // ACK    (Acknowledgment)
    Invalid, // BEL    (Bell)
    Invalid, //  BS    (Backspace)
    Tabulation, //  HT    (Horizontal Tab)
    LineBreak, //  LF    (Line Feed)
    Invalid, //  VT    (Vertical Tab)
    Invalid, //  FF    (Form Feed)
    CarriageReturn, //  CR    (Carriage Return)
    Invalid, //  SO    (Shift Out)
    Invalid, //  SI    (Shift In)
    Invalid, // DLE    (Data Link Escape)
    Invalid, // DC1    (XON)(Device Control 1)
    Invalid, // DC2    (Device Control 2)
    Invalid, // DC3    (XOFF)(Device Control 3)
    Invalid, // DC4    (Device Control 4)
    Invalid, // NAK    (Negative Acknowledgement)
    Invalid, // SYN    (Synchronous Idle)
    Invalid, // ETB    (End of Trans. Block)
    Invalid, // CAN    (Cancel)
    Invalid, //  EM    (End of Medium)
    Invalid, // SUB    (Substitute)
    Invalid, // ESC    (Escape)
    Invalid, //  FS    (File Separator)
    Invalid, //  GS    (Group Separator)
    Invalid, //  RS    (Request to Send)(Record Separator)
    Invalid, //  US    (Unit Separator)
    Space, //  SP    (Space)
    Punctuator, //   !    (exclamation mark)
    DoubleQuote, //   "    (double quote)
    Punctuator, //   #    (number sign)
    Punctuator, //   $    (dollar sign)
    Punctuator, //   %    (percent)
    Punctuator, //   &    (ampersand)
    SingleQuote, //   '    (single quote)
    LeftPar, //   (    (left opening parenthesis)
    RightPar, //   )    (right closing parenthesis)
    Punctuator, //   *    (asterisk)
    Punctuator, //   +    (plus)
    Comma, //   ,    (comma)
    Punctuator, //   -    (minus or dash)
    Dot, //   .    (dot)
    Punctuator, //   /    (forward slash)
    Digit, //   0
    Digit, //   1
    Digit, //   2
    Digit, //   3
    Digit, //   4
    Digit, //   5
    Digit, //   6
    Digit, //   7
    Digit, //   8
    Digit, //   9
    Colon, //   :    (colon)
    Semicolon, //   ;    (semi-colon)
    Punctuator, //   <    (less than sign)
    Punctuator, //   =    (equal sign)
    Punctuator, //   >    (greater than sign)
    QuestionMark, //   ?    (question mark)
    Punctuator, //   @    (AT symbol)
    Letter, //   A
    Letter, //   B
    Letter, //   C
    Letter, //   D
    Letter, //   E
    Letter, //   F
    Letter, //   G
    Letter, //   H
    Letter, //   I
    Letter, //   J
    Letter, //   K
    Letter, //   L
    Letter, //   M
    Letter, //   N
    Letter, //   O
    Letter, //   P
    Letter, //   Q
    Letter, //   R
    Letter, //   S
    Letter, //   T
    Letter, //   U
    Letter, //   V
    Letter, //   W
    Letter, //   X
    Letter, //   Y
    Letter, //   Z
    LeftBracket, //   [    (left opening bracket)
    Punctuator, //   \    (back slash)
    RightBracket, //   ]    (right closing bracket)
    Punctuator, //   ^    (caret cirumflex)
    Underscore, //   _    (underscore)
    Punctuator, //   `
    Letter, //   a
    Letter, //   b
    Letter, //   c
    Letter, //   d
    Letter, //   e
    Letter, //   f
    Letter, //   g
    Letter, //   h
    Letter, //   i
    Letter, //   j
    Letter, //   k
    Letter, //   l
    Letter, //   m
    Letter, //   n
    Letter, //   o
    Letter, //   p
    Letter, //   q
    Letter, //   r
    Letter, //   s
    Letter, //   t
    Letter, //   u
    Letter, //   v
    Letter, //   w
    Letter, //   x
    Letter, //   y
    Letter, //   z
    LeftBrace, //   {    (left opening brace)
    Punctuator, //   |    (vertical bar)
    RightBrace, //   }    (right closing brace)
    Punctuator, //   ~    (tilde)
    Invalid, // DEL    (delete)
  };

  if(c <= 127)
    return map[c];
  return Other;
}

bool Lexer::isDiscardable(char c)
{
  return ctype(c) == Space || ctype(c) == LineBreak || ctype(c) == CarriageReturn || ctype(c) == Tabulation;
}


//template<>
//bool Lexer::checkAfter<Token::DecimalLiteral>() const
//{
//  return atEnd() || ctype(peekChar()) != Letter;
//}


Token Lexer::readNumericLiteral(size_t start)
{
  if (atEnd()) {
    if (charAt(start) == '0')
      return create(start, 1, TokenType::OctalLiteral);
    else
      return create(start, 1, TokenType::IntegerLiteral);
  }

  char c = peekChar();

  // Reading binary, octal or hexadecimal number
  // eg. : 0b00110111
  //       018
  //       0xACDBE
  if (charAt(start) == '0' && c != '.')
  {
    if (c == 'x') // hexadecimal
      return readHexa(start);
    else if (c == 'b') // binary
      return readBinary(start);
    else if (Lexer::isDigit(c))// octal
      return readOctal(start);
    else // it is zero
    {
      //if (!checkAfter<Token::DecimalLiteral>())
      //  throw std::runtime_error{ "Lexer::readNumericLiteral() : error" };
      return create(start, TokenType::OctalLiteral);
    }
  }

  return readDecimal(start);
}

//template<>
//bool Lexer::checkAfter<Token::HexadecimalLiteral>() const
//{
//  return atEnd() || ctype(peekChar()) != Letter;
//}

Token Lexer::readHexa(size_t start)
{
  const char x = readChar();
  assert(x == 'x');

  if (atEnd())  // input ends with '0x' -> error
    throw std::runtime_error{ "Lexer::readHexa() : unexpected end of input" };

  while (!atEnd() && Lexer::isHexa(peekChar()))
    readChar();

  if(pos() - start == 2/* || !checkAfter<Token::HexadecimalLiteral>()*/) // e.g. 0x+
    throw std::runtime_error{ "Lexer::readHexa() : unexpected end of input" };
  
  return create(start, TokenType::HexadecimalLiteral);
}

//template<>
//bool Lexer::checkAfter<Token::OctalLiteral>() const
//{
//  return checkAfter<Token::HexadecimalLiteral>();
//}

Token Lexer::readOctal(size_t start)
{
  while (!atEnd() && Lexer::isOctal(peekChar()))
    readChar();

  //if (!checkAfter<Token::OctalLiteral>())
  //  throw std::runtime_error{ "Lexer::readOCtal() : unexpected end of input" };

  return create(start, TokenType::OctalLiteral);
}


//template<>
//bool Lexer::checkAfter<Token::BinaryLiteral>() const
//{
//  return checkAfter<Token::HexadecimalLiteral>();
//}

Token Lexer::readBinary(size_t start)
{
  const char b = readChar();
  assert(b == 'b');

  if (atEnd())  // input ends with '0b' -> error
    throw std::runtime_error{ "Lexer::readBinary() : unexpected end of input" };

  while (!atEnd() && Lexer::isBinary(peekChar()))
    readChar();

  //if (!checkAfter<Token::BinaryLiteral>())
  //  throw std::runtime_error{ "Lexer::readBinary() : unexpected end of input" };

  return create(start, TokenType::BinaryLiteral);
}

Token Lexer::readDecimal(size_t start)
{
  // Reading decimal numbers
  // eg. : 25
  //       3.14
  //       3.14f
  //       100e100
  //       6.02e23
  //       6.67e-11

  while (!atEnd() && Lexer::isDigit(peekChar()))
    readChar();

  if (atEnd())
    create(start, TokenType::IntegerLiteral);

  bool is_decimal = false;

  if (peekChar() == '.')
  {
    readChar();
    is_decimal = true;

    while (!atEnd() && Lexer::isDigit(peekChar()))
      readChar();

    if (atEnd())
      return create(start, TokenType::DecimalLiteral);
  }

  if (peekChar() == 'e')
  {
    readChar();
    is_decimal = true;

    if (atEnd())
      throw std::runtime_error{ "Lexer::readDecimal() : unexpected end of input while reading floating point literal" };

    if (peekChar() == '+' || peekChar() == '-')
    {
      readChar();
      if (atEnd())
        throw std::runtime_error{ "Lexer::readDecimal() : unexpected end of input while reading floating point literal" };
    }

    while (!atEnd() && Lexer::isDigit(peekChar()))
      readChar();

    if (atEnd())
      return create(start, TokenType::DecimalLiteral);
  }


  if (peekChar() == 'f') // eg. 125.f
  {
    readChar();
    is_decimal = true;
  }
  else
  {
    if (tryReadLiteralSuffix())
      return create(start, TokenType::UserDefinedLiteral);
  }

  //if(!checkAfter<Token::DecimalLiteral>())
  //  throw std::runtime_error{ "Lexer::readDecimal() : unexpected input char after floating point literal" };

  return create(start, is_decimal ? TokenType::DecimalLiteral : TokenType::IntegerLiteral);
}

bool Lexer::tryReadLiteralSuffix()
{
  auto cpos = pos();

  if (!this->atEnd() && (Lexer::isLetter(peekChar()) || peekChar() == '_'))
    readChar();
  else
    return false;

  while (!this->atEnd() && (Lexer::isLetter(peekChar()) || Lexer::isDigit(peekChar()) || peekChar() == '_'))
    readChar();

  const bool read = (cpos != pos());
  return read;
}

Token Lexer::readIdentifier(size_t start)
{
  while (!this->atEnd() && (Lexer::isLetter(peekChar()) || Lexer::isDigit(peekChar()) || peekChar() == '_'))
    readChar();
  return create(start, pos() - start, identifierType(start, pos()));
}


struct Keyword {
  const char *name;
  TokenType toktype;
};

const Keyword l2k[] = {
  { "if", TokenType::If },
};

const Keyword l3k[] = {
  { "for", TokenType::For },
  { "int", TokenType::Int },
};

const Keyword l4k[] = {
  { "auto", TokenType::Auto },
  { "bool", TokenType::Bool },
  { "char", TokenType::Char },
  { "else", TokenType::Else },
  { "enum", TokenType::Enum },
  { "this", TokenType::This },
  { "true", TokenType::True },
  { "void", TokenType::Void },
};

const Keyword l5k[] = {
  { "break", TokenType::Break },
  { "class", TokenType::Class },
  { "const", TokenType::Const },
  { "false", TokenType::False },
  { "final", TokenType::Final },
  { "float", TokenType::Float },
  { "using", TokenType::Using },
  { "while", TokenType::While },
};

const Keyword l6k[] = {
  { "delete", TokenType::Delete },
  { "double", TokenType::Double },
  { "export", TokenType::Export },
  { "friend", TokenType::Friend },
  { "import", TokenType::Import },
  { "inline", TokenType::Inline },
  { "public", TokenType::Public },
  { "return", TokenType::Return },
  { "static", TokenType::Static },
  { "struct", TokenType::Struct },
  { "typeid", TokenType::Typeid },
};

const Keyword l7k[] = {
  { "default", TokenType::Default },
  { "mutable", TokenType::Mutable },
  { "private", TokenType::Private },
  { "typedef", TokenType::Typedef },
  { "virtual", TokenType::Virtual },
};

const Keyword l8k[] = {
  { "continue", TokenType::Continue },
  { "explicit", TokenType::Explicit },
  { "noexcept", TokenType::Noexcept },
  { "operator", TokenType::Operator },
  { "override", TokenType::Override },
  { "template", TokenType::Template },
  { "typename", TokenType::Typename },
};

const Keyword l9k[] = {
  { "constexpr", TokenType::Constexpr },
  { "namespace", TokenType::Namespace },
  { "protected", TokenType::Protected },
};

static TokenType findKeyword(const Keyword * keywords, size_t arraySize, const char *str, size_t length)
{
  for (int i(0); i < arraySize; ++i) {
    if (std::memcmp(keywords[i].name, str, length) == 0)
      return keywords[i].toktype;
  }
  return TokenType::UserDefinedName;
}

TokenType Lexer::identifierType(size_t begin, size_t end) const
{
  const char *str = m_chars + begin;
  const size_t l = end - begin;

  switch (l) {
  case 1:
    return TokenType::UserDefinedName;
  case 2:
    return findKeyword(l2k, sizeof(l2k) / sizeof(Keyword), str, l);
  case 3:
    return findKeyword(l3k, sizeof(l3k) / sizeof(Keyword), str, l);
  case 4:
    return findKeyword(l4k, sizeof(l4k) / sizeof(Keyword), str, l);
  case 5:
    return findKeyword(l5k, sizeof(l5k) / sizeof(Keyword), str, l);
  case 6:
    return findKeyword(l6k, sizeof(l6k) / sizeof(Keyword), str, l);
  case 7:
    return findKeyword(l7k, sizeof(l7k) / sizeof(Keyword), str, l);
  case 8:
    return findKeyword(l8k, sizeof(l8k) / sizeof(Keyword), str, l);
  case 9:
    return findKeyword(l9k, sizeof(l9k) / sizeof(Keyword), str, l);
  default:
    break;
  }

  return TokenType::UserDefinedName;
}


//template<>
//bool Lexer::checkAfter<Token::StringLiteral>() const
//{
//  return atEnd() || ctype(peekChar()) != Digit;
//}

Token Lexer::readStringLiteral(size_t start)
{
  while (!atEnd() && peekChar() != '"')
  {
    if (peekChar() == '\\')
    {
      readChar();
      if (!atEnd())
        readChar();
    }
    else if (peekChar() == '\n')
      throw std::runtime_error{ "Lexer::readStringLiteral() : end of line reached before end of string literal " };
    else
      readChar();
  }

  if(atEnd())
    throw std::runtime_error{ "Lexer::readStringLiteral() : unexpected end of input before end of string literal " };

  assert(peekChar() == '"');
  readChar();

  if (tryReadLiteralSuffix())
    return create(start, TokenType::UserDefinedLiteral);
  
  //if (!checkAfter<Token::StringLiteral>())
  //  throw std::runtime_error{ "Lexer::readStringLiteral() : unexpected character after string literal" };

  return create(start, TokenType::StringLiteral);
}

Token Lexer::readCharLiteral(size_t start)
{
  if(atEnd())
    throw std::runtime_error{ "Lexer::readCharLiteral() : unexpected end of input before end of char-literal " };

  readChar();

  if (atEnd())
    throw std::runtime_error{ "Lexer::readCharLiteral() : unexpected end of input before end of char-literal " };

  if(ctype(readChar()) != SingleQuote)
    throw std::runtime_error{ "Lexer::readCharLiteral() : malformed char-literal " };

  return create(start, TokenType::StringLiteral);
}

Token Lexer::readFromPunctuator(size_t start)
{
  char p = m_chars[pos() - 1]; /// bad, TODO : clean up
  if (p == '/')
  {
    if (atEnd())
      return create(start, TokenType::Div);
    if (peekChar() == '/')
      return readSingleLineComment(start);
    else if (peekChar() == '*')
      return readMultiLineComment(start);
    else
      return readOperator(start);
  }
  
  return this->readOperator(start);
}


Token Lexer::readColonOrColonColon(size_t start)
{
  if (atEnd())
    return create(start, TokenType::Colon);

  if (peekChar() == ':')
  {
    readChar();
    return create(start, TokenType::ScopeResolution);
  }

  return create(start, TokenType::Colon);
}


struct OperatorLexeme {
  const char *name;
  TokenType toktype;
};


const OperatorLexeme l1op[] = {
  { "+", TokenType::Plus },
  { "-", TokenType::Minus },
  { "!", TokenType::LogicalNot },
  { "~", TokenType::BitwiseNot },
  { "*", TokenType::Mul },
  { "/", TokenType::Div },
  { "%", TokenType::Remainder },
  { "<", TokenType::Less },
  { ">", TokenType::GreaterThan },
  { "&", TokenType::BitwiseAnd },
  { "^", TokenType::BitwiseXor },
  { "|", TokenType::BitwiseOr },
  { "=", TokenType::Eq },
};

const OperatorLexeme l2op[] = {
  { "++", TokenType::PlusPlus },
  { "--", TokenType::MinusMinus },
  { "<<", TokenType::LeftShift },
  { ">>", TokenType::RightShift },
  { "<=", TokenType::LessEqual },
  { ">=", TokenType::GreaterThanEqual },
  { "==", TokenType::EqEq },
  { "!=", TokenType::Neq },
  { "&&", TokenType::LogicalAnd },
  { "||", TokenType::LogicalOr },
  { "*=", TokenType::MulEq },
  { "/=", TokenType::DivEq },
  { "%=", TokenType::RemainderEq },
  { "+=", TokenType::AddEq },
  { "-=", TokenType::SubEq },
  { "&=", TokenType::BitAndEq },
  { "|=", TokenType::BitOrEq },
  { "^=", TokenType::BitXorEq },
};


const OperatorLexeme l3op[] = {
  { "<<=", TokenType::LeftShiftEq },
  { ">>=", TokenType::RightShiftEq },
};


static TokenType findOperator(const OperatorLexeme * ops, size_t arraySize, const char *str, size_t length)
{
  for (int i(0); i < arraySize; ++i) {
    if (std::memcmp(ops[i].name, str, length) == 0)
      return ops[i].toktype;
  }
  return TokenType::Invalid;
}


TokenType Lexer::getOperator(size_t begin, size_t end) const
{
  const char *str = m_chars + begin;
  const size_t l = end - begin;

  switch (l) {
  case 1:
    return findOperator(l1op, sizeof(l1op) / sizeof(OperatorLexeme), str, l);
  case 2:
    return findOperator(l2op, sizeof(l2op) / sizeof(OperatorLexeme), str, l);
  case 3:
    return findOperator(l3op, sizeof(l3op) / sizeof(OperatorLexeme), str, l);
  default:
    break;
  }

  return TokenType::Invalid;
}

Token Lexer::readOperator(size_t start)
{
  TokenType op = getOperator(start, pos());
  if (op == TokenType::Invalid)
    throw std::runtime_error{ "Lexer::readOperator() : no operator found starting with given chars" };
  
  while (!atEnd())
  {
    size_t p = pos();
    readChar();
    TokenType candidate = getOperator(start, pos());
    if (candidate == TokenType::Invalid)
    {
      seek(p);
      break;
    }
    else
      op = candidate;
  }

  return create(start, op);
}

Token Lexer::readSingleLineComment(size_t start)
{
  readChar(); // reads the second '/'

  while (!atEnd() && peekChar() != '\n')
    readChar();

  return create(start, TokenType::SingleLineComment);
}

Token Lexer::readMultiLineComment(size_t start)
{
  readChar(); // reads the '*' after opening '/'

  do {
    while (!atEnd() && peekChar() != '*')
      readChar();

    if (atEnd())
      throw std::runtime_error{ "Lexer::readMultiLineComment() : unexpected end of input before end of comment" };

    assert(peekChar() == '*');
    readChar(); // reads the '*'

    if (atEnd())
      throw std::runtime_error{ "Lexer::readMultiLineComment() : unexpected end of input before end of comment" };

  } while (peekChar() != '/');

  readChar(); // reads the closing '/'
  return create(start, TokenType::MultiLineComment);
}

} // namespace parsers

} // namespace cxx
