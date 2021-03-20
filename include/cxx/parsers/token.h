// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TOKEN_H
#define CXXAST_TOKEN_H

#include "cxx/cxxast-defs.h"

#include <string>

namespace cxx
{

namespace parsers
{

class StringView
{
public:
  const char* str_ = nullptr;
  size_t count_ = 0;

public:
  StringView() = default;
  StringView(const StringView&) = default;
  ~StringView() = default;

  StringView(const char* str, size_t count)
    : str_(str), count_(count)
  {

  }

  const char* data() const { return str_; }
  size_t size() const { return count_; }
  
  std::string to_string() const { return std::string(str_, str_ + count_); }

  StringView& operator=(const StringView&) = default;
};

inline bool eqchar_same_size(size_t n, const char* lhs, const char* rhs)
{
  while(n-- > 0)
  { 
    if (*(rhs++) != *(lhs++))
      return false;
  }

  return true;
}

inline bool operator==(const StringView& lhs, const char* rhs)
{
  size_t n = 0;

  while (*rhs != '\0' && n < lhs.size())
  {
    if (*(rhs++) != lhs.str_[n++])
      return false;
  }

  return n == lhs.size() && *rhs == '\0';
}

inline bool operator==(const StringView& lhs, const StringView& rhs)
{
  return lhs.size() == rhs.size() && eqchar_same_size(lhs.size(), lhs.str_, rhs.str_);
}

inline bool operator==(const StringView& lhs, const std::string& rhs)
{
  return lhs.size() == rhs.size() && eqchar_same_size(lhs.size(), lhs.str_, rhs.data());
}

inline bool operator!=(const StringView& lhs, const StringView& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const StringView& lhs, const char* rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const StringView& lhs, const std::string& rhs)
{
  return !(lhs == rhs);
}

class TokenCategory
{
public:
  enum Value
  {
    Punctuator = 0x010000,
    Literal = 0x020000,
    OperatorToken = 0x040000,
    Identifier = 0x080000,
    Keyword = 0x100000 | Identifier,
  };
};

class TokenType
{
public:
  enum Value
  {
    Invalid = 0,
    // literals 
    IntegerLiteral = TokenCategory::Literal | 1,
    DecimalLiteral,
    BinaryLiteral,
    OctalLiteral,
    HexadecimalLiteral,
    // do we need to create a CharLiteral 
    StringLiteral,
    // punctuators 
    LeftPar = TokenCategory::Punctuator | 7,
    RightPar,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    Semicolon,
    Colon,
    Dot,
    QuestionMark,
    SlashSlash,
    SlashStar,
    StarSlash,
    // keywords 
    Auto = TokenCategory::Keyword | 20,
    Bool,
    Break,
    Case,
    Catch,
    Char,
    Class,
    Const,
    Constexpr,
    Continue,
    Default,
    Delete,
    Do,
    Double,
    Else,
    Enum,
    Explicit,
    Export,
    False,
    Final,
    Float,
    For,
    Friend,
    If,
    Import,
    Inline,
    Int,
    Mutable,
    Namespace,
    Noexcept,
    Operator,
    Override,
    Private,
    Protected,
    Public,
    Return,
    Static,
    Struct,
    Template,
    This,
    Throw,
    True,
    Try,
    Typedef,
    Typeid,
    Typename,
    Using,
    Virtual,
    Void,
    While,
    //Operators
    ScopeResolution = TokenCategory::OperatorToken | 68,
    PlusPlus,
    MinusMinus,
    Plus,
    Minus,
    LogicalNot,
    BitwiseNot,
    Mul,
    Div,
    Remainder,
    LeftShift,
    RightShift,
    Less,
    GreaterThan,
    LessEqual,
    GreaterThanEqual,
    EqEq,
    Neq,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    LogicalAnd,
    LogicalOr,
    Eq,
    MulEq,
    DivEq,
    AddEq,
    SubEq,
    RemainderEq,
    LeftShiftEq,
    RightShiftEq,
    BitAndEq,
    BitOrEq,
    BitXorEq,
    Comma,
    // Misc
    UserDefinedName = TokenCategory::Identifier | 103,
    UserDefinedLiteral = TokenCategory::Literal | 104,
    SingleLineComment = 102,
    LeftRightPar,
    LeftRightBracket,
    //perhaps it would be better to have two tokens for
    //multiline comments : an opening token and a  closing one
    MultiLineComment,
    //alias
    Ampersand = BitwiseAnd,
    Ref = Ampersand,
    RefRef = LogicalAnd,
    LeftAngle = Less,
    RightAngle = GreaterThan,
    LeftLeftAngle = LeftShift,
    RightRightAngle = RightShift,
    Tilde = BitwiseNot,
    Asterisk = Mul,
    Star = Asterisk,
    Zero = OctalLiteral, // Zero is an octal literal of length 1
  };

private:
  Value m_value = Invalid;

public:
  TokenType() = default;
  TokenType(Value v) : m_value(v) { }
  

  Value value() const { return m_value; }
};

inline bool operator==(const TokenType& lhs, const TokenType& rhs)
{
  return lhs.value() == rhs.value();
}

inline bool operator!=(const TokenType& lhs, const TokenType& rhs)
{
  return lhs.value() != rhs.value();
}

class Token
{
private:
  TokenType type_;
  StringView str_;
  // @TODO: try to merge 'col_' and 'type_' to save space
  int line_ = -1;
  int col_ = -1;

public:
  Token();
  Token(const Token & ) = default;
  ~Token() = default;

  Token(TokenType t, StringView str)
    : type_(t), str_(str)
  {

  }

  Token(TokenType t, StringView str, int l, int c)
    : type_(t), str_(str), line_(l), col_(c)
  {

  }

  bool isValid() const { return type_.value() != TokenType::Invalid; }

  TokenType type() const { return type_; }
  StringView text() const { return str_; }
  std::string to_string() const { return str_.to_string(); }
  int line() const { return line_; }
  int col() const { return col_; }

  bool isOperator() const { return type_.value() & TokenCategory::OperatorToken; }
  bool isIdentifier() const { return type_.value() & TokenCategory::Identifier; }
  bool isKeyword() const { return (type_.value() & TokenCategory::Keyword) == TokenCategory::Keyword; }
  bool isLiteral() const { return type_.value() & TokenCategory::Literal; }

  bool operator==(const Token& other) const { return type_ == other.type_ && other.str_ == str_; }
  bool operator!=(const Token & other) const { return !operator==(other); }
  bool operator==(TokenType tok) const { return type_ == tok; }
  bool operator!=(TokenType tok) const { return !operator==(tok); }

  Token & operator=(const Token&) = default;
};

inline Token::Token()
  : type_(TokenType::Invalid), str_()
{

}

} // namespace parsers

} // namespace cxx


#endif // CXXAST_TOKEN_H
