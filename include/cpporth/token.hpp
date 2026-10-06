#ifndef CPPORTH__TOKEN_HPP_
#define CPPORTH__TOKEN_HPP_

#include <boost/regex.hpp>
#include <optional>
#include <string>
#include <unordered_map>

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
    OperatorAdd,
    OperatorSub,
    OperatorMul,
    OperatorEq,
    OperatorNeq,
    OperatorGt,
    OperatorGte,
    OperatorLt,
    OperatorLte,
    OperatorLoad8,
    OperatorLoad16,
    OperatorLoad32,
    OperatorLoad64,
    OperatorStore8,
    OperatorStore16,
    OperatorStore32,
    OperatorStore64,
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
    KeywordSyscallN,
    KeywordPrint,
    KeywordMax,
    KeywordDivmod,
    KeywordShr,
    KeywordShl,
    KeywordOr,
    KeywordAnd,
    KeywordNot,
    KeywordCastInt,
    KeywordCastBool,
    KeywordCastPtr,
    Newline,
    Bikeshedder,
    EndOfFile,
    NIL,
    COUNT_PLUS_ONE,
  };

  enum class BuiltinType
  {
    Int,
    Addr,
    Ptr,
    Bool
  };

  namespace detail
  {
    using enum TokenType;
    const std::unordered_map<std::string, TokenType> tokentype_map
        = { { "if", KeywordIf },
            { "if*", KeywordIfStar },
            { "while", KeywordWhile },
            { "proc", KeywordProc },
            { "do", KeywordDo },
            { "let", KeywordLet },
            { "in", KeywordIn },
            { "var", KeywordVar },
            { "memory", KeywordMemory },
            { "end", KeywordEnd },
            { "reset", KeywordReset },
            { "offset", KeywordOffset },
            { "addr-of", KeywordAddrOf },
            { "call-like", KeywordCallLike },
            { "include", KeywordInclude },
            { "syscall", KeywordSyscallN },
            { "here", KeywordHere },
            { "inline", KeywordInline },
            { "assert", KeywordAssert },
            { "dup", KeywordDup },
            { "over", KeywordOver },
            { "rot", KeywordRot },
            { "drop", KeywordDrop },
            { "swap", KeywordSwap },
            { "peek", KeywordPeek },
            { "else", KeywordElse },
            { "const", KeywordConst },
            { "divmod", KeywordDivmod },
            { "max", KeywordMax },
            { "shl", KeywordShl },
            { "shr", KeywordShr },
            { "not", KeywordNot },
            { "and", KeywordAnd },
            { "or", KeywordOr },
            { "print", KeywordPrint },
            { "cast(bool)", KeywordCastBool },
            { "cast(int)", KeywordCastInt },
            { "cast(ptr)", KeywordCastPtr },
            { "+", OperatorAdd },
            { "-", OperatorSub },
            { "*", OperatorMul },
            { "=", OperatorEq },
            { "!=", OperatorNeq },
            { ">", OperatorGt },
            { "<", OperatorLt },
            { ">=", OperatorGte },
            { "<=", OperatorLt },
            { "!8", OperatorStore8 },
            { "!16", OperatorStore16 },
            { "!32", OperatorStore32 },
            { "!64", OperatorStore64 },
            { "@8", OperatorLoad8 },
            { "@16", OperatorLoad16 },
            { "@32", OperatorLoad32 },
            { "@64", OperatorLoad64 },
            { "--", Bikeshedder } };
  }

  inline std::optional<TokenType>
  string_to_tokentype (const std::string &in)
  {
    if (auto search = detail::tokentype_map.find (in);
        search != detail::tokentype_map.end ())
      return search->second;
    return {};
  }

  inline std::optional<std::string>
  tokentype_to_string (TokenType type)
  {
    for (const auto &[key, value] : detail::tokentype_map)
      if (value == type)
        return key;
    return {};
  }

  struct Token
  {
    TokenType token_type;
    std::string string_value;
    std::size_t start;
    std::size_t end;

    bool
    categorize (const boost::regex &pat, TokenType on_success)
    {
      if (boost::regex_match (string_value, pat))
        {
          token_type = on_success;
          return true;
        }
      return false;
    }
  };
}

#endif // CPPORTH__TOKEN_HPP_