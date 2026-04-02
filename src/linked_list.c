/*
 * IFJ Project
 * @brief Implementation file for linked list contains functions implementation for linked list
 * operations.
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "linked_list.h"

void listInit(List *list) {
    list->activeElement = NULL;
    list->firstElement = NULL;
    list->currentLength = 0;
}

void listDispose(List *list) {
    while (list->firstElement != NULL) {
        ListElementPtr temp = list->firstElement->nextElement;

        free(list->firstElement);

        list->firstElement = temp;
    }

    list->activeElement = NULL;
    list->currentLength = 0;
}

void listInsertFirst(List *list, ListData data) {
    ListElementPtr temp = list->firstElement;

    list->firstElement = malloc(sizeof(ListElement));
    if (list->firstElement == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    list->firstElement->data = data;
    list->firstElement->nextElement = temp;
    list->currentLength++;
}

void listFirst(List *list) { list->activeElement = list->firstElement; }

void listGetFirst(List *list, ListData *dataPtr) {
    if (list->firstElement == NULL) {
        HANDLE_ERROR("First element is NULL", INTERNAL_ERROR);
    }

    *dataPtr = list->firstElement->data;
}

void listDeleteFirst(List *list) {
    if (list->firstElement == NULL)
        return;

    if (list->activeElement == list->firstElement) {
        list->activeElement = NULL;
    }

    ListElementPtr temp = list->firstElement->nextElement;

    free(list->firstElement);

    list->firstElement = temp;
    list->currentLength--;
}

void listDeleteAfter(List *list) {
    if (list->activeElement == NULL || list->activeElement->nextElement == NULL)
        return;

    ListElementPtr temp = list->activeElement->nextElement->nextElement;

    free(list->activeElement->nextElement);

    list->activeElement->nextElement = temp;
    list->currentLength--;
}

void listInsertAfter(List *list, ListData data) {
    if (list->activeElement == NULL)
        return;

    ListElementPtr temp = list->activeElement->nextElement;

    list->activeElement->nextElement = malloc(sizeof(ListElement));
    if (list->activeElement->nextElement == NULL) {
        HANDLE_ERROR("Memory allocation failed", INTERNAL_ERROR);
    }

    list->activeElement->nextElement->data = data;
    list->activeElement->nextElement->nextElement = temp;
    list->currentLength++;
}

void listGetValue(List *list, ListData *data) {
    if (list->activeElement == NULL) {
        HANDLE_ERROR("Active element is NULL", INTERNAL_ERROR);
    }

    *data = list->activeElement->data;
}

void listSetValue(List *list, ListData data) {
    if (list->activeElement == NULL)
        return;

    list->activeElement->data = data;
}

void listNext(List *list) {
    if (list->activeElement == NULL)
        return;

    list->activeElement = list->activeElement->nextElement;
}

int listIsActive(List *list) { return list->activeElement != NULL; }
