# Campus Library Management System

A C++17 OOP library application with both a terminal interface and a browser dashboard. The dashboard stores data in the same library_data.txt file used by the C++ program.

## Features

- Add books and magazines with unique positive IDs
- List all items and show availability
- Issue and return items with validation
- Automatically save data in `library_data.txt`
- Recover gracefully from invalid menu input and malformed stored records

## OOP concepts demonstrated

- **Encapsulation:** `LibraryItem` keeps its ID, title, and availability private and exposes controlled methods.
- **Abstraction:** `LibraryItem` is an abstract base class with pure virtual functions.
- **Inheritance:** `Book` and `Magazine` derive from `LibraryItem`.
- **Runtime polymorphism:** the library stores base-class pointers and calls overridden `display`, `type`, and `serialize` methods.
- **Composition:** `Library` owns a collection of library items.
- **Exception handling:** invalid operations are reported with exceptions caught by the menu loop.
- **File handling:** data is loaded at startup and saved after each change and on exit.

## Requirements

A C++17 compiler such as `g++` or `clang++`.

## Browser dashboard

The dashboard runs locally and shares the same library_data.txt data file as the C++ program. Python 3 is used only as a small local web server.

```bash
python3 LibraryDashboardServer.py
```

Then open http://127.0.0.1:8001 in a browser. Stop the server with Ctrl+C. Avoid editing the same catalogue simultaneously in the terminal app and browser dashboard. The C++ program and dashboard use the same `library_data.txt` file.

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic LibraryManagementSystem.cpp -o library
./library
```

On Windows with MinGW:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic LibraryManagementSystem.cpp -o library.exe
.\library.exe
```

The program creates `library_data.txt` in its current working directory. Keep that file beside the program's working directory to retain data. To reset the demo catalogue, exit the program and delete the data file.

## Data format

Each line is a pipe-separated record. `B` means book and `M` means magazine. The final field is `1` for available and `0` for issued. Example:

```text
B|101|The C++ Programming Language|Bjarne Stroustrup|1
M|201|Science Monthly|12|1
```

## Project files

- `LibraryManagementSystem.cpp` — commented C++ source code
- `LibraryDashboard.html` — responsive library dashboard frontend
- `LibraryDashboardServer.py` — local HTTP API sharing the C++ text data file
- `LibraryManagementSystem_Report.docx` — printable project report
- `Output/sample_run.txt` — sample console session

## Limitations

This learning project intentionally uses a single local data file and does not track borrower identity, due dates, or fines. Titles and author names should not contain the `|` character because it separates fields in the simple text format.

## References

- Bjarne Stroustrup, *Programming: Principles and Practice Using C++*, Addison-Wesley.
- cppreference.com, C++ language and standard library reference: <https://en.cppreference.com/>
- ISO C++ Core Guidelines: <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>
