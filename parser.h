/*
 * IFJ Project
 * @brief Header file for parser
 *
 * @author Martin Valapka - xvalapm00
 */

#ifndef _PARSER_H
#define _PARSER_H

#include "scanner.h"
#include "ast.h"
#include "error_codes.h"
#include "expr-parser.h"
#include "symtable.h"

extern Token* currentToken;
extern SymbolTable* symbolTable;
extern AST* ast;

/**
 * @brief Peeks the next token from scanner before consuming it
 */
void peek();

/**
 * @brief Parses the whole program
 * 
 * @return Returns 0 if the program was parsed successfully, non-zero if an error was encountered
 */
int parse();

/**
 * @brief PROLOG ::= token_const token_ifj token_equals token_@import("ifj24.zig"); 
 */
void parseProlog();

/**
 * @brief FUNC_DEFS ::= FUNC_DEF FUNC_DEFS | ε
 */
void parseFuncDefs();

/**
 * @brief FUNC_DEF ::= token_pub token_fn token_func_id token_Orb PARAMS token_Crb FUNC_TYPE 
 * 
 * Also handles FUNC_TYPE decision (void vs non-void)
 * V_FUNC ::= token_void token_Ocb STATEMENTS V_RETURN token_Ccb
 * FUNC ::= TYPE token_Ocb STATEMENTS RETURN token_Ccb
 */
void parseFuncDef();

/**
 * @brief FUNC ::= TYPE token_Ocb STATEMENTS RETURN token_Ccb
 */
void parseFunc();

/**
 * @brief RETURN ::= token_return EXPR token_semicolon
 *        V_RETURN ::= token_return token_semicolon | ε       
 */
void parseReturn();

/**
 * @brief PARAMS ::= token_id token_colon TYPE NEXT_PARAM | ε
 * Also handles NEXT_PARAM decision (token_comma vs ε)
 */
void parseParams();

/**
 * @brief TYPE ::= token_i32 | token_?i32 | token_f64 | token_?f64 | token_[]u8 | token_?[]u8 | token_void
 */
void parseType();

/**
 * @brief VAR_TYPE token_id TYPE_SPEC token_equals EXPR token_semicolon
 * Also handles VAR_TYPE decision (var vs const)
 */
void parseVarDef();

/**
 * @brief TYPE_SPEC ::= token_colon TYPE | ε
 */
void parseTypeSpec();

/**
 * @brief TYPE_SPEC ::= token_Osb token_Csb | ε
 */
void parseVarAss();

/**
 * @brief token_if token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb ELSE
 */
void parseIf();

/**
 * @brief token_else token_Ocb STATEMENTS token_Ccb | ε
 */
void parseElse();

/**
 * @brief token_while token_Orb EXPR token_Crb NULL_COND token_Ocb STATEMENTS token_Ccb
 */
void parseWhile();

/**
 * @brief token_vb token_id token_vb | ε
 */
void parseNullCond();

/**
 * @brief token_func_id token_Orb ARGS token_Crb token_semicolon
 */
void parseFuncCall();

/**
 * @brief token_underscore token_equals EXPR token_semicolon
 */
void parseDiscardCall();

/**
 * @brief ARGS ::= VAL NEXT_ARG | ε
 * Also handles VAL decision (EXPR vs token_id)
 * Also handles NEXT_ARG decision (token_comma vs ε)
 */
void parseArgs();

#endif //_PARSER_H