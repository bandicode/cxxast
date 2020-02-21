// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TOKEN_H
#define CXXAST_TOKEN_H

#include "cxx/cxxast-defs.h"

#include <string>

namespace cxx
{

namespace parser
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
    DecimalLiteral = TokenCategory::Literal | 2,
    BinaryLiteral = TokenCategory::Literal | 3,
    OctalLiteral = TokenCategory::Literal | 4,
    HexadecimalLiteral = TokenCategory::Literal | 5,
    // do we need to create a CharLiteral 
    StringLiteral = TokenCategory::Literal | 6,
    // punctuators 
    LeftPar = TokenCategory::Punctuator | 7,
    RightPar = TokenCategory::Punctuator | 8,
    LeftBracket = TokenCategory::Punctuator | 9,
    RightBracket = TokenCategory::Punctuator | 10,
    LeftBrace = TokenCategory::Punctuator | 11,
    RightBrace = TokenCategory::Punctuator | 12,
    Semicolon = TokenCategory::Punctuator | 13,
    Colon = TokenCategory::Punctuator | 14,
    Dot = TokenCategory::Punctuator | 15,
    QuestionMark = TokenCategory::Punctuator | 16,
    SlashSlash = TokenCategory::Punctuator | 17,
    SlashStar = TokenCategory::Punctuator | 18,
    StarSlash = TokenCategory::Punctuator | 19,
    // keywords 
    Auto = TokenCategory::Keyword | 20,
    Bool = TokenCategory::Keyword | 21,
    Break = TokenCategory::Keyword | 22,
    Char = TokenCategory::Keyword | 23,
    Class = TokenCategory::Keyword | 24,
    Const = TokenCategory::Keyword | 25,
    Continue = TokenCategory::Keyword | 26,
    Default = TokenCategory::Keyword | 27,
    Delete = TokenCategory::Keyword | 28,
    Double = TokenCategory::Keyword | 29,
    Else = TokenCategory::Keyword | 30,
    Enum = TokenCategory::Keyword | 31,
    Explicit = TokenCategory::Keyword | 32,
    Export = TokenCategory::Keyword | 33,
    False = TokenCategory::Keyword | 34,
    Float = TokenCategory::Keyword | 35,
    For = TokenCategory::Keyword | 36,
    Friend = TokenCategory::Keyword | 37,
    If = TokenCategory::Keyword | 38,
    Import = TokenCategory::Keyword | 39,
    Int = TokenCategory::Keyword | 40,
    Mutable = TokenCategory::Keyword | 41,
    Namespace = TokenCategory::Keyword | 42,
    Operator = TokenCategory::Keyword | 43,
    Private = TokenCategory::Keyword | 44,
    Protected = TokenCategory::Keyword | 45,
    Public = TokenCategory::Keyword | 46,
    Return = TokenCategory::Keyword | 47,
    Static = TokenCategory::Keyword | 48,
    Struct = TokenCategory::Keyword | 49,
    Template = TokenCategory::Keyword | 50,
    This = TokenCategory::Keyword | 51,
    True = TokenCategory::Keyword | 52,
    Typedef = TokenCategory::Keyword | 53,
    Typeid = TokenCategory::Keyword | 54,
    Typename = TokenCategory::Keyword | 55,
    Using = TokenCategory::Keyword | 56,
    Virtual = TokenCategory::Keyword | 57,
    Void = TokenCategory::Keyword | 58,
    While = TokenCategory::Keyword | 59,
    //Operators
    ScopeResolution = TokenCategory::OperatorToken | 60,
    PlusPlus = TokenCategory::OperatorToken | 61,
    MinusMinus = TokenCategory::OperatorToken | 62,
    Plus = TokenCategory::OperatorToken | TokenCategory::Punctuator | 63,
    Minus = TokenCategory::OperatorToken | TokenCategory::Punctuator | 64,
    LogicalNot = TokenCategory::OperatorToken | TokenCategory::Punctuator | 65,
    BitwiseNot = TokenCategory::OperatorToken | TokenCategory::Punctuator | 66,
    Mul = TokenCategory::OperatorToken | TokenCategory::Punctuator | 67,
    Div = TokenCategory::OperatorToken | TokenCategory::Punctuator | 68,
    Remainder = TokenCategory::OperatorToken | TokenCategory::Punctuator | 69,
    LeftShift = TokenCategory::OperatorToken | 70,
    RightShift = TokenCategory::OperatorToken | 71,
    Less = TokenCategory::OperatorToken | TokenCategory::Punctuator | 72,
    GreaterThan = TokenCategory::OperatorToken | TokenCategory::Punctuator | 73,
    LessEqual = TokenCategory::OperatorToken | 74,
    GreaterThanEqual = TokenCategory::OperatorToken | 75,
    EqEq = TokenCategory::OperatorToken | 76,
    Neq = TokenCategory::OperatorToken | 77,
    BitwiseAnd = TokenCategory::OperatorToken | TokenCategory::Punctuator | 78,
    BitwiseOr = TokenCategory::OperatorToken | TokenCategory::Punctuator | 79,
    BitwiseXor = TokenCategory::OperatorToken | TokenCategory::Punctuator | 80,
    LogicalAnd = TokenCategory::OperatorToken | 81,
    LogicalOr = TokenCategory::OperatorToken | 82,
    Eq = TokenCategory::OperatorToken | 83,
    MulEq = TokenCategory::OperatorToken | 84,
    DivEq = TokenCategory::OperatorToken | 85,
    AddEq = TokenCategory::OperatorToken | 86,
    SubEq = TokenCategory::OperatorToken | 87,
    RemainderEq = TokenCategory::OperatorToken | 88,
    LeftShiftEq = TokenCategory::OperatorToken | 89,
    RightShiftEq = TokenCategory::OperatorToken | 90,
    BitAndEq = TokenCategory::OperatorToken | 91,
    BitOrEq = TokenCategory::OperatorToken | 92,
    BitXorEq = TokenCategory::OperatorToken | 93,
    Comma = TokenCategory::OperatorToken | 94,
    UserDefinedName = TokenCategory::Identifier | 95,
    UserDefinedLiteral = TokenCategory::Literal | 96,
    SingleLineComment = 97,
    LeftRightPar = 98,
    LeftRightBracket = 99,
    //perhaps it would be better to have two tokens for
    //multiline comments : an opening token and a  closing one
    MultiLineComment = 100,
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

public:
  Token();
  Token(const Token & ) = default;
  ~Token() = default;

  Token(TokenType t, StringView str)
    : type_(t), str_(str)
  {

  }

  bool isValid() const { return type_.value() != TokenType::Invalid; }

  TokenType type() const { return type_; }
  StringView text() const { return str_; }
  std::string to_string() const { return str_.to_string(); }

  bool isOperator() const { return type_.value() & TokenCategory::OperatorToken; }
  bool isIdentifier() const { return type_.value() & TokenCategory::Identifier; }
  bool isKeyword() const { return type_.value() & TokenCategory::Keyword; }
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

} // namespace parser

} // namespace cxx


#endif // CXXAST_TOKEN_H
