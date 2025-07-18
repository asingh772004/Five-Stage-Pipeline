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


- On successful build, your binary will be in `build/Debug/` (Debug mode, Windows/MSVC) or `build/` (Linux/g++), named `pipeline.exe` or `pipeline`.

---

## Usage

### 1. Prepare Your Instruction File

Instructions go in `test_instructions.txt` (or any file of your choice). Example:

```
addi x1, x0, 5 # x1 = 5
addi x2, x0, 10 # x2 = 10
add x3, x1, x2 # x3 = x1 + x2 = 15
sw x3, 0(x0) # mem = x3 = 15
lw x5, 0(x0) # x5 = mem = 15
beq x1, x2, 2 # if x1==x2 jump 2 ahead (not taken)
addi x6, x0, 100 # x6 = 100
```

### 2. Run the Simulator

Run from your `build/Debug` (Windows) or `build` (Linux) folder. Give the correct **relative path** to your instruction file.

**Windows Example:**

```
cd build/Debug
pipeline.exe
```

**Linux/macOS Example:**

```
cd build
./pipeline
```


If you have modified your code to accept a specific file path or to always read from the project root, adjust accordingly.

---

## Output

- For each input instruction, the program prints its binary encoding.
- For each pipeline cycle, you see only the registers and memory that changed.
- Output format:  

```
Cycle: 3
Register/Memory Contents after Cycle 3:
Reg1 = 5; Reg2 = 10; Reg3 = 15;
Mem0 = 15;
```


- The pipeline continues until all activity has stopped.

---

## Extending the Project

- **To add new instructions:** Update opcode and encoding tables in `src/assembly_to_binary.cpp`.
- **To handle more hazards:** Extend the pipeline logic in `src/pipeline.cpp`.
- **To support new registers/more memory:** Change vector sizes in `pipeline.cpp`.

---

## Troubleshooting

- **No output or blank cycles?**  
Likely your instructions file path is wrong relative to where you run. Double-check the path and file contents.
- **Linker or include errors?**  
Make sure to use `cmake` and include all `src/` files in your build.
- **Build/Debug folders:**  
Your executable is inside `build/Debug/` by default (Visual Studio/MSVC), or in `build/` for Linux/g++.

---

## Contributing

1. Fork, create a branch, and open PRs for improvements.
2. Follow the coding/naming conventions as in the current repository.
3. Please add tests and documentation for new features.

---

## Authors & Contact

Developed by Aditya Singh  
For queries and contributions, open a GitHub issue or pull request.
