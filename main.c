#include "ast.h"
#include "error_codes.h"
#include "expr-parser.h" // Ensure this header includes declarations for parseExpression and AST functions
#include "helpers.h"
#include "scanner.h"
#include "stack.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

FILE *sourceFile;

int main() {
    // Open the source file containing expressions to parse
    sourceFile = fopen("test.txt", "r");
    if (sourceFile == NULL) {
        fprintf(stderr, "Error: Unable to open source file.\n");
        return EXIT_FAILURE;
    }

    int parseResult = 0;
    int expressionCount = 0;

    // Initialize the main AST
    AST *mainAST = initAST();
    if (mainAST == NULL) {
        fprintf(stderr, "Error: Failed to initialize main AST.\n");
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }

    // Create the root node of the main AST
    ASTNode *rootNode = initASTNode();
    if (rootNode == NULL) {
        fprintf(stderr, "Error: Failed to initialize root AST node.\n");
        freeAST(mainAST);
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }
    mainAST->root = rootNode;

    // Initialize the root node's expression tree
    rootNode->exprTree = initAST();
    if (rootNode->exprTree == NULL) {
        fprintf(stderr, "Error: Failed to initialize root expression AST.\n");
        disposeSubtree(rootNode);
        free(mainAST);
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }

    // Initialize the root expression AST's root node
    ASTNode *rootExprNode = initASTNode();
    if (rootExprNode == NULL) {
        fprintf(stderr, "Error: Failed to initialize root expression AST node.\n");
        freeAST(mainAST);
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }
    rootNode->exprTree->root = rootExprNode;

    // Create an initial empty token and assign it to the root expression node
    Token *initialToken = createToken(TOKEN_TYPE_EMPTY);
    if (initialToken == NULL) {
        fprintf(stderr, "Error: Failed to create initial token.\n");
        disposeSubtree(rootNode);
        freeAST(mainAST);
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }
    rootExprNode->token = initialToken;

    // Variable to hold the current token
    Token *currentToken = createToken(TOKEN_TYPE_EMPTY);
    if (currentToken == NULL) {
        fprintf(stderr, "Error: Failed to create current token.\n");
        disposeSubtree(rootNode);
        freeAST(mainAST);
        fclose(sourceFile);
        return INTERNAL_ERROR;
    }

    // Loop to parse multiple expressions until EOF is reached
    while (currentToken->type != TOKEN_TYPE_EOF) {
        // Parse the current expression and build its AST
        Token *token1 = createToken(TOKEN_TYPE_DOUBLE_VALUE);
        Token *token2 = createToken(TOKEN_TYPE_PLUS);
        parseResult = parseExpression(rootNode->exprTree, token1, token2, currentToken);

        if (parseResult == SYNTAX_ERROR) {
            printf("Syntax error encountered.\n");
            printf("%d. Delimiter token type: %s\n", ++expressionCount,
                   TokenTypeToString(currentToken->type));
        } else if (parseResult == INTERNAL_ERROR) {
            printf("Internal error encountered.\n");
            printf("%d. Delimiter token type: %s\n", ++expressionCount,
                   TokenTypeToString(currentToken->type));
        } else { // Successful parsing
            printf("Expression %d parsed successfully.\n", ++expressionCount);
            displayAST(rootNode->exprTree);

            // Attach the successfully parsed expression AST as a left node
            ASTNode *newLeftNode = initASTNode();
            if (newLeftNode == NULL) {
                fprintf(stderr, "Error: Failed to initialize new left AST node.\n");
                disposeSubtree(rootNode);
                freeAST(mainAST);
                free(currentToken);
                fclose(sourceFile);
                return INTERNAL_ERROR;
            }
            newLeftNode->exprTree = rootNode->exprTree; // Link the parsed expression AST

            // Add the new left node to the main AST
            addLeftNode(mainAST, rootNode, newLeftNode);

            // Move the rootNode pointer to the new left node for the next expression
            rootNode = newLeftNode;

            // Re-initialize the expression AST for the next expression
            rootNode->exprTree = initAST();
            if (rootNode->exprTree == NULL) {
                fprintf(stderr, "Error: Failed to initialize new expression AST.\n");
                disposeSubtree(mainAST->root);
                freeAST(mainAST);
                free(currentToken);
                fclose(sourceFile);
                return INTERNAL_ERROR;
            }

            // Initialize the new expression AST's root node
            ASTNode *newExprRoot = initASTNode();
            if (newExprRoot == NULL) {
                fprintf(stderr, "Error: Failed to initialize new expression AST root node.\n");
                disposeSubtree(mainAST->root);
                freeAST(mainAST);
                free(currentToken);
                fclose(sourceFile);
                return INTERNAL_ERROR;
            }
            rootNode->exprTree->root = newExprRoot;

            // Assign an empty token to the new expression AST's root node
            Token *emptyToken = createToken(TOKEN_TYPE_EMPTY);
            if (emptyToken == NULL) {
                fprintf(stderr, "Error: Failed to create empty token for new expression AST.\n");
                disposeSubtree(mainAST->root);
                freeAST(mainAST);
                free(currentToken);
                fclose(sourceFile);
                return INTERNAL_ERROR;
            }
            newExprRoot->token = emptyToken;
        }

        // Retrieve the next token from the source file
        // Assuming a function getNextToken exists to fetch the next token from sourceFile
        // This function should update currentToken accordingly
        getNextToken(currentToken);
    }

    // Final display of the constructed main AST
    printf("Final AST Structure:\n");
    displayAST(mainAST);

    // Cleanup: Free all allocated memory and close the source file
    freeAST(mainAST);
    free(currentToken);
    fclose(sourceFile);

    return 0; // Indicate successful execution
}