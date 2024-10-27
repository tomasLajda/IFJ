/*
 * IFJ Project
 * @brief Implementation file for binary search tree
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
    if (node->left != NULL) {
        treeDispose(node->left);
    }
    if (node->right != NULL) {
        treeDispose(node->right);
    }

    free(node->data.key);

    if (node->data.function) {
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

bool symbolTableSearch(SymbolTable *table, const char *key) { return treeSearch(table->root, key); }

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

void symbolTableDelete(SymbolTable *table, const char *key) {
    table->root = treeDelete(table->root, key);
}

void symbolTableReassign(SymbolTable *table, const char *key, Symbol data) {
    treeReassign(table->root, key, data);
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
        HANDLE_ERROR("Stack pointer is NULL", INTERNAL_ERROR, NULL);
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

    stack->top = stack->top->next;
    free(tmp);
}