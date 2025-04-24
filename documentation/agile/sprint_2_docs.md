# Sprint 2: Compiler Modules Development

**Deadline Formats:**  
- **End of Week 1:** Saturday, Mar 22  
- **End of Week 2:** Saturday, Mar 29  

---

## Team Tasks & Deliverables

### Dat (Lexer Enhancements)
- **Extended Lexer Implementation**  
  - **Task:** Develop a c++ based lexer to support additional tokens, multi-line comments, string literals, and improved error handling.  
  - **Deadline:** Initial implementation by End of Week 1; final adjustments by End of Week 2.  
  - **End Goal:** A robust lexer module that reliably tokenizes a wider range of language constructs.

---

### An (Parser Module)
- **Parser Implementation**  
  - **Task:** Develop a recursive descent c++ based parser that converts tokens from the lexer into an Abstract Syntax Tree (AST).  
  - **Deadline:** Basic parser framework by End of Week 1; refinements and bug fixes by End of Week 2.  
  - **End Goal:** A functioning parser that accurately generates ASTs from the token stream.

---

### Duong (Semantic Analysis & Intermediate Representation)
- **Semantic Analyzer Module**  
  - **Task:** Implement a c++ based semantic analysis module that processes the AST to perform type checking, build symbol tables, and generate an intermediate representation (IR).  
  - **Deadline:** Core functionality by End of Week 1; complete integration by End of Week 2.  
  - **End Goal:** A semantic analyzer that enriches the AST with context and produces a viable IR.

---

### Trung (Code Generation & Transpiler)
- **Code Generation Module**  
  - **Task:** Create a code generation module that converts the IR into C++ code, enabling the transpiling process.  
  - **Deadline:** Prototype by End of Week 1; complete implementation by End of Week 2.  
  - **End Goal:** A working transpiler that produces valid C++ code from the IR.
- **Integration & Error Reporting**  
  - **Task:** Integrate the code generation module with the other compiler components and implement comprehensive error reporting for code generation failures.  
  - **Deadline:** End of Week 2.  
  - **End Goal:** A seamless module that effectively handles integration issues and reports errors clearly.