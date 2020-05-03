# Introduction
This is a example TCP sockets ChatServer build on top of boost::asio library with PostgreSQL connection through yandex::ozo library. Source code was extracted from much greater project and not everything may work, as tons of other code was removed - it's only meant as an example on how to use certain technologies named in **Technology Stack** section of this README.  
Server implements simple chat: clients can set their usernames, query for other users and send messages.

# Getting Started
1. To build ChatServer, run the following commands:  
  `mkdir -p build/`  
  `cd build`  
  `cmake ..`  
  `make -j4`  
  ChatServer binary will is located in `build/src/ChatServer`. Just please remember it also requires having `logger.config` to run.    

2. How to run unit tests:  
  `./ChatServer/build/tests/ChatServer_tests`  

# Development
This section outlines how to develop ChatServer.
## Coding hints

### Language references
There are tons of good materials on C++ language, the ones I advise to get familiar with are:  
- Official language reference: https://en.cppreference.com/  
- Official C++ guidelines: https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md  
- Official C++ FAQ: https://isocpp.org/faq  
- _The C++ Programming Language_ by Bjarne Stroustrup  
- _Effective Modern C++_ by Scott Meyers  
### Coding style
I don't have any Coding Standard document, because coding style should be obvious from our source code. When developing, just please follow convention from other files, e.g. includes order, camelCase naming style, etc, because _"The need for consistency beats personal taste"_.  
Please also remember to format your code using `.clang-format`. (To use ClangFormat in CLion: https://www.jetbrains.com/help/clion/clangformat-as-alternative-formatter.html)  
Below you can read why I decided to choose to write code this way:
- Why I don't (and you generally shouldn't) use abbreviations (with exceptions to _i_ and _e_ variables): https://softwareengineering.stackexchange.com/questions/72593/how-to-abbreviate-variable-names
- In order to make code shorter horizontally, I follow _camelCase_ convention: https://en.wikipedia.org/wiki/Camel_case
- In order to make code shorter vertically, I follow _java indenting style_: https://en.wikipedia.org/wiki/Indentation_style#Variant:_Java
- I don't mark fields in classes with `_` because it's forbidden: https://www.gnu.org/software/libc/manual/html_node/Reserved-Names.html and in general I avoid every symptom of _Hungarian Notation_: https://www.gnu.org/software/libc/manual/html_node/Reserved-Names.html (and your IDE does it for you already)
## Technology stack
This section mentions all the technologies I use to develop ChatServer application  
- GCC compiler
  - versions 7 & 8 (https://gcc.gnu.org/gcc-8/)
  - Both implement full _C++17_
- CMake build system
  - version 3.x (https://cmake.org)
- boost::asio for networking
  - https://www.boost.org/doc/libs/1_72_0/doc/html/boost_asio.html
  - I'm using `spawn::spawn` (a `boost::asio::spawn` wrapper) to give users impression of sequential code/data while performing async operations, by spawning stackful coroutines (https://www.boost.org/doc/libs/1_66_0/libs/coroutine/doc/html/coroutine/motivation.html#coroutine.motivation.event_based_asynchronous_paradigm)
  - asio also provides us networking layer: sockets, tcp, signals, timers
- boost::ptree for XML
  - https://www.boost.org/doc/libs/1_66_0/doc/html/property_tree/tutorial.html
- yandex::ozo PostgreSQL client library
  - https://github.com/yandex/ozo
- log4cpp for logging
  - version 1.1.2 (http://log4cpp.sourceforge.net)
  - There are many ports of log4j to C++, this is one of many. Unfortunately, no tutorials are available for this particular library, but you can check other docs on log4j, they are very similar, e.g.: https://logging.apache.org/log4cxx/latest_stable/usage.html
- gTest + gMock for unit testing
  - https://github.com/google/googletest
- python3 for integration tests
- git version control system
- clang tools
  - clang-format
  - clang-tidy
- CLion/VIM IDEs
- Smaller tools:
  - gdb (debugging)
  - valgrind (memory leaks)
  - perf (performance profiler)
  - gcov (code coverage)