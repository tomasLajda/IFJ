/*
 * IFJ Project
 * @brief Implementation file for binary search tree and symbol table operations.
 *
 * This file contains the definition for functions and structures used
 * for managing a binary search tree and a symbol table.
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "symtable.h"

void treeCreate(BinaryTreeNodePtr *node, Symbol data) {
    *node = malloc(sizeof(BinaryTreeNode));
    if (*node == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    (*node)->data = data;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->height = 1;
}

unsigned treeHeight(BinaryTreeNodePtr node) { return node == NULL ? 0 : node->height; }

int treeBalanceFactor(BinaryTreeNodePtr node) {
    return node == NULL ? 0 : treeHeight(node->left) - treeHeight(node->right);
}

void treeUpdateHeight(BinaryTreeNodePtr node) {
    unsigned leftHeight = treeHeight(node->left);
    unsigned rightHeight = treeHeight(node->right);
    node->height = leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
}

void treeRotateLeft(BinaryTreeNodePtr *node) {
    BinaryTreeNodePtr right = (*node)->right;
    (*node)->right = right->left;
    right->left = *node;
    *node = right;

    treeUpdateHeight((*node)->left);
    treeUpdateHeight(*node);
}

void treeRotateRight(BinaryTreeNodePtr *node) {
    BinaryTreeNodePtr left = (*node)->left;
    (*node)->left = left->right;
    left->right = *node;
    *node = left;

    treeUpdateHeight((*node)->right);
    treeUpdateHeight(*node);
}

void treeRebalance(BinaryTreeNodePtr *node) {
    if (*node == NULL) {
        return;
    }

    int balanceFactor = treeBalanceFactor(*node);

    if (balanceFactor > 1) {
        if (treeBalanceFactor((*node)->left) < 0) {
            treeRotateLeft(&(*node)->left);
        }
        treeRotateRight(node);
    } else if (balanceFactor < -1) {
        if (treeBalanceFactor((*node)->right) > 0) {
            treeRotateRight(&(*node)->right);
        }
        treeRotateLeft(node);
    }
}

void treeDispose(BinaryTreeNodePtr node) {
    if (node == NULL) {
        return;
    }

    if (node->left != NULL) {
        treeDispose(node->left);
    }
    if (node->right != NULL) {
        treeDispose(node->right);
    }

    if (node->data.params != NULL) {
        listDispose(node->data.params);
    }

    free(node);
}

BinaryTreeNodePtr treeMinValueNode(BinaryTreeNodePtr node) {
    BinaryTreeNodePtr current = node;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

bool treeSearch(BinaryTreeNodePtr node, const char *key) {
    if (node == NULL) {
        return false;
    }

    while (node != NULL) {
        int compare = strcmp(key, node->data.key);
        if (compare == 0) {
            return true;
        } else if (compare < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return false;
}

BinaryTreeNodePtr treeInsert(BinaryTreeNodePtr node, Symbol data) {
    if (node == NULL) {
        treeCreate(&node, data);
        return node;
    }

    if (strcmp(data.key, node->data.key) < 0) {
        node->left = treeInsert(node->left, data);
    } else {
        node->right = treeInsert(node->right, data);
    }

    treeUpdateHeight(node);
    treeRebalance(&node);

    return node;
}

BinaryTreeNodePtr treeDelete(BinaryTreeNodePtr node, const char *key) {
    if (node == NULL) {
        return node;
    }

    if (strcmp(key, node->data.key) < 0) {
        node->left = treeDelete(node->left, key);
    } else if (strcmp(key, node->data.key) > 0) {
        node->right = treeDelete(node->right, key);
    } else {
        if (node->left == NULL || node->right == NULL) {
            BinaryTreeNodePtr temp = node->left ? node->left : node->right;

            if (temp == NULL) {
                temp = node;
                node = NULL;
            } else {
                *node = *temp;
            }

            free(temp->data.key);
            if (temp->data.function) {
                listDispose(temp->data.params);
            }
            free(temp);
        } else {
            BinaryTreeNodePtr temp = treeMinValueNode(node->right);

            node->data = temp->data;
            node->right = treeDelete(node->right, temp->data.key);
        }
    }

    if (node == NULL) {
        return node;
    }

    treeUpdateHeight(node);
    treeRebalance(&node);

    return node;
}

void treeReassign(BinaryTreeNodePtr node, const char *key, Symbol data) {
    if (node == NULL) {
        return;
    }

    while (node != NULL) {
        int compare = strcmp(key, node->data.key);
        if (compare == 0) {
            node->data = data;
            return;
        } else if (compare < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
}

Symbol *treeGet(BinaryTreeNodePtr node, const char *key) {
    while (node != NULL) {
        int compare = strcmp(key, node->data.key);
        if (compare == 0) {
            return &node->data;
        } else if (compare < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return NULL;
}

void printSpaces(int count) {
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

void treePrint(BinaryTreeNodePtr node, int level) {
    if (node == NULL) {
        return;
    }

    treePrint(node->right, level + 1);

    printSpaces(level * 4);
    printf("%s\n", node->data.key);

    treePrint(node->left, level + 1);
}

void treeCheckUsed(BinaryTreeNodePtr node) {
    if (node == NULL) {
        return;
    }

    treeCheckUsed(node->left);

    if (!node->data.used) {
        HANDLE_ERROR("Variable is declared but not used", UNUSED_VARIABLE_ERROR);
    }

    treeCheckUsed(node->right);
}

void treeCheckChanged(BinaryTreeNodePtr node) {
    if (node == NULL) {
        return;
    }

    treeCheckChanged(node->left);

    if (!node->data.changed && !node->data.constant) {
        HANDLE_ERROR("Variable is not modified", UNUSED_VARIABLE_ERROR);
    }

    treeCheckChanged(node->right);
}

bool symbolTableSearch(SymbolTable *table, const char *key) {
    bool found = false;

    while (!found) {
        found = treeSearch(table->root, key);

        if (table->previousTable != NULL) {
            table = table->previousTable;
        } else {
            break;
        }
    }

    return found;
}

void symbolTableInsert(SymbolTable *table, Symbol data) {
    table->root = treeInsert(table->root, data);
}

void symbolTableDispose(SymbolTable *table) {
    if (table->root != NULL) {
        treeDispose(table->root);
    }
}

SymbolTable *symbolTableGetPrevious(SymbolTable *table) { return table->previousTable; }

void symbolTableInit(SymbolTable *table, SymbolTable *previousTable) {
    table->root = NULL;
    table->previousTable = previousTable;
}

void symbolTableSetFunctionKey(SymbolTable *table, char *functionKey) {
    table->functionKey = functionKey;
}

void symbolTableDelete(SymbolTable *table, const char *key) {
    table->root = treeDelete(table->root, key);
}

void symbolTableReassign(SymbolTable *table, const char *key, Symbol data) {
    treeReassign(table->root, key, data);
}

void symbolTableSetUsed(SymbolTable *table, const char *key) {
    Symbol *symbol = NULL;

    while (symbol == NULL) {
        symbol = treeGet(table->root, key);

        if (table->previousTable != NULL) {
            table = table->previousTable;
        } else {
            break;
        }
    }

    if (symbol == NULL) {
        HANDLE_ERROR("Symbol not found", UNDEFINED_ERROR);
    }

    symbol->used = true;
}

void symbolTableSetChanged(SymbolTable *table, const char *key) {
    Symbol *symbol = NULL;

    while (symbol == NULL) {
        symbol = treeGet(table->root, key);

        if (table->previousTable != NULL) {
            table = table->previousTable;
        } else {
            break;
        }
    }

    if (symbol == NULL) {
        HANDLE_ERROR("Symbol not found", UNDEFINED_ERROR);
    }

    symbol->changed = true;
}

Symbol *symbolTableGetSymbol(SymbolTable *table, const char *key) {
    Symbol *symbol = NULL;

    while (symbol == NULL) {
        symbol = treeGet(table->root, key);

        if (table->previousTable != NULL) {
            table = table->previousTable;
        } else {
            break;
        }
    }

    return symbol;
}

void symbolTablePush(Stack *stack, SymbolTable *table) {
    if (stack == NULL || table == NULL) {
        HANDLE_ERROR("Stack or SymbolTable pointer is NULL", INTERNAL_ERROR);
    }

    StackElement *element = malloc(sizeof(StackElement));
    if (element == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    element->tokenPtr = (Token *)table;
    element->next = stack->top;
    stack->top = element;
}

SymbolTable *symbolTableTop(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }

    if (stack->top == NULL) {
        return NULL;
    }

    return (SymbolTable *)stack->top->tokenPtr;
}

void symbolTablePop(Stack *stack) {
    if (stack == NULL) {
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR);
    }

    if (stack->top == NULL) {
        return;
    }

    StackElement *tmp = stack->top;

    symbolTableDispose((SymbolTable *)tmp->tokenPtr);

    stack->top = tmp->next;
    free(tmp);
}

void symbolTablePrint(SymbolTable *table) { treePrint(table->root, 0); }

void symbolTableCopyFunctionParams(SymbolTable *table, List *params) {
    if (params == NULL) {
        return;
    }

    listFirst(params);
    while (listIsActive(params)) {
        ListData data;
        listGetValue(params, &data);
        Symbol newSymbol;
        symbolSetValues(&newSymbol, data.key, data.type, false, true, false);
        newSymbol.compileTime = false;
        symbolTableInsert(table, newSymbol);
        listNext(params);
    }
}

void symbolSetValues(Symbol *symbol, char *key, DataType type, bool function, bool constant,
                     bool used) {
    symbol->key = key;
    symbol->type = type;
    symbol->function = function;
    symbol->constant = constant;
    symbol->used = used;
    symbol->params = NULL;
}

void symbolResetValues(Symbol *symbol) {
    symbol->key = NULL;
    symbol->type = TYPE_VOID;
    symbol->function = false;
    symbol->constant = false;
    symbol->used = false;
    symbol->changed = false;
    symbol->params = NULL;
    symbol->compileTime = false;
    symbol->value = 0;
}

void symbolTableCheckUsed(SymbolTable *table) {
    if (table == NULL) {
        return;
    }

    if (table->root != NULL) {
        treeCheckUsed(table->root);
    }
}

void symbolTableCheckChanged(SymbolTable *table) {
    if (table == NULL) {
        return;
    }

    if (table->root != NULL) {
        treeCheckChanged(table->root);
    }
}