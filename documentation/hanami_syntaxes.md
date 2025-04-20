**Hanami Syntaxes:**

Use the following **Keyword Conversion** table:

| **Hanami** | **C++ Equivalent** | **Meaning** |
| --- | --- | --- |
| garden &lt;Name&gt; | namespace &lt;Name&gt; { ... } | Declares a namespace (“garden”). |
| species &lt;Name&gt; | class &lt;Name&gt; | Declares a class (“species,” i.e., a “blossom”). |
| open: | public: | Public section. |
| hidden: | private: | Private section. |
| guarded: | protected: | Protected section. |
| grow &lt;functionName&gt;() -> &lt;type&gt; | &lt;type&gt; &lt;functionName&gt;() | Declares a function (“grow”). |
| blossom &lt;value&gt;; | return &lt;value&gt;; | Returns a value. |
| style &lt;library&gt; | #include &lt;library&gt; | Includes a library. |
| bloom << x; | std::cout << x; | Prints to the console. (“bloom” = console output) |
| water >> x; | std::cin >> x; | Reads from input. (“water” = console input) |
| branch (&lt;cond&gt;) { ... } | if (&lt;cond&gt;) { ... } | If‐statement. |
| else branch (&lt;cond&gt;) { ... } | else if (&lt;cond&gt;) { ... } | Else‐if. |
| else { ... } | else { ... } | Else. |

**Note**: We keep standard C++ types (int, string, bool, etc.) exactly as in C++.

**1.1 Example Code**

style &lt;iostream&gt;

style &lt;string&gt;

garden SimpleGarden

species Rose {

open:

grow sayHello() -> void {

bloom << "Hello from Hanami Rose!\\n";

blossom;

}

hidden:

int secretNumber = 42;

guarded:

bool isFriendly = true;

};

grow mainGarden() -> int {

std::string userName;

bloom << "What's your name? ";

water >> userName;

Rose g;

g.sayHello();

branch (userName == "Rose") {

bloom << "You have a lovely name!\\n";

}

else branch (userName == "Lily") {

bloom << "Another beautiful flower name!\\n";

}

else {

bloom << "Nice to meet you, " << userName << "!\\n";

}

blossom 0;

}

A hypothetical Hanami compiler would transpile this into regular C++ (replacing the custom keywords with standard namespace, class, #include &lt;iostream&gt;, etc.) and then compile with a normal C++ toolchain.