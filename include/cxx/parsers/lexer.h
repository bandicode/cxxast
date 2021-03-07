// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_LEXER_H
#define CXXAST_LEXER_H

#include "cxx/parsers/token.h"

namespace cxx
{

namespace parsers
{

class CXXAST_API Lexer
{
public:
  Lexer() = default;
  explicit Lexer(const std::string* src);

  const std::string& source() const;

  void start();

  Token read();

  bool atEnd() const;

  size_t pos() const;
  int line() const;
  int col() const;

  void seek(size_t pos);
  void reset(const std::string* src);

  enum CharacterType {
    Invalid,
    Space,
    Letter,
    Digit,
    Dot,
    SingleQuote,
    DoubleQuote,
    LeftPar,
    RightPar,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Punctuator,
    Underscore,
    Semicolon,
    Colon, 
    QuestionMark,
    Comma,
    Tabulation,
    LineBreak,
    CarriageReturn,
    Other,
  };

  static CharacterType ctype(char c);
  inline static bool isLetter(char c) { return ctype(c) == Letter; }
  inline static bool isDigit(char c) { return ctype(c) == Digit; }
  inline static bool isBinary(char c) { return c == '0' || c == '1'; }
  inline static bool isOctal(char c) { return '0' <= c && c <= '7'; }
  inline static bool isDecimal(char c) { return isDigit(c); }
  inline static bool isHexa(char c) { return isDecimal(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
  static bool isDiscardable(char c);
  inline static bool isSpace(char c) { return ctype(c) == Space; }

protected:
  char readChar();
  void discardChar() noexcept;
  char charAt(size_t pos);
  char currentChar() const;
  inline char peekChar() const { return currentChar(); }
  void consumeDiscardable();
  Token create(size_t pos, size_t length, TokenType type);
  Token create(size_t pos, TokenType type);
  Token readNumericLiteral(size_t pos);
  Token readHexa(size_t pos);
  Token readOctal(size_t pos);
  Token readBinary(size_t pos);
  Token readDecimal(size_t pos);
  Token readIdentifier(size_t pos);
  TokenType identifierType(size_t begin, size_t end) const;
  Token readStringLiteral(size_t pos);
  Token readCharLiteral(size_t pos);
  TokenType getOperator(size_t begin, size_t end) const;
  Token readOperator(size_t pos);
  Token readColonOrColonColon(size_t pos);
  Token readFromPunctuator(size_t pos);
  Token readSingleLineComment(size_t pos);
  Token readMultiLineComment(size_t pos);
  bool tryReadLiteralSuffix();

private:
  friend class LexerGuard;

private:
  const std::string* m_source = nullptr;
  const char* m_chars = nullptr;
  size_t m_pos = 0;
  int m_line = 0;
  int m_col = 0;
};

} // namespace parsers

} // namespace cxx

#endif // CXXAST_LEXER_H
