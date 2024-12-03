/*
 * IFJ Project
 * @brief Header file for dynamic string operations.
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#include "dynamic_string.h"
#include "error_codes.h"

#define DYNAMIC_STRING_INITIAL_SIZE 8

void dynamicStringInit(DynamicString *ds) {
    ds->string = NULL;
    ds->length = 0;
    ds->allocated = 0;
}

void dynamicStringFree(DynamicString *ds) {
    free(ds->string);
    ds->string = NULL;
    ds->length = 0;
    ds->allocated = 0;
}

void dynamicStringResize(DynamicString *ds, unsigned size) {
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

void dynamicStringAddChar(DynamicString *ds, char c) {
    if (ds->length + 2 > ds->allocated) {
        dynamicStringResize(ds, ds->length + 2);
    }

    ds->string[ds->length] = c;
    ds->length++;
    ds->string[ds->length] = '\0';
}

void dynamicStringAddString(DynamicString *ds, const char *string) {
    unsigned len = strlen(string);

    if (ds->length + len + 1 > ds->allocated) {
        dynamicStringResize(ds, ds->length + len + 1);
    }

    memcpy(ds->string + ds->length, string, len);
    ds->length += len;
    ds->string[ds->length] = '\0';
}

void dynamicStringReplaceChar(DynamicString *ds, char c, unsigned index) {
    if (index >= ds->length) {
        fprintf(stderr, "Index %u out of string scope. Length is %u.\n", index, ds->length);
        return;
    }

    ds->string[index] = c;
}

void dynamicStringCopy(const DynamicString *ds, DynamicString *copy) {
    if (ds->length + 1 > copy->allocated) {
        dynamicStringResize(copy, ds->length + 1);
    }

    memcpy(copy->string, ds->string, ds->length);
    copy->length = ds->length;
    copy->string[copy->length] = '\0';
}

bool dynamicStringCompare(const DynamicString *ds, const char *string) {
    return !strcmp(ds->string, string);
}

void dynamicStringPrint(const DynamicString *ds) {
    fprintf(stdout, "String: %s\n", ds->string);
    fprintf(stdout, "Length: %d\n", ds->length);
    fprintf(stdout, "Allocated: %d\n", ds->allocated);
}

char *dynamicStringToCString(const DynamicString *ds) {
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