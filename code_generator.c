/*
IFJ project

@brief Code generator implementation

@author Vojtěch Gajdušek - xgajduv00
*/
#include "code_generator.h"
#include "enums.h"
#include "error_codes.h"
#include "helpers.h"
#include <math.h>
#include <stdio.h>

#define ADD_TO_BUFFER(_code) dynamicStringAddString(&codeBuffer, (const char *)(_code))

DynamicString codeBuffer;
int labelCounter = 0;
int expressionCounter = 0;
int tempVarCounter = 0;

void addLabelToBuffer(const char *labelType, const char *suffix) {
    // Static buffer for label
    char label[100]; // enough space for label

    // Construct label name in the format $<labelType>_<suffix>_<labelCounter>
    snprintf(label, sizeof(label), "$%s_%s_%d\n", labelType, suffix, labelCounter);

    ADD_TO_BUFFER(label);
}

void addFloatToBuffer(double value) {
    ADD_TO_BUFFER("float@");
    int enoughSpaceForDouble = 64;
    char doubleStr[enoughSpaceForDouble];
    snprintf(doubleStr, sizeof(doubleStr), "%a", value);
    ADD_TO_BUFFER(doubleStr);
    ADD_TO_BUFFER(" \n");
}

void addIntToBuffer(int value) {
    ADD_TO_BUFFER("int@");
    int enoughSpaceForInt = (int)((ceil(log10(value)) + 1) * sizeof(char));
    char intStr[enoughSpaceForInt];
    snprintf(intStr, sizeof(intStr), "%d", value);
    ADD_TO_BUFFER(intStr);
    ADD_TO_BUFFER(" \n");
}

// TODO: function for generating unique labels

// TODO : function for generating unique temporary variables? mozna nebude potreba - diky pouziti
// datoveho zasobniku pro expressions

int generateCode(FILE *outputFile, AST *ast) {
    if (ast == NULL) {
        return INTERNAL_ERROR;
    }
    if (outputFile == NULL) {
        return INTERNAL_ERROR;
    }

    dynamicStringInit(&codeBuffer);

    generateCodeHeader();

    // Traverse all nodes to the right of the root - functions and generate code for them
    ASTNode *currentNode = ast->root;
    // currentnode - pub
    // currentnode->left - fn
    // currentnode->left->left - function name
    while (currentNode != NULL) {
        if (strcmp(currentNode->left->left->token->attribute.string, "main") == 0) {
            mainStart();
            generateFuncBody(currentNode->left->left);
            mainEnd();
        } else {
            functionStart(currentNode->left->token->attribute.string);
            generateFuncBody(currentNode->left->left);
            functionEnd(currentNode->left->token->attribute.string);
        }

        currentNode = currentNode->right;
    }

    ADD_TO_BUFFER("$$END\n");
    codeGeneratorFlush(outputFile);

    dynamicStringFree(&codeBuffer);
    return 0;
}

int generateCodeHeader() {
    ADD_TO_BUFFER("# Header\n");
    ADD_TO_BUFFER(".IFJcode24\n");
    generateBuiltInFunctions();
    ADD_TO_BUFFER("JUMP $$main\n");
    ADD_TO_BUFFER("# End of Header\n");
    return 0;
}

void generateBuiltInFunctions() {
    ADD_TO_BUFFER("# Built-in functions\n");
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_STRING);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_READ);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_WRITE);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_I2F);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_F2I);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_CONCAT);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_SUBSTRING);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_ORD);
    ADD_TO_BUFFER(BUILT_IN_FUNCTION_CHR);
    ADD_TO_BUFFER("# End of Built-in functions\n");
}

void generateFuncBody(ASTNode *node) {

    // DEFVAR for all variables
    ASTNode *currentVarible = node->exprTree->root;
    while (currentVarible != NULL) {
        ADD_TO_BUFFER("DEFVAR LF@");
        ADD_TO_BUFFER(node->token->attribute.string);
        ADD_TO_BUFFER("\n");
        currentVarible = currentVarible->right;
    }

    // FUNCTION PARAMETERS
    ASTNode *currentNode = node->left;
    int paramID = 0;
    while (currentNode != NULL) {
        generateParam(currentNode, paramID);
        currentNode = currentNode->right;
        paramID++;
    }

    // FUNCTION CODE
    currentNode = node->right;

    while (currentNode != NULL) {
        processNode(currentNode);
        currentNode = currentNode->right;
    }
}

void processNode(ASTNode *node) {
    if (node == NULL) {
        // todo: handle error
        HANDLE_ERROR("ASTNode in processNode func (code gen) is NULL", INTERNAL_ERROR);
    }

    switch (node->token->type) {
    case TOKEN_TYPE_IDENTIFIER:
        if (node->isAssignment == true) {
            if (node->left->exprTree->isExpression == true) {
                generateExpression(node->left->exprTree->root);
                ADD_TO_BUFFER("POPS LF@");
                ADD_TO_BUFFER(node->token->attribute.string);
                ADD_TO_BUFFER("\n");
            } else {
                generateFuncCall(node->left->exprTree->root);
                ADD_TO_BUFFER("MOVE LF@");
                ADD_TO_BUFFER(node->token->attribute.string);
                ADD_TO_BUFFER(" TF@%%retval\n");
            }

        } else {
            generateFuncCall(node);
        }
        break;

    case TOKEN_TYPE_KEYWORD:
        if (node->token->attribute.keyword == KEYWORD_IF) {
            if (node->left->token->type == TOKEN_TYPE_NULL_COND) {
                // nullable if
                labelCounter++;
                ADD_TO_BUFFER("DEFVAR LF@");
                ADD_TO_BUFFER(node->left->right->token->attribute.string);
                ADD_TO_BUFFER("\n");
                ASTNode *conditionNode = node->left->left;
                generateExpression(conditionNode->exprTree->root);
                // expression result is on top of the data stack
                ADD_TO_BUFFER("PUSHS nil@nil\n");
                ADD_TO_BUFFER("JUMPIFEQS ");
                addLabelToBuffer("NULL_if", "false");
                ADD_TO_BUFFER("MOVE LF@");
                ADD_TO_BUFFER(node->left->right->token->attribute.string);
                ADD_TO_BUFFER(" LF@");
                ADD_TO_BUFFER(conditionNode->exprTree->root->token->attribute.string);
                processNode(conditionNode->left); // true
                ADD_TO_BUFFER("JUMP ");
                addLabelToBuffer("NULL_if", "end");
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("NULL_if", "false");
                processNode(conditionNode->right); // false
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("NULL_if", "end");

            } else {
                // normal if
                labelCounter++;
                ASTNode *conditionNode = node->left;
                generateExpression(conditionNode->exprTree->root);
                // expression result is on top of the data stack
                ADD_TO_BUFFER("PUSHS bool@false\n");
                ADD_TO_BUFFER("JUMPIFEQS ");
                addLabelToBuffer("if", "false");
                processNode(conditionNode->left); // true
                ADD_TO_BUFFER("JUMP ");
                addLabelToBuffer("if", "end");
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("if", "false");
                processNode(conditionNode->right); // false
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("if", "end");
            }

        } else if (node->token->attribute.keyword == KEYWORD_WHILE) {
            if (node->left->token->type == TOKEN_TYPE_NULL_COND) {
                // nullable while
                labelCounter++;
                ADD_TO_BUFFER("DEFVAR LF@");
                ADD_TO_BUFFER(node->left->right->token->attribute.string);
                ADD_TO_BUFFER("\n");
                ASTNode *conditionNode = node->left->left;
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("NULL_while", "start");
                generateExpression(conditionNode->exprTree->root);
                // expression result is on top of the data stack
                ADD_TO_BUFFER("PUSHS nil@nil\n");
                ADD_TO_BUFFER("JUMPIFEQS ");
                addLabelToBuffer("NULL_while", "end");
                processNode(conditionNode->left); // while body
                ADD_TO_BUFFER("JUMP ");
                addLabelToBuffer("NULL_while", "start");
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("NULL_while", "end");
            } else {
                // normal while
                labelCounter++;
                ASTNode *conditionNode = node->left;
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("while", "start");
                generateExpression(conditionNode->exprTree->root);
                // expression result is on top of the data stack
                ADD_TO_BUFFER("PUSHS bool@false\n");
                ADD_TO_BUFFER("JUMPIFEQS ");
                addLabelToBuffer("while", "end");
                processNode(conditionNode->left); // while body
                ADD_TO_BUFFER("JUMP ");
                addLabelToBuffer("while", "start");
                ADD_TO_BUFFER("LABEL ");
                addLabelToBuffer("while", "end");
            }

        } else if (node->token->attribute.keyword == KEYWORD_UNDERSCORE) {
            // generate function call otherwise do nothing
            if (node->isAssignment == false) {
                generateFuncCall(node);
            }

        } else if (node->token->attribute.keyword == KEYWORD_RETURN) {
            generateExpression(node->exprTree->root);
            ADD_TO_BUFFER("POPS LF@%%retval\n");
        } else if (node->token->attribute.keyword == KEYWORD_WRITE) {
            ADD_TO_BUFFER("CREATEFRAME\n");
            ADD_TO_BUFFER("DEFVAR TF@value\n");
            generateExpression(node->left);
            ADD_TO_BUFFER("POPS TF@value\n");
            ADD_TO_BUFFER("CALL $IFJ24_write\n");
        } else if (node->token->attribute.keyword == KEYWORD_VAR ||
                   node->token->attribute.keyword == KEYWORD_CONST) {
            // skip VARDEF and continue with the assignement
            ASTNode *idNode = node->left;
            if (idNode->right->exprTree->isExpression == true) {
                generateExpression(idNode->right->exprTree->root);
                ADD_TO_BUFFER("POPS LF@");
                ADD_TO_BUFFER(idNode->token->attribute.string);
                ADD_TO_BUFFER("\n");
            } else {
                generateFuncCall(idNode->right->exprTree->root);
                ADD_TO_BUFFER("MOVE LF@");
                ADD_TO_BUFFER(idNode->token->attribute.string);
                ADD_TO_BUFFER(" TF@%%retval\n");
            }
        } else {
        }
        break;

    // TODO: odstranit, sam o sobe nikdy nebude
    case TOKEN_TYPE_EXPR:
        // todo: generateExpression(node);
        break;

    default:
        break;
    }
}

int mainStart() {
    ADD_TO_BUFFER("# Start of Main:\n");
    ADD_TO_BUFFER("$$main\n");
    ADD_TO_BUFFER("CREATEFRAME\n");
    ADD_TO_BUFFER("PUSHFRAME\n");
    return 0;
}

int mainEnd() {
    ADD_TO_BUFFER("POPFRAME\n");
    ADD_TO_BUFFER("CLEARS\n");
    ADD_TO_BUFFER("JUMP $$END\n");

    return 0;
}

int functionStart(char *functionName) {
    ADD_TO_BUFFER("# Start of function ");
    ADD_TO_BUFFER(functionName);
    ADD_TO_BUFFER(":\n");
    ADD_TO_BUFFER("LABEL $");
    ADD_TO_BUFFER(functionName);
    ADD_TO_BUFFER("\n");
    ADD_TO_BUFFER("PUSHFRAME\n");
    return 0;
}

int functionEnd(char *functionName) {
    ADD_TO_BUFFER("POPFRAME\n");
    ADD_TO_BUFFER("RETURN\n");
    ADD_TO_BUFFER("# End of function ");
    ADD_TO_BUFFER(functionName);
    ADD_TO_BUFFER("\n");
    return 0;
}

int generateExpression(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }

    // process left subtree
    generateExpression(node->left);
    // process right subtree
    generateExpression(node->right);

    TokenType currentTokenType = node->token->type;

    if (currentTokenType == TOKEN_TYPE_IDENTIFIER) {
        // Push variable onto the data stack
        ADD_TO_BUFFER("PUSHS LF@");
        ADD_TO_BUFFER(node->token->attribute.string);
        ADD_TO_BUFFER(" \n");

    } else if (currentTokenType == TOKEN_TYPE_INTEGER_VALUE) {
        // Push integer value onto the data stack
        // PUSHS int@value
        ADD_TO_BUFFER("PUSHS ");
        addIntToBuffer(node->token->attribute.integer);
        ADD_TO_BUFFER(" \n");

    } else if (currentTokenType == TOKEN_TYPE_DOUBLE_VALUE) {
        // Push double value onto the data stack
        // PUSHS float@value
        ADD_TO_BUFFER("PUSHS ");
        addFloatToBuffer(node->token->attribute.decimal);
        ADD_TO_BUFFER(" \n");

    } else if (isTokenTypeOperator(currentTokenType)) {
        if (currentTokenType == TOKEN_TYPE_PLUS) {
            ADD_TO_BUFFER("ADDS\n");
        } else if (currentTokenType == TOKEN_TYPE_MINUS) {
            ADD_TO_BUFFER("SUBS\n");
        } else if (currentTokenType == TOKEN_TYPE_MUL) {
            ADD_TO_BUFFER("MULS\n");
        } else if (currentTokenType == TOKEN_TYPE_DIV) {
            ADD_TO_BUFFER("DIVS\n");
        } else if (currentTokenType == TOKEN_TYPE_EQ) {
            ADD_TO_BUFFER("EQS\n");
        } else if (currentTokenType == TOKEN_TYPE_NEQ) {
            ADD_TO_BUFFER("EQS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else if (currentTokenType == TOKEN_TYPE_LTH) {
            ADD_TO_BUFFER("LTS\n");
        } else if (currentTokenType == TOKEN_TYPE_LEQ) {
            ADD_TO_BUFFER("GTS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else if (currentTokenType == TOKEN_TYPE_GTH) {
            ADD_TO_BUFFER("GTS\n");
        } else if (currentTokenType == TOKEN_TYPE_GEQ) {
            ADD_TO_BUFFER("LTS\n");
            ADD_TO_BUFFER("NOTS\n");
        } else {
            return INTERNAL_ERROR;
        }
    } else {
        return INTERNAL_ERROR;
    }

    return 0;
}

int generateFuncCall(ASTNode *node) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }

    if (node->token->type == TOKEN_TYPE_KEYWORD) {
        builtInFuncCall(node);
        return 0;
    }

    // node - function name
    ADD_TO_BUFFER("CREATEFRAME\n");
    ADD_TO_BUFFER("DEFVAR TF@%%retval\n");

    int paramID = 0;
    // generate parameters
    ASTNode *currentNode = node->left;
    // currentNode - each parameter
    while (currentNode != NULL) {
        // TODO u defvaru pouzit unikatni jmeno?
        ADD_TO_BUFFER("DEFVAR TF@%%arg");
        addIntToBuffer(paramID);
        ADD_TO_BUFFER("\n");
        ADD_TO_BUFFER("MOVE TF@%%arg");
        addIntToBuffer(paramID);
        ADD_TO_BUFFER(" ");

        paramID++;

        TokenType currentType = currentNode->exprTree->root->token->type;

        if (currentType == TOKEN_TYPE_IDENTIFIER) {
            ADD_TO_BUFFER("LF@");
            ADD_TO_BUFFER(currentNode->exprTree->root->token->attribute.string);
            ADD_TO_BUFFER(" \n");
        } else if (currentType == TOKEN_TYPE_INTEGER_VALUE) {
            addIntToBuffer(currentNode->exprTree->root->token->attribute.integer);
        } else if (currentType == TOKEN_TYPE_DOUBLE_VALUE) {
            addFloatToBuffer(currentNode->exprTree->root->token->attribute.decimal);
        } else {
            return INTERNAL_ERROR;
        }
        currentNode = currentNode->right;
    }

    ADD_TO_BUFFER("CALL $");
    ADD_TO_BUFFER(node->token->attribute.string);
    ADD_TO_BUFFER(" \n");
    return 0;
}

int generateParam(ASTNode *node, int paramID) {
    if (node == NULL) {
        return INTERNAL_ERROR;
    }
    char *paramName = node->token->attribute.string;

    ADD_TO_BUFFER("DEFVAR LF@");
    ADD_TO_BUFFER(paramName);
    ADD_TO_BUFFER("\n");
    ADD_TO_BUFFER("MOVE LF@");
    ADD_TO_BUFFER(paramName);
    ADD_TO_BUFFER(" LF@%%arg");
    addIntToBuffer(paramID);
    ADD_TO_BUFFER("\n");
    return 0;
}

void builtInFuncCall(ASTNode *node) {
    if (node->token->attribute.keyword == KEYWORD_READSTR) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@type\n");
        ADD_TO_BUFFER("MOVE TF@type string@string\n");
        ADD_TO_BUFFER("CALL $IFJ24_read\n");

    } else if (node->token->attribute.keyword == KEYWORD_READI32) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@type\n");
        ADD_TO_BUFFER("MOVE TF@type string@int\n");
        ADD_TO_BUFFER("CALL $IFJ24_read\n");

    } else if (node->token->attribute.keyword == KEYWORD_READF64) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@type\n");
        ADD_TO_BUFFER("MOVE TF@type string@float\n");
        ADD_TO_BUFFER("CALL $IFJ24_read\n");

    } else if (node->token->attribute.keyword == KEYWORD_STRING) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@value\n");
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@value\n");
        ADD_TO_BUFFER("CALL $IFJ24_string\n");

    } else if (node->token->attribute.keyword == KEYWORD_LENGTH) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@s\n");
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s\n");
        ADD_TO_BUFFER("CALL $IFJ24_length\n");

    } else if (node->token->attribute.keyword == KEYWORD_CONCAT) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@s1\n");
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s1\n");
        ADD_TO_BUFFER("DEFVAR TF@s2\n");
        generateExpression(node->left->right->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s2\n");
        ADD_TO_BUFFER("CALL $IFJ24_concat\n");

    } else if (node->token->attribute.keyword == KEYWORD_SUBSTRING) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@s\n");
        // s is first parameter
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s\n");
        ADD_TO_BUFFER("DEFVAR TF@i\n");
        // i (start index) is second parameter
        generateExpression(node->left->right->exprTree->root);
        ADD_TO_BUFFER("POPS TF@i\n");
        ADD_TO_BUFFER("DEFVAR TF@j\n");
        // j (index after last char) is third parameter
        generateExpression(node->left->right->right->exprTree->root);
        ADD_TO_BUFFER("POPS TF@j\n");
        ADD_TO_BUFFER("CALL $IFJ24_substring\n");

    } else if (node->token->attribute.keyword == KEYWORD_STRCMP) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@s1\n");
        // s1 is first parameter
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s1\n");
        ADD_TO_BUFFER("DEFVAR TF@s2\n");
        // s2 is second parameter
        generateExpression(node->left->right->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s2\n");
        ADD_TO_BUFFER("CALL $IFJ24_strcmp\n");

    } else if (node->token->attribute.keyword == KEYWORD_ORD) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@s\n");
        // s is first parameter
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@s\n");
        ADD_TO_BUFFER("DEFVAR TF@i\n");
        // i is second parameter
        generateExpression(node->left->right->exprTree->root);
        ADD_TO_BUFFER("POPS TF@i\n");
        ADD_TO_BUFFER("CALL $IFJ24_ord\n");

    } else if (node->token->attribute.keyword == KEYWORD_CHR) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@i\n");
        // i is first parameter
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@i\n");
        ADD_TO_BUFFER("CALL $IFJ24_chr\n");

    } else if (node->token->attribute.keyword == KEYWORD_I2F) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@input\n");
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@input\n");
        ADD_TO_BUFFER("CALL $IFJ24_i2f\n");

    } else if (node->token->attribute.keyword == KEYWORD_F2I) {
        ADD_TO_BUFFER("CREATEFRAME\n");
        ADD_TO_BUFFER("DEFVAR TF@input\n");
        generateExpression(node->left->exprTree->root);
        ADD_TO_BUFFER("POPS TF@input\n");
        ADD_TO_BUFFER("CALL $IFJ24_f2i\n");
    }
}

void codeGeneratorFlush(FILE *outputFile) { fputs(codeBuffer.string, outputFile); }