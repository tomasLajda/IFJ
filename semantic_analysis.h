/*
 * IFJ Project
 * @brief Header file for semantic analysis
 *
 * @author Tomáš Lajda - xlajdat00
 *
 */

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "symtable.h"

typedef struct Operand {
    DataType type;
    bool compileTime;
} Operand;

/**
 * @brief Performs semantic analysis on the given input.
 *
 * This function serves as a starting point for semantic analysis.
 * It checks the input for semantic correctness according to the
 * specified rules and constraints of the language.
 *
 */
void semanticAnalysis();

#endif