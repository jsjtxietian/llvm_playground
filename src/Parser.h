#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"

// The coding guidelines from LLVM forbid the use of the <iostream> library
#include "llvm/Support/raw_ostream.h"

class Parser {
  // Tok stores the next token (the look-ahead),
  // Lex is used to retrieve the next token from the input.
  Lexer &Lex;
  Token Tok;
  bool HasError;

  void error() {
    llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
    HasError = true;
  }

  void advance() { Lex.next(Tok); }

  bool expect(Token::TokenKind Kind) {
    if (!Tok.is(Kind)) {
      error();
      return true;
    }
    return false;
  }

  // retrieves the next token if the look-ahead is of the expected kind.
  bool consume(Token::TokenKind Kind) {
    if (expect(Kind))
      return true;
    advance();
    return false;
  }

  AST *parseCalc();
  Expr *parseExpr();
  Expr *parseTerm();
  Expr *parseFactor();

public:
  Parser(Lexer &Lex) : Lex(Lex), HasError(false) { advance(); }
  AST *parse();
  bool hasError() { return HasError; }
};

#endif