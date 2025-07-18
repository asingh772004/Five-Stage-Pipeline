# Five-Stage RISC-V Pipeline Simulator

## Overview

This project is a **C++ simulator of a five-stage pipelined processor** modeled after the RISC-V architecture. It reads a user-supplied text file of RISC-V assembly instructions, converts them to binary, and simulates their passage through the classic five-stage pipeline (Instruction Fetch, Decode, Execute, Memory, Writeback). The simulator provides detailed per-cycle views of register and memory state, making it ideal for computer architecture education and experimentation.

---

## Features

- Modular, readable C++ code structure
- Converts human-readable RISC-V assembly to binary
- Simulates all five architectural pipeline stages
- Detects data hazards and stalls when needed
- Per-cycle debug printing of changed registers/memory
- Easy to extend (add instructions, hazards handling, etc.)

---

## Directory Structure
```
Five-Stage-Pipeline/
├── include/
│   └── assembly_to_binary.h
├── src/
│   ├── assembly_to_binary.cpp
│   └── pipeline.cpp
├── test_instructions.txt
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## Setup & Build

### Prerequisites

- **C++ compiler**:
  - Windows: Visual Studio/MSVC recommended
  - Linux/macOS: g++ (7.0+)
- **CMake** (version 3.10 or newer)
- (Optional) Visual Studio Code with [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

### Build Instructions (CMake-based, cross-platform)

1. Open a terminal (cmd/PowerShell for Windows; terminal for Linux/Mac).
2. Clone or copy this repository and change to your project directory.
3. Run the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .
```

