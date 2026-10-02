#ifndef CPPORTH__TOKEN_HPP_
#define CPPORTH__TOKEN_HPP_

#include <string>

namespace cpporth
{

  // NEWLINE = 0,
  //   PROC,
  //   IN,
  //   VAR,
  //   WHILE,
  //   END,
  //   IF,
  //   PRINT,
  //   INTVAL, // 8
  //   INCLUDE,
  //   STRING,
  //   CSTRING,
  //   DROP,
  //   SWAP,
  //   DUP,
  //   ROT,
  //   OVER, // 16
  //   OP,
  //   MAX,
  //   MEMORY,
  //   ELSE,
  //   DO,
  //   INT,
  //   INLINE, // 24
  //   PTR,
  //   CONST,
  //   LET,
  //   PEEK,
  //   OFFSET,
  //   RESET,
  //   BOOL,
  //   ADDR, // 32
  //   BIKESHEDDER,
  //   ASSERT,
  //   IFSTAR,
  //   HERE,
  //   END_OF_FILE,
  //   SYSCALLN,
  //   CHAR,
  //   ADDROF, // 40
  //   CALLLIKE,

  // My extensions..
  //   LINE,
  //   TYPE,
  //   MATCH,
  //   RSQUARE,
  //   LSQUARE,
  //   RPAREN,
  //   LPAREN,
  //   COMMA,
  //   ALLOC,
  //   FREE, // 48
  //   COLON,
  //   NEW

  enum class TokenType
  {
    Integer,
    Operator,
    StringLit,
    CStrLit,
    CharLit,
    Identifier,
    KeywordProc,
    KeywordEnd,
    KeywordConst,
    KeywordMemory,
    KeywordIn,
    KeywordVar,
    KeywordDo,
    KeywordWhile,
    KeywordIf,
    KeywordIfStar,
    KeywordLet,
    KeywordPtr,
    KeywordBool,
    KeywordElse,
    KeywordPeek,
    KeywordDup,
    KeywordSwap,
    KeywordDrop,
    KeywordRot,
    KeywordOver,
    KeywordAssert,
    KeywordHere,
    KeywordInclude,
    KeywordInline,
    KeywordInt,
    KeywordOffset,
    KeywordReset,
    KeywordAddr,
    KeywordAddrOf,
    KeywordCallLike, // IDK what this does still..
    Newline,
    Bikeshedder,
    EndOfFile
  };

  struct Token
  {
    TokenType token_type;
    std::string string_value;
    std::size_t start;
    std::size_t end;
  };
}

#endif // CPPORTH__TOKEN_HPP_