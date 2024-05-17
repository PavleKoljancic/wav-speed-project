# Parallelizable Data Processing Algorithm for WAV Files

## Overview

This project implements a parallelizable algorithm for slowing down or speeding up WAV files with 8 or 16-bit samples using polynomial interpolation in C. The program accepts command-line arguments for the input file path, output file path, and algorithm parameters. It also includes default values for these arguments.

The project explores various optimization techniques and their impact on performance, including SIMD programming, cache optimization, multi-core parallelization, distributed processing, and GPGPU programming. The results are documented and graphically presented to show the speedup achieved.

## Key Features

### Command-line Arguments
- **Input File Path:** Path to the input WAV file.
- **Output File Path:** Path to the output WAV file.
- **Algorithm Parameters:** Values for algorithm-specific parameters.

### Default Values
- If not provided, the program uses sensible default values for all arguments.

### Data Processing Algorithm
- The algorithm processes WAV files with 8 or 16-bit samples.
- Implemented in C for efficiency and control over low-level operations.

### Optimization Techniques
**SIMD Programming:**
   - Utilizes SIMD (Single Instruction, Multiple Data) instructions to process multiple data points in parallel.
   - Implemented using compiler-specific intrinsics for vector operations.

**Multi-core Parallelization:**
   - Uses OpenMP or custom threading solutions to parallelize the algorithm across multiple CPU cores.
