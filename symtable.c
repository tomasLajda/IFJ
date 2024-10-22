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
    unsigned leftHeight = height(node->left);
    unsigned rightHeight = height(node->right);
    node->height = leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
}

void treeRotateLeft(BinaryTreeNodePtr *node) {
    BinaryTreeNodePtr right = (*node)->right;
    (*node)->right = right->left;
    right->left = *node;
    *node = right;
}

void treeRotateRight(BinaryTreeNodePtr *node) {
    BinaryTreeNodePtr left = (*node)->left;
    (*node)->left = left->right;
    left->right = *node;
    *node = left;
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

void symbolTableInit(SymbolTable *table, SymbolTable *previousTable) {
    table->root = NULL;
    table->previousTable = previousTable;
}

void symbolTableDispose(SymbolTable *table) {
    if (table->root != NULL) {
        treeDispose(table->root);
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

bool symbolTableSearch(SymbolTable *table, const char *key) {
    BinaryTreeNodePtr current = table->root;

    while (current != NULL) {
        int compare = strcmp(key, current->data.key);
        if (compare == 0) {
            return true;
        } else if (compare < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return false;
}

void symbolTableInsert(SymbolTable *table, Symbol data) {
    if (table->root == NULL) {
        treeCreate(&table->root, data);
        return;
    }

    BinaryTreeNodePtr previous = NULL;
    BinaryTreeNodePtr current = table->root;

    bool found = false;
    do {
        int compare = strcmp(data.key, current->data.key);
        if (compare == 0) {
            HANDLE_ERROR("Redefinition of variable or function", REDEFINITION_ERROR);
        }

        if (compare < 0) {
            if (current->left == NULL) {
                found = true;
            } else {
                previous = current;
                current = current->left;
            }
        } else {
            if (current->right == NULL) {
                found = true;
            } else {
                previous = current;
                current = current->right;
            }
        }
    } while (found);

    if (strcmp(data.key, current->data.key) < 0) {
        treeCreate(&current->left, data);
    } else {
        treeCreate(&current->right, data);
    }
    treeUpdateHeight(current);

    treeRebalance(&previous);
}
