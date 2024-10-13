/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "dynamic_string.h"
#include "error_codes.h"

#define DYNAMIC_STRING_INITIAL_SIZE 8

void DynamicStringInit(DynamicString *ds) {
    ds->string = NULL;
    ds->length = 0;
    ds->allocated = 0;
}

void DynamicStringFree(DynamicString *ds) {
    free(ds->string);
    ds->string = NULL;
    ds->length = 0;
    ds->allocated = 0;
}

void DynamicStringResize(DynamicString *ds, unsigned size) {
    if (size == 0) {
        fprintf(stderr, "DynamicStringResize: Invalid size\n");
        exit(INTERNAL_ERROR);
    }

    unsigned newSize = (size / DYNAMIC_STRING_INITIAL_SIZE + 1) * DYNAMIC_STRING_INITIAL_SIZE;

    char *newString = realloc(ds->string, newSize);
    if (newString == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }

    if (newSize > ds->allocated) {
        memset(newString + ds->allocated, 0, newSize - ds->allocated);
    }

    ds->allocated = newSize;
    ds->string = newString;

    if (ds->length > ds->allocated) {
        ds->length = ds->allocated - 1;
        ds->string[ds->length] = '\0';
    }
}

void DynamicStringClear(DynamicString *ds) {
    ds->length = 0;
    DynamicStringResize(ds, 1);

    ds->string[0] = '\0';
}

void DynamicStringAddChar(DynamicString *ds, char c) {
    if (ds->length + 2 > ds->allocated) {
        DynamicStringResize(ds, ds->length + 2);
    }

    ds->string[ds->length] = c;
    ds->length++;
    ds->string[ds->length] = '\0';
}

void DynamicStringAddString(DynamicString *ds, const char *string) {
    unsigned len = strlen(string);

    if (ds->length + len + 1 > ds->allocated) {
        DynamicStringResize(ds, ds->length + len + 1);
    }

    memcpy(ds->string + ds->length, string, len);
    ds->length += len;
    ds->string[ds->length] = '\0';
}

void DynamicStringReplaceChar(DynamicString *ds, char c, unsigned index) {
    if (index >= ds->length) {
        fprintf(stderr, "Index %u out of string scope. Length is %u.\n", index, ds->length);
        return;
    }

    ds->string[index] = c;
}

void DynamicStringCopy(const DynamicString *ds, DynamicString *copy) {
    if (ds->length + 1 > copy->allocated) {
        DynamicStringResize(copy, ds->length + 1);
    }

    memcpy(copy->string, ds->string, ds->length);
    copy->length = ds->length;
    copy->string[copy->length] = '\0';
}

bool DynamicStringCompare(const DynamicString *ds, const char *string) {
    if (ds->length != strlen(string)) {
        return false;
    }

    return strcmp(ds->string, string);
}

void DynamicStringPrint(const DynamicString *ds) {
    fprintf(stdout, 'String: %s\n', ds->string);
    fprintf(stdout, 'Length: %d\n', ds->length);
    fprintf(stdout, 'Allocated: %d\n', ds->allocated);
}

char *DynamicStringToCString(const DynamicString *ds) {
    if (ds == NULL || ds->length == 0) {
        return NULL;
    }

    char *copy = malloc(ds->length + 1);
    if (copy == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }

    strcpy(copy, ds->string);
    copy[ds->length] = '\0';

    return copy;
}