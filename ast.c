/*
 * IFJ Project
 * @brief Implementation file of AST
 *
 * @author Martin Valapka - xvalapm00
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "error_codes.h"

AST* createAST() {
    AST* tree = (AST*) malloc(sizeof(AST));
    if (tree == NULL) {
        HANDLE_ERROR("Memory allocation failed", 99, NULL);
    }
    tree->root = NULL;
    return tree;
}

int main(int argc, char const *argv[]) {
    return 0;
}
