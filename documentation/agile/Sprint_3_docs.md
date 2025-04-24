# Sprint 3: Integration, Testing, Documentation & DX Foundation (Revised Timeline)

**Sprint Duration:** 3 Weeks (Friday, Apr 11th - Saturday, Apr 26th)

**Deadline Formats:**
- **End of Week 1:** ~Friday, Apr 18th
- **End of Week 2:** ~Friday, Apr 25th
- **End of Sprint (Week 3):** Saturday, Apr 26th

---

## Sprint Goals (for the Apr 11 - Apr 26 period)

1.  **Unit Testing Completion:** Establish and complete a baseline of unit tests for all core compiler modules.
2.  **Module Integration & Debugging:** Achieve a functional, integrated pipeline, resolving major bugs identified during testing and integration.
3.  **Core Documentation Drafting:** Draft key sections of the final project report (Introduction, Overview, System Architecture).
4.  **Visual Documentation Creation:** Create essential UML diagrams (Class, Sequence) for the report.
5.  **Developer Experience (DX) Exploration:** Begin foundational work on a VS Code extension.

---

## Team Tasks & Deliverables (Reflecting 3-Week Timeline)

### Trung (PM / Lead Integrator)
- **Module Integration & Pipeline Finalization**
  - **Task:** Oversee and implement the linking of all compiler modules. Define intermediate formats (.tokens, .ast, .ir) and manage `common/`. Focus on resolving integration issues found during debugging.
  - **Timeline:** Major integration efforts during Weeks 1 & 2, final wrap-up and checks in Week 3.
  - **End Goal:** A runnable compiler pipeline by End of Sprint.
- **Report Structure & Coordination**
  - **Task:** Finalize report structure early and coordinate documentation efforts.
  - **Timeline:** Finalized structure by End of Week 1. Coordination ongoing.
- **VS Code Extension - Phase 1 (Stretch Goal / Initial Setup)**
  - **Task:** Initiate VS Code extension project: basic syntax highlighting and simple snippets.
  - **Timeline:** Primarily Week 3 (Apr 26).
  - **End Goal:** Basic VS Code extension providing syntax highlighting by End of Sprint.

---

### Đạt Phạm (Core Logic & Research)
- **Module Debugging & Extension**
  - **Task:** Lead debugging efforts based on unit test results and integration issues. Extend module capabilities as needed.
  - **Timeline:** Heavily focused during Weeks 1 & 2; final fixes in Week 3.
  - **End Goal:** More robust and debugged compiler modules by End of Sprint.
- **AST -> IR Refinement**
  - **Task:** Analyze semantic analysis output, design/implement changes for a more valuable IR.
  - **Timeline:** Design by End of Week 2; Implementation completed in Week 3.
  - **End Goal:** A distinct IR representation by End of Sprint.
- **Documentation: Introduction & Related Work**
  - **Task:** Draft Introduction and Related Work sections, including Key Findings.
  - **Timeline:** Research during Weeks 1-2, writing focused in Week 3 (Apr 26).
  - **End Goal:** Comprehensive introduction draft by End of Sprint.

---

### An Bùi (Testing & Documentation/Visualization)
- **Unit Testing (Lexer & Parser)**
  - **Task:** Develop and execute unit tests for Lexer and Parser (min 5 each). Report bugs found.
  - **Timeline:** Completed primarily within Weeks 1 & 2 (by Apr 25th).
  - **End Goal:** Reliable Lexer and Parser modules, verified by tests.
- **Documentation: Overview**
  - **Task:** Draft the Overview section (language design, syntax) using `Hanami Syntaxes.docx`.
  - **Timeline:** Focused effort in Week 3 (Apr 26).
  - **End Goal:** Clear language overview draft by End of Sprint.
- **Visualization: Sequence Diagram**
  - **Task:** Create Sequence Diagram for a key scenario (e.g., `grow` function processing).
  - **Timeline:** Completed during Week 3 (by Apr 26th).
  - **End Goal:** Visual explanation of component interaction by End of Sprint.

---

### Thái Dương (Testing & Documentation/Visualization)
- **Unit Testing (Semantic Analyzer & Code Gen)**
  - **Task:** Develop and execute unit tests for Semantic Analyzer and Code Generator (min 5 each). Report bugs found.
  - **Timeline:** Completed primarily within Weeks 1 & 2 (by Apr 25th).
  - **End Goal:** Reliable Semantic Analyzer and Code Generator modules, verified by tests.
- **Documentation: System Architecture**
  - **Task:** Draft System Architecture section referencing `System_Architecture.pdf`.
  - **Timeline:** Focused effort in Week 3 (Apr 26).
  - **End Goal:** Detailed internal design draft by End of Sprint.
- **Visualization: UML Class Diagram**
  - **Task:** Create UML Class Diagram for key data structures (AST nodes, Symbol Table).
  - **Timeline:** Completed during Week 3 (by Apr 26th).
  - **End Goal:** Visual representation of data structures by End of Sprint.

---

## Sprint Process & Meetings (Apr 11 - Apr 26)

- **Weeks 1 & 2 Focus:** Intensive debugging sessions based on unit testing feedback; steady progress on module integration.
- **Week 3 Focus (Apr 26):** Final integration checks, intensive documentation writing, diagram creation, VS Code extension work.
- **Meeting (Historical/Planned):** Sunday meeting likely happened around Apr 20th (Mid-sprint).
- **Sprint Review/Demo:** **Saturday, Apr 26th** - Demo the integrated pipeline, review all drafted documentation sections, diagrams, test results, and plan next steps (project finalization/report completion).

---