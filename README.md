# PathFindingCpp ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![MSVC](https://img.shields.io/badge/MSVC-%23007ACC.svg?style=for-the-badge&logo=visual-studio&logoColor=white)

**PathFindingCpp** is a small project designed to explore and learn the fundamentals of pathfinding algorithms in C++ 23. This project demonstrates the basics of pathfinding through simple implementations of well-known algorithms.

## Features

- **Algorithms**: Gain insights into the inner workings of pathfinding with these algorithms:
  - [A* (A-Star)](https://en.wikipedia.org/wiki/A*_search_algorithm): A heuristic-based algorithm for optimal pathfinding.
  - [Greedy Search](https://en.wikipedia.org/wiki/Greedy_algorithm): A faster, less memory-intensive algorithm with locally optimal choices.

## Getting Started

Follow the steps below to build and run **PathFindingCpp**.

### Prerequisites

- **CMake** (version 3.16 or later recommended)
- **C++ 23-compatible compiler**
- **MSVC (Microsoft Visual C++)** or **Mingw**: Required for building this project. Ensure that your MSVC installation is up to date.

### Building the Project

To build **PathFindingCpp** in Debug mode, follow these steps

1. **Configure the build**:
   ```bash
   cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug -H.
   ```
2. **Compile the project** (replace 1 with the number of threads you want to use for a faster build)::
   ```bash
    cmake --build build -j1
   ```

### Prebuilt Binary

If you prefer, a prebuild binary is available:

- Location: `bin/PathFinding.exe`
- MD5 Checksum: The folder also contains an md5sum file to verify the integrity of the binary.

### Running the Program

Once built (or using the prebuilt binary), you can run PathFindingCpp as follows:

```bash
.\bin\PathFinding.exe --help
```

This command provides usage instructions and available command-line options.