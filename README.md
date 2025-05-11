# Hanami: A Nature-Inspired, Multi-Target Transpilation Programming Language

Hanami is an educational programming language and transpiler developed as a student group project for CS3370 at Hanoi University of Science and Technology - Troy Program. Inspired by the Japanese tradition of flower viewing (花見), Hanami features intuitive, nature-themed syntax and a modular architecture, translating source code into C++, Java, JavaScript, and Python. The project demonstrates core concepts in language design and compiler construction, offering an interactive web playground for immediate experimentation.

> Live Playground: https://hanami-fe.netlify.app/
---

## Table of Contents

- [Introduction](#introduction)
- [Philosophy & Design Principles](#philosophy--design-principles)
- [Language Features](#language-features)
- [Getting Started](#getting-started)
- [Syntax Overview](#syntax-overview)
- [Architecture](#architecture)
- [Interactive Playground](#interactive-playground)
- [Examples](#examples)
- [Limitations & Future Work](#limitations--future-work)
- [Team](#team)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Introduction

Hanami explores the fundamentals of programming language design through hands-on implementation. Rather than compiling to machine code, Hanami transpiles to established languages, allowing us to focus on language concepts and observe how features map across platforms. The project bridges the gap between theory and practice, reinforcing compiler construction principles and design trade-off analysis.

For more information, please take a look at our paper: [Hanami_FinalPaper.pdf](https://github.com/kiyo9w/Hanami-CS370/blob/main/Hanami_FinalPaper.pdf)

---

## Philosophy & Design Principles

Hanami's development was guided by these core principles:

- **Aesthetic Clarity:** Code should be visually appealing and readable.
- **Conceptual Harmony:** Features should work together coherently.
- **Educational Transparency:** Mechanisms should be visible, not hidden.
- **Practical Efficiency:** Focus on achievable goals with high learning value.

The language employs a consistent "garden" metaphor, with keywords like `garden`, `species`, `seed`, and `grow` representing familiar programming constructs.

---

## Language Features

- **Multi-Target Transpilation:** Outputs C++, Java, JavaScript, and Python.
- **Nature-Inspired Syntax:** Memorable, metaphorical keywords.
- **Gradual Type System:** Configurable strictness, static/dynamic type exploration.
- **Effect Handling:** Basic support for controlled side effects.
- **Web Playground:** Interactive, browser-based development environment.

---

## Getting Started

### Prerequisites

- C++17-compatible compiler
- `make` build system
- [Optional] Node.js & npm (for playground development)

### Building from Source

```
git clone https://github.com/kiyo9w/Hanami-CS370.git
cd Hanami-CS370/development/MODULES
make
```

### Running the Transpiler

```
make run INPUT_FILE=path/to/example.hanami --target cpp # Or left blank to transpile to all languages
```

### Running Individual Modules

```
cd path/to/module 
make run INPUT_FILE=input/filename{.hanami/.tokens/.ast/.ir} # Left blank to run default input file
```

### To Clean Up For A Fresh Start
```
cd Hanami-CS370/development/MODULES or path/to/module
make clean # This will clean up all execution files and output files.
```

---

## Syntax Overview

Hanami's syntax is designed to be both novel and approachable, mapping directly to C++-like constructs:

| Hanami Keyword    | C++ Equivalent    | Meaning                       |
|-------------------|------------------|-------------------------------|
| `garden`          | `namespace`      | Namespace declaration         |
| `species`         | `class`          | Class declaration             |
| `seed`            | Member variable  | Class property                |
| `grow`            | Function         | Method/function definition    |
| `bloom > x;`     | `std::cin >> x;` | Console input                 |
| `branch (cond)`   | `if (cond)`      | Conditional                   |
| `open:`           | `public:`        | Public access                 |
| `hidden:`         | `private:`       | Private access                |
| `blossom`         | `return`         | Return value                  |

---

## Architecture

Hanami follows a modular pipeline architecture with distinct components:

1. **Lexer**: Converts source code (.hanami) into a stream of tokens
2. **Parser**: Builds an Abstract Syntax Tree (AST) from the token stream
3. **Semantic Analyzer**: Verifies program correctness and enriches the AST
4. **Code Generator**: Transforms the AST into target languages

This modular design allows components to be developed, tested, and modified independently, enabling extensibility and maintainability. The pipeline preserves semantic meaning at each transformation stage while preparing for the next step in the process.

More information about architecture design (graphs, diagrams, drafts) can be found in our documentation folder: [Documentation](https://github.com/kiyo9w/Hanami-CS370/tree/main/documentation)

---

## Interactive Playground

The Hanami playground provides a web-based interface to experiment with the language without installing anything locally:

- **Live Demo**: [https://hanami-fe.netlify.app/](https://hanami-fe.netlify.app/)
- **Features**:
  - Monaco code editor with Hanami syntax highlighting
  - Real-time compilation feedback
  - Visualization of compiler pipeline stages
  - Outputs in multiple target languages

The playground consists of a Next.js frontend application and an Express.js backend API connecting to the C++ compiler modules. This approach makes Hanami immediately accessible while providing educational insights into the compilation process.

---
## Examples

### A Simple Hello World Program

```
style <iostream>

grow main() -> int {
    bloom << "Hello, World!";
    blossom 0;
}
```

### Simple Math Program
```
garden MathTest

grow calculate() -> int {
    int a = 10;
    int b = 5 * (a - 3); // b should be 35
    int c = (a + b) % 8; // c should be 5

    bloom << "Result: " << c << "\n"; 

    blossom c;
} 
```

### Example Program With Floating Point, Hexa, Octal and Binary Numbers

```
style <iostream>

grow main() -> int {
    // Standard decimal integer
    int decimal_int = 123;
    bloom << "Decimal Int: " << decimal_int;

    // Floating-point number
    double floating_point = 4.56;
    bloom << "Floating Point: " << floating_point;

    // Floating-point with exponent
    double scientific = 1.23e+5; // 123000.0
    bloom << "Scientific Notation: " << scientific;

    // Hexadecimal integer (equivalent to 255)
    int hex_int = 0xFF;
    bloom << "Hex Int (0xFF): " << hex_int;

    // Binary integer (equivalent to 10)
    int binary_int = 0b1010;
    bloom << "Binary Int (0b1010): " << binary_int;

    // Octal integer (equivalent to 83)
    int octal_int = 0o123;
    bloom << "Octal Int (0o123): " << octal_int;

    blossom 0;
}
```

### Example program with access modifiers
```
style <iostream>
style <string>
style <vector>

garden TestGarden

species Pet {
hidden:
    string name = "Unknown";
    int age = 0;

open:
    grow setDetails(string newName, int newAge) -> void {
        name = newName;
        age = newAge;
        bloom << "Pet details updated: Name=" << name << ", Age=" << age << "\n";
        blossom;
    }
    grow introduce() -> void {
        bloom << "Woof! My name is " << name << " and I am " << age << " year(s) old.\n";
        blossom;
    }

guarded:
    grow getAgeInDogYears() -> int {
        blossom age * 7;
    }

};

grow mainGarden() -> int {
    bloom << "--- Hanami Pet Program Starting ---\n";

    string ownerName;
    bool nameIsHanami = false;
    int calculatedValue = 0;
    Pet myDog;

    myDog.setDetails("Buddy", 3);

    myDog.introduce();

    bloom << "What is your name? ";
    water >> ownerName;

    branch (ownerName == "Hanami") {
        bloom << "Welcome, Creator Hanami!\n";
        nameIsHanami = true;
        calculatedValue = 100;
    } else branch (ownerName == "Buddy") {
         bloom << "Hey, that's my name!\n";
         calculatedValue = 50;
    }
    else {
        bloom << "Nice to meet you, " << ownerName << "!\n";
        nameIsHanami = false;
        calculatedValue = myDog.getAgeInDogYears(); // Call guarded method
    }

    calculatedValue = calculatedValue + 5;
    bloom << "Calculated value: " << calculatedValue << "\n";
    bloom << "Is owner Hanami? " << nameIsHanami << "\n";

    bloom << "--- Hanami Pet Program Ending ---\n";

    blossom 0;
}

```
---

## Limitations & Future Work

### Current Limitations

- No array or collection data structures.
- Limited string manipulation (no interpolation/regex).
- No `switch`/`case` or advanced flow control.
- Incomplete OOP (inheritance, polymorphism, access control).
- No functional programming features (lambdas, higher-order functions).

### Planned Enhancements

- Arrays, lists, sets, and maps with intuitive syntax.
- String interpolation and regex support.
- Pattern matching and robust exception handling.
- Bidirectional type inference and flow-sensitive typing.
- Context-aware compilation and effect handlers.

---

## Team

- **Ngo Thanh Trung** - Team Lead, Code Generation, Playground Integration
- **Pham Tien Dat** - Documentation, Lexer Implementation
- **Pham Thai Duong** - Semantic Analyzer, Architecture
- **Bui Dang Quoc An** - Parser Implementation, Testing

---

## License

Hanami is licensed under the MIT License. 

See the [LICENSE](https://github.com/kiyo9w/Hanami-CS370/blob/main/LICENSE) file for details.

---

## Acknowledgments

We would like to express our deepest gratitude to Dr. Nguyen Dinh Han for his invaluable lectures that guided us in the development of this project. His lessons in programming language theory and compiler design provided the foundation for our work on Hanami.
