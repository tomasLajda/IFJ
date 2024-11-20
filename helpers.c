/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 * @author Matúš Csirik - xcsirim00
 *
 */

#include "helpers.h"
#include "ast.h"
#include "enums.h"
#include "error_codes.h"
#include "scanner.h"
#include "stack.h"
#include <stdio.h>

char *TokenTypeToString(TokenType type) {
    switch (type) {
    case TOKEN_TYPE_EMPTY:
        return "EMPTY";
    case TOKEN_TYPE_EOF:
        return "EOF";
    case TOKEN_TYPE_EOL:
        return "EOL";
    case TOKEN_TYPE_COMMENT:
        return "COMMENT";
    case TOKEN_TYPE_IDENTIFIER:
        return "ID";
    case TOKEN_TYPE_KEYWORD:
        return "KEYWORD";
    case TOKEN_TYPE_INTEGER_VALUE:
        return "INTEGER_VALUE";
    case TOKEN_TYPE_DOUBLE_VALUE:
        return "DOUBLE_VALUE";
    case TOKEN_TYPE_STRING_VALUE:
        return "STRING_VALUE";
    case TOKEN_TYPE_EQ:
        return "==";
    case TOKEN_TYPE_NEQ:
        return "!=";
    case TOKEN_TYPE_LTH:
        return "<";
    case TOKEN_TYPE_LEQ:
        return "<=";
    case TOKEN_TYPE_GTH:
        return ">";
    case TOKEN_TYPE_GEQ:
        return ">=";
    case TOKEN_TYPE_ASSIGN:
        return "=";
    case TOKEN_TYPE_PLUS:
        return "+";
    case TOKEN_TYPE_MINUS:
        return "-";
    case TOKEN_TYPE_MUL:
        return "*";
    case TOKEN_TYPE_DIV:
        return "/";
    case TOKEN_TYPE_VB:
        return "|";
    case TOKEN_TYPE_LEFT_BR:
        return "(";
    case TOKEN_TYPE_RIGHT_BR:
        return ")";
    case TOKEN_TYPE_LEFT_CURLY_BR:
        return "{";
    case TOKEN_TYPE_RIGHT_CURLY_BR:
        return "}";
    case TOKEN_TYPE_COMMA:
        return ",";
    case TOKEN_TYPE_COLON:
        return ":";
    case TOKEN_TYPE_SEMICOLON:
        return ";";
    case TOKEN_TYPE_DOT:
        return ".";
    case TOKEN_TYPE_EXPR:
        return "EXPR";
    case TOKEN_TYPE_DOLLA:
        return "$";
    default:
        return "UNKNOWN_TOKEN_TYPE";
    }
}

Token *createToken(TokenType type) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    token->type = type;
    token->line = 0;
    // Initialize the attribute based on token type
    switch (type) {
    case TOKEN_TYPE_IDENTIFIER:
    case TOKEN_TYPE_STRING_VALUE:
        token->attribute.string = NULL;
        break;
    case TOKEN_TYPE_INTEGER_VALUE:
        token->attribute.integer = 0;
        break;
    case TOKEN_TYPE_DOUBLE_VALUE:
        token->attribute.decimal = 0.0;
        break;
    default:
        break;
    }
    return token;
}

Token *copyToken(Token *token) {
    if (token == NULL) {
        return NULL;
    }
    Token *newToken = (Token *)malloc(sizeof(Token));
    if (newToken == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    newToken->type = token->type;
    newToken->line = token->line;

    // Deep copy attribute based on its type
    switch (token->type) {
    case TOKEN_TYPE_IDENTIFIER:
    case TOKEN_TYPE_STRING_VALUE:
        if (token->attribute.string != NULL) {
            newToken->attribute.string = (char *)malloc(strlen(token->attribute.string) + 1);
            if (newToken->attribute.string == NULL) {
                HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
            }
            strcpy(newToken->attribute.string, token->attribute.string);
        } else {
            newToken->attribute.string = NULL;
        }
        break;
    case TOKEN_TYPE_INTEGER_VALUE:
        newToken->attribute.integer = token->attribute.integer;
        break;
    case TOKEN_TYPE_DOUBLE_VALUE:
        newToken->attribute.decimal = token->attribute.decimal;
        break;
    case TOKEN_TYPE_KEYWORD:
        newToken->attribute.keyword = token->attribute.keyword;
        break;
    default:
        // For token types without attributes
        break;
    }
    return newToken;
}

ASTNode *copyASTNode(ASTNode *node) {
    if (node == NULL) {
        return NULL;
    }
    ASTNode *newNode = initASTNode();
    newNode->token = copyToken(node->token);
    newNode->parent = node->parent;

    // Copy left child and set parent
    if (node->left != NULL) {
        newNode->left = copyASTNode(node->left);
    }

    // Copy right child and set parent
    if (node->right != NULL) {
        newNode->right = copyASTNode(node->right);
    }

    return newNode;
}

StackElement *createStackElement(Token *token, ASTNode *astNodePtr) {
    StackElement *element = (StackElement *)malloc(sizeof(StackElement));
    if (element == NULL) {
        HANDLE_ERROR("Memory allocation failure", INTERNAL_ERROR, NULL);
    }
    element->ASTNodePtr = astNodePtr;
    element->tokenPtr = token;
    element->next = NULL;
    return element;
}

void freeToken(Token *token) {
    if (token == NULL) {
        return;
    }
    if ((token->type == TOKEN_TYPE_IDENTIFIER || token->type == TOKEN_TYPE_STRING_VALUE) &&
        token->attribute.string != NULL) {
        free(token->attribute.string);
        token->attribute.string = NULL;
    }
    free(token);
}

// Returns 1 if the token is an operator, 0 otherwise
int isOperator(Token *token) {
    return token->type == TOKEN_TYPE_PLUS ||  // +
           token->type == TOKEN_TYPE_MINUS || // -
           token->type == TOKEN_TYPE_MUL ||   // /
           token->type == TOKEN_TYPE_DIV ||   // *
           token->type == TOKEN_TYPE_EQ ||    // ==
           token->type == TOKEN_TYPE_NEQ ||   // !=
           token->type == TOKEN_TYPE_LTH ||   // <
           token->type == TOKEN_TYPE_LEQ ||   // <=
           token->type == TOKEN_TYPE_GTH ||   // >
           token->type == TOKEN_TYPE_GEQ;     // >=
}

// Returns 1 if the token is an operand, 0 otherwise
int isOperand(Token *token) {
    return token->type == TOKEN_TYPE_IDENTIFIER       // id
           || token->type == TOKEN_TYPE_INTEGER_VALUE // i32
           || token->type == TOKEN_TYPE_DOUBLE_VALUE; // f64
}

// Returns 1 if the token is a parentheses, 0 otherwise
int isParentheses(Token *token) {
    return token->type == TOKEN_TYPE_LEFT_BR || token->type == TOKEN_TYPE_RIGHT_BR;
}

// Returns 1 if the token is a delimiter, 0 otherwise
int isDelimiter(Token *token) {
    return token->type == TOKEN_TYPE_SEMICOLON || // ;    a = 2 + 3;
           token->type == TOKEN_TYPE_COMMA ||     // ,    f(2+3, 4){}
           token->type == TOKEN_TYPE_RIGHT_BR;    // )    f(2+3){}
}

int isRelOperator(Token *token) {
    return token->type == TOKEN_TYPE_EQ ||  // ==
           token->type == TOKEN_TYPE_NEQ || // !=
           token->type == TOKEN_TYPE_LTH || // <
           token->type == TOKEN_TYPE_LEQ || // <=
           token->type == TOKEN_TYPE_GTH || // >
           token->type == TOKEN_TYPE_GEQ;   // >=
}
/**
 * @brief Function that transforms the token keyword integer to the corresponding character (string)
 */
char *TokenKeywordToString(Keyword keyword) {
    switch (keyword) {
    case KEYWORD_I_32:
        return "i32";
    case KEYWORD_I_32_NULL:
        return "i32_null";
    case KEYWORD_F_64:
        return "f64";
    case KEYWORD_F_64_NULL:
        return "f64_null";
    case KEYWORD_U_8_ARRAY:
        return "u8_array";
    case KEYWORD_U_8_ARRAY_NULL:
        return "u8_array_null";
    case KEYWORD_VOID:
        return "void";
    case KEYWORD_NULL:
        return "null";
    case KEYWORD_IF:
        return "if";
    case KEYWORD_ELSE:
        return "else";
    case KEYWORD_WHILE:
        return "while";
    case KEYWORD_PUB:
        return "pub";
    case KEYWORD_FN:
        return "fn";
    case KEYWORD_RETURN:
        return "return";
    case KEYWORD_VAR:
        return "var";
    case KEYWORD_CONST:
        return "const";
    case KEYWORD_UNDERSCORE:
        return "_";
    case KEYWORD_MAIN:
        return "main";
    case KEYWORD_IMPORT:
        return "import";
    case KEYWORD_IFJ:
        return "ifj";
    case KEYWORD_STRING:
        return "string";
    case KEYWORD_LENGTH:
        return "length";
    case KEYWORD_CONCAT:
        return "concat";
    case KEYWORD_SUBSTRING:
        return "substring";
    case KEYWORD_STRCMP:
        return "strcmp";
    case KEYWORD_ORD:
        return "ord";
    case KEYWORD_CHR:
        return "chr";
    case KEYWORD_WRITE:
        return "write";
    case KEYWORD_READSTR:
        return "readstr";
    case KEYWORD_READI32:
        return "readi32";
    case KEYWORD_READF64:
        return "readf64";
    case KEYWORD_I2F:
        return "i2f";
    case KEYWORD_F2I:
        return "f2i";
    default:
        return "UNKNOWN_KEYWORD";
    }
}

ASTNode *mockASTProgramStructure(int type) {
    ASTNode *root = initASTNode();
    switch (type) {
    // Discard call
    case 1:
        root->token = createToken(TOKEN_TYPE_KEYWORD);
        root->token->attribute.keyword = KEYWORD_UNDERSCORE;

        ASTNode *left = initASTNode();
        left->token = createToken(TOKEN_TYPE_EXPR);
        AST *exprTree = initAST();

        exprTree->root = initASTNode();
        exprTree->root->token = createToken(TOKEN_TYPE_PLUS);
        exprTree->root->left = initASTNode();
        exprTree->root->left->token = createToken(TOKEN_TYPE_INTEGER_VALUE);
        exprTree->root->left->token->attribute.integer = 2;
        exprTree->root->right = initASTNode();
        exprTree->root->right->token = createToken(TOKEN_TYPE_INTEGER_VALUE);
        exprTree->root->right->token->attribute.integer = 3;
        exprTree->isExpression = true;
        left->exprTree = exprTree;

        root->left = left;

        return root;
        break;

    // Function call
    case 2:
        root->token = createToken(TOKEN_TYPE_IDENTIFIER);
        root->token->attribute.string = malloc(5);
        strcpy(root->token->attribute.string, "fooo");

        root->left = initASTNode();
        root->left->token = createToken(TOKEN_TYPE_EXPR);
        AST *exprTree1 = initAST();
        ASTNode *exprTreeRoot = initASTNode();
        exprTreeRoot->token = createToken(TOKEN_TYPE_DOUBLE_VALUE);
        exprTreeRoot->token->attribute.decimal = 3.14;
        exprTree1->root = exprTreeRoot;
        exprTree1->isExpression = true;
        root->left->exprTree = exprTree1;

        root->left->right = initASTNode();
        root->left->right->token = createToken(TOKEN_TYPE_EXPR);
        AST *exprTree2 = initAST();
        ASTNode *exprTreeRoot2 = initASTNode();
        exprTreeRoot2->token = createToken(TOKEN_TYPE_INTEGER_VALUE);
        exprTreeRoot2->token->attribute.decimal = 100;
        exprTree2->root = exprTreeRoot2;
        exprTree2->isExpression = true;
        root->left->right->exprTree = exprTree2;

        return root;
        break;

    // Function definition
    case 3:

        root->token = createToken(TOKEN_TYPE_KEYWORD);
        root->token->attribute.keyword = KEYWORD_PUB;

        root->left = initASTNode();
        root->left->token = createToken(TOKEN_TYPE_KEYWORD);
        root->left->token->attribute.keyword = KEYWORD_FN;

        root->left->right = initASTNode();
        root->left->right->token = createToken(TOKEN_TYPE_KEYWORD);
        root->left->right->token->attribute.keyword = KEYWORD_F_64;

        root->left->left = initASTNode();
        root->left->left->token = createToken(TOKEN_TYPE_IDENTIFIER);
        root->left->left->token->attribute.string = malloc(5);
        strcpy(root->left->left->token->attribute.string, "bar");

        root->left->left->left = initASTNode();
        root->left->left->left->token = createToken(TOKEN_TYPE_IDENTIFIER);
        root->left->left->left->token->attribute.string = malloc(2);
        strcpy(root->left->left->left->token->attribute.string, "a");
        root->left->left->left->left = initASTNode();
        root->left->left->left->left->token = createToken(TOKEN_TYPE_KEYWORD);
        root->left->left->left->left->token->attribute.keyword = KEYWORD_F_64;

        root->left->left->left->right = initASTNode();
        root->left->left->left->right->token = createToken(TOKEN_TYPE_IDENTIFIER);
        root->left->left->left->right->token->attribute.string = malloc(2);
        strcpy(root->left->left->left->right->token->attribute.string, "b");
        root->left->left->left->right->left = initASTNode();
        root->left->left->left->right->left->token = createToken(TOKEN_TYPE_KEYWORD);
        root->left->left->left->right->left->token->attribute.keyword = KEYWORD_F_64;

        root->left->left->right = initASTNode();
        root->left->left->right->token = createToken(TOKEN_TYPE_KEYWORD);
        root->left->left->right->token->attribute.keyword = KEYWORD_RETURN;
        root->left->left->right->exprTree = initAST();
        root->left->left->right->exprTree->root = initASTNode();
        root->left->left->right->exprTree->root->token = createToken(TOKEN_TYPE_PLUS);
        root->left->left->right->exprTree->root->left = initASTNode();
        root->left->left->right->exprTree->root->left->token = createToken(TOKEN_TYPE_DOUBLE_VALUE);
        root->left->left->right->exprTree->root->left->token->attribute.decimal = 3.14;
        root->left->left->right->exprTree->root->right = initASTNode();
        root->left->left->right->exprTree->root->right->token = createToken(TOKEN_TYPE_PLUS);
        root->left->left->right->exprTree->root->right->left = initASTNode();
        root->left->left->right->exprTree->root->right->left->token =
            createToken(TOKEN_TYPE_IDENTIFIER);
        root->left->left->right->exprTree->root->right->left->token->attribute.string = malloc(2);
        strcpy(root->left->left->right->exprTree->root->right->left->token->attribute.string, "a");
        root->left->left->right->exprTree->root->right->right = initASTNode();
        root->left->left->right->exprTree->root->right->right->token =
            createToken(TOKEN_TYPE_IDENTIFIER);
        root->left->left->right->exprTree->root->right->right->token->attribute.string = malloc(2);
        strcpy(root->left->left->right->exprTree->root->right->right->token->attribute.string, "b");

        return root;
        break;

    // If
    case 4:
        break;

    // Null if
    case 5:
        break;

    // While
    case 6:
        break;

    // Null while
    case 7:
        break;

    // Variable definition
    case 8:
        break;

    // Variable assignment
    case 9:
        break;

    // Default case
    default:
        break;
    }
    return NULL;
}