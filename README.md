# IFJ24 Compiler

A complete, full-stack compiler pipeline implemented in C, translating a statically typed, Zig-inspired language (`IFJ24`) into a stack-oriented virtual machine bytecode (`IFJcode24`).

Built as a university project for the IFJ (Formal Languages and Compilers) course, it covers multiple computer science disciplines: finite-state machine tokenization, formal grammar evaluation (both recursive-descent and shift-reduce parsing), semantic analysis with self-balancing tree scopes, and low-level code generation.

## Architecture

The compiler operates as a strictly linear single-pass pipeline. Each stage acts as an isolated transformation step with a single entry point and defined output contract.

```txt
stdin source
  ->  Scanner          (Finite-state machine, emits typed Tokens)
  ->  Parser           (Recursive-descent on LL grammar + shift-reduce precedence engine)
  ->  AST              (Shared intermediate tree representation)
  ->  Semantic Pass    (Two-sweep: function signatures, then bodies)
       -> Symbol Table (Per-scope AVL trees, lexically chained)
  ->  Code Generator   (Post-order AST walk, emits IFJcode24 stack instructions)
  ->  stdout VM Bytecode
```

### 1. Lexical Analysis

The scanner is a hand-written 25-state finite automaton. It reads `stdin` sequentially and emits strictly typed tokens. Complex lexical rules—such as interpreting Zig-style multiline strings and numeric formatting constraints—are resolved immediately, ensuring downstream stages receive a clean mathematical vocabulary stripped of spaces and comments.

### 2. Syntactic Analysis (Parsing)

Tree construction is handled by two distinct parsing engines, allowing us to avoid deeply nested recursive structures for simple expressions:

- **Recursive-Descent:** Handles the LL program structure and statement bodies (functions, `if`/`else`, `while` loops) using a one-function-per-nonterminal approach.
- **Precedence-Based Shift-Reduce:** Invoked for expressions. It utilizes a 14x14 precedence table to dictate stack shifts and reductions without polluting the statement grammar with operator priority rules.

### 3. Semantic Analysis

Performs two complete traversals of the Abstract Syntax Tree (AST):

- **First pass:**  Analyzes function signatures to support mutual recursion without forward declarations.
- **Second pass:** Validates constraints on the function bodies.

Scopes are managed using **AVL trees** (self-balancing binary search trees). This provides $O(\log n)$ variable lookups and allows $O(n)$ in-order traversals at scope exit to verify constraints like "no dead variable" and initialization policies. The semantic phase also renames local variables to avoid collisions in the target VM's flat memory frame and statically evaluates binary operations (e.g., detecting division by zero at compile time).

### 4. Code Generation

Flattens the AST into a one-dimensional stream of `IFJcode24` instructions. Expressions are evaluated via post-order traversal to conform to the stack-machine execution model. The generator bridges language semantics with primitive operations, performing tasks like runtime type dispatch for division (`DIVS` vs `IDIVS`) and unrolling `if (expr) |val|` null-unwrapping into conditional jumps.

Standard library functions are injected dynamically into the output as IFJcode24 assembly blocks when required.

## Hello World Example

To illustrate the pipeline, here is how a simple "Hello, World!" program is compiled:

### 1. Source Code (`IFJ24`)

```rust
const ifj = @import("ifj24.zig");
pub fn main() void {
    ifj.write("Hello, World!\n");
}
```

### 2. Tokens (Scanner output)

```txt
KEYWORD(const) ID(ifj) ASSIGN BUILTIN(@import) STRING("ifj24.zig") SEMICOLON
KEYWORD(pub) KEYWORD(fn) ID(main) LPAREN RPAREN KEYWORD(void) LBRACE
ID(ifj) DOT ID(write) LPAREN STRING("Hello, World!\n") RPAREN SEMICOLON
RBRACE EOF
```

### 3. Abstract Syntax Tree (Parser output)

```txt
Program
└── FunctionDef (main, void)
    └── Call (ifj.write)
        └── Arg: String("Hello, World!\n")
```

### 4. IFJcode24 Bytecode (Code Generator output)

```txt
.IFJcode24
JUMP $$main
LABEL $$main
CREATEFRAME
PUSHFRAME
WRITE string@Hello,\032World!\010
POPFRAME
CLEARS
```

## Tech Stack

- **Implementation Language:** C (Standard library only, no external dependencies)
- **Tooling:** CMake, Make
- **Testing:** C unit tests (for symtables/semantics) and Python (for integration testing)
- **Target Architecture:** IFJcode24 virtual machine instructions

## Build and Run

### Prerequisites

- GCC / Clang
- Make / CMake
- Python 3+ (for integration tests)

### Quick Start

```bash
# Build the ifj_compiler binary into bin/
make

# Run the complete test suite (unit + integration)
make test

# Pack the project for course submission
make pack
```

## Team

Built by:

| Name             | Main focus                                                      |
|------------------|-----------------------------------------------------------------|
| Matúš Csirik     | Team lead, Expression parser, Syntactic analysis, QA, Utilities |
| Tomáš Lajda      | Semantic analysis, Data structures, QA, Repository management   |
| Vojtěch Gajdušek | Lexical analysis, Code generator                                |
| Martin Valapka   | Syntactic analysis, Documentation                               |
