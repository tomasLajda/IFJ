/*
 * IFJ Project
 * @brief This file contains all the enums used in the project
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef ENUMS_H
#define ENUMS_H

typedef enum {
    TYPE_I_32 = 0,
    TYPE_I_32_NULL,
    TYPE_F_64,
    TYPE_F_64_NULL,
    TYPE_U_8_ARRAY,
    TYPE_U_8_ARRAY_NULL,
    TYPE_VOID,
} DataType;

typedef enum {
    KEYWORD_I_32 = TYPE_I_32,
    KEYWORD_I_32_NULL = TYPE_I_32_NULL,
    KEYWORD_F_64 = TYPE_F_64,
    KEYWORD_F_64_NULL = TYPE_F_64_NULL,
    KEYWORD_U_8_ARRAY = TYPE_U_8_ARRAY,
    KEYWORD_U_8_ARRAY_NULL = TYPE_U_8_ARRAY_NULL,
    KEYWORD_VOID = TYPE_VOID,
    KEYWORD_NULL,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_PUB,
    KEYWORD_FN,
    KEYWORD_RETURN,
    KEYWORD_VAR,
    KEYWORD_CONST,
    KEYWORD_UNDERSCORE,
    KEYWORD_MAIN,
    KEYWORD_IMPORT, // @import("ifj24.zig")
    KEYWORD_IFJ,
    KEYWORD_STRING, // These might be different -> ifj lib
    KEYWORD_LENGTH,
    KEYWORD_CONCAT,
    KEYWORD_SUBSTRING,
    KEYWORD_STRCMP,
    KEYWORD_ORD,
    KEYWORD_CHR,
    KEYWORD_WRITE,
    KEYWORD_READSTR,
    KEYWORD_READI32,
    KEYWORD_READF64,
    KEYWORD_I2F,
    KEYWORD_F2I,
} Keyword;

#endif