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

