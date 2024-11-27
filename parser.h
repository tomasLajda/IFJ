/**
 * IFJ Project
 * @brief Header file for parser functions. Contains functions for all non-terminal symbols in the
 * grammar. Some functions are two non-terminal symbols combined.
 *
 * @author Martin Valapka - xvalapm00
 * @author Matúš Csirik - xcsirim00
 **/

#ifndef _PARSER_H
#define _PARSER_H

#include "ast.h"
#include "error_codes.h"
#include "expr_parser.h"
#include "scanner.h"
#include "semantic_analysis.h"
#include "symtable.h"

extern Token *currentToken;
extern AST *ast;

typedef struct {
    Token *first;
    Token *second;
} TokenBuffer;

/**
 * @brief Parses the whole program. Stores an AST (abstract syntax tree) representing the source
 * code in the global variable 'ast' which must be initialised beforehand.
 *
 * @return Returns 0 if the program was parsed successfully, exits with the corresponding error code
 * otherwise.
 */
int parse();

/**
 * @brief Initializes the token buffer by allocating memory for tokens
 */
void initTokenBuffer();

/**
 * @brief Frees all memory allocated for the token buffer
 */
void freeTokenBuffer();

/**
 * @brief Goes back to the last node that is an IF, WHILE or PUB KEYWORD and sets it as the current
 * and main parent.
 *
 * @param startNode The node to start from.
 */
void goBack(ASTNode *startNode);

/**
 * @brief Goes to the last node that is a PUB KEYWORD and sets it as the current and main parent.
 *
 */
void goToPub();

/**
 * @brief Checks if the current token is a built-in function
 *
 * @param token The token to check
 *
 * @return True if the current token is a built-in function, false otherwise
 */
bool isTokenBuiltInFunction(Token *token);

/**
 * @brief Checks if the current token is a keyword
 *
 * @param keyword The keyword to check
 * @return True if the current token is the keyword, false otherwise
 */
bool isTokenKeyword(Token *token, Keyword keyword);

/**
 * @brief PROG ::= PROLOG FUNC_DEFS
 */
void parseProg();

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
 */
void parseFuncDef();

/**
 * @brief FUNC_TYPE ::= V_FUNC | FUNC
 */
void parseFuncType();

/**
 * @brief V_FUNC ::= token_void token_Ocb STATEMENTS V_RETURN token_Ccb
 */
void parseVoidFunc();

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
 * @brief STATEMENTS ::= STATEMENT STATEMENTS | ε
 */
void parseStatements();

/**
 * @brief STATEMENT ::= VAR_DEF | IF | WHILE | FUNC_CALL | DISCARD_CALL | VAR_ASS
 */
void parseStatement();

/**
 * @brief PARAMS ::= token_id token_colon TYPE NEXT_PARAM | ε
 * Also handles NEXT_PARAM decision (token_comma vs ε)
 */
void parseParams();

/**
 * @brief TYPE ::= token_i32 | token_?i32 | token_f64 | token_?f64 | token_[]u8 | token_?[]u8 |
 * token_void
 */
void parseType();

/**
 * @brief VAR_DEF ::= VAR_TYPE token_id TYPE_SPEC token_equals EXPR token_semicolon
 * Also handles VAR_TYPE decision (var vs const)
 */
void parseVarDef();

/**
 * @brief TYPE_SPEC ::= token_colon TYPE | ε
 */
void parseTypeSpec();

/**
 * @brief VAR_ASS ::= token_id token_equals EXPR token_semicolon
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
 * @brief DISCARD_CALL ::= token_underscore token_equals EXPR token_semicolon
 */
void parseDiscardCall();

/**
 * @brief ARGS ::= VAL NEXT_ARG | ε
 * Also handles VAL decision (EXPR vs token_id)
 * Also handles NEXT_ARG decision (token_comma vs ε)
 */
void parseArgs();

#endif //_PARSER_H