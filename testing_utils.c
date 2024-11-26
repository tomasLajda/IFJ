#include "testing_utils.h"

const char *getTokenTypeName(TokenType type) {
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
        return "IDENTIFIER";
    case TOKEN_TYPE_KEYWORD:
        return "KEYWORD";
    case TOKEN_TYPE_INTEGER_VALUE:
        return "INTEGER_VALUE";
    case TOKEN_TYPE_DOUBLE_VALUE:
        return "DOUBLE_VALUE";
    case TOKEN_TYPE_STRING_VALUE:
        return "STRING_VALUE";
    case TOKEN_TYPE_EQ:
        return "EQUAL";
    case TOKEN_TYPE_NEQ:
        return "NOT_EQUAL";
    case TOKEN_TYPE_LTH:
        return "LESS_THAN";
    case TOKEN_TYPE_LEQ:
        return "LESS_OR_EQUAL";
    case TOKEN_TYPE_GTH:
        return "GREATER_THAN";
    case TOKEN_TYPE_GEQ:
        return "GREATER_OR_EQUAL";
    case TOKEN_TYPE_ASSIGN:
        return "ASSIGN";
    case TOKEN_TYPE_PLUS:
        return "PLUS";
    case TOKEN_TYPE_MINUS:
        return "MINUS";
    case TOKEN_TYPE_MUL:
        return "MULTIPLY";
    case TOKEN_TYPE_DIV:
        return "DIVIDE";
    case TOKEN_TYPE_VB:
        return "VB";
    case TOKEN_TYPE_LEFT_BR:
        return "LEFT_BRACKET";
    case TOKEN_TYPE_RIGHT_BR:
        return "RIGHT_BRACKET";
    case TOKEN_TYPE_LEFT_CURLY_BR:
        return "LEFT_CURLY_BRACE";
    case TOKEN_TYPE_RIGHT_CURLY_BR:
        return "RIGHT_CURLY_BRACE";
    case TOKEN_TYPE_COMMA:
        return "COMMA";
    case TOKEN_TYPE_COLON:
        return "COLON";
    case TOKEN_TYPE_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_TYPE_DOT:
        return "DOT";
    case TOKEN_TYPE_EXPR:
        return "EXPRESSION";
    case TOKEN_TYPE_DOLLA:
        return "DOLLAR";
    default:
        return "UNKNOWN_TOKEN_TYPE";
    }
}

const char *getKeywordName(Keyword keyword) {
    switch (keyword) {
    case KEYWORD_I_32:
        return "i32";
    case KEYWORD_I_32_NULL:
        return "?i32";
    case KEYWORD_F_64:
        return "f64";
    case KEYWORD_F_64_NULL:
        return "?f64";
    case KEYWORD_U_8_ARRAY:
        return "[]u8";
    case KEYWORD_U_8_ARRAY_NULL:
        return "?[]u8";
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
        return "@import";
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

void printTokenInfo(Token *token) {
    if (token == NULL) {
        printf("NULL_TOKEN\n");
        return;
    }

    switch (token->type) {
    case TOKEN_TYPE_IDENTIFIER:
        printf("%s (%s)\n", getTokenTypeName(token->type), token->attribute.string);
        break;

    case TOKEN_TYPE_STRING_VALUE:
        printf("%s (\"%s\")\n", getTokenTypeName(token->type), token->attribute.string);
        break;

    case TOKEN_TYPE_INTEGER_VALUE:
        printf("%s (%lld)\n", getTokenTypeName(token->type), token->attribute.integer);
        break;

    case TOKEN_TYPE_DOUBLE_VALUE:
        printf("%s (%f)\n", getTokenTypeName(token->type), token->attribute.decimal);
        break;

    case TOKEN_TYPE_KEYWORD:
        printf("%s (%s)\n", getTokenTypeName(token->type),
               getKeywordName(token->attribute.keyword));
        break;

    case TOKEN_TYPE_EOL:
        printf("%s\n", getTokenTypeName(token->type));
        break;

    case TOKEN_TYPE_COMMENT:
        printf("%s\n", getTokenTypeName(token->type));
        break;

    case TOKEN_TYPE_EQ:
    case TOKEN_TYPE_NEQ:
    case TOKEN_TYPE_LTH:
    case TOKEN_TYPE_LEQ:
    case TOKEN_TYPE_GTH:
    case TOKEN_TYPE_GEQ:
    case TOKEN_TYPE_ASSIGN:
    case TOKEN_TYPE_PLUS:
    case TOKEN_TYPE_MINUS:
    case TOKEN_TYPE_MUL:
    case TOKEN_TYPE_DIV:
    case TOKEN_TYPE_VB:
        printf("%s\n", getTokenTypeName(token->type));
        break;

    case TOKEN_TYPE_LEFT_BR:
    case TOKEN_TYPE_RIGHT_BR:
    case TOKEN_TYPE_LEFT_CURLY_BR:
    case TOKEN_TYPE_RIGHT_CURLY_BR:
    case TOKEN_TYPE_COMMA:
    case TOKEN_TYPE_COLON:
    case TOKEN_TYPE_SEMICOLON:
    case TOKEN_TYPE_DOT:
    case TOKEN_TYPE_DOLLA:
        printf("%s\n", getTokenTypeName(token->type));
        break;

    case TOKEN_TYPE_EOF:
        printf("%s\n", getTokenTypeName(token->type));
        break;

    default:
        if (token->attribute.string != NULL) {
            printf("UNKNOWN TOKEN (\"%s\")\n", token->attribute.string);
        }
        break;
    }
}