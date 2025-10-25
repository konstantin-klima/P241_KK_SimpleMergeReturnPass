# Konstrukcija Kompilatora (P241) @ MatF - Simple Merge Return Pass

A Simple Merge Return Pass implementation done in LLVM as part of the Compiler Construction course at the Faculty of Mathematics, University of Belgrade.

## Building and Using the Pass
### Building the Pass
To build the Simple Merge Return Pass:
1. **Clone and build LLVM**: Ensure you have LLVM built from source. You can follow the official LLVM documentation for building LLVM.
2. **Add the Pass**: Place the `src` folder from this repository in the appropriate directory within the LLVM source tree and include it in `CMakeLists.txt` via `add_subdirectory`.
3. **Build the Pass**: Rebuild LLVM to include the new pass.

### Using the Pass
To use the Simple Merge Return Pass:
1. **Position yourself in the LLVM build directory**:
   ```bash
   cd <path-to-llvm-build-directory>
   ```
2. **Prepare an LLVM IR file**: you can generate it using clang:
   ```bash
    ./bin/clang -S -emit-llvm <file.c> -o <file.ll>
    ```
3. **Run the Pass**: Use the `opt` tool to run the pass on an LLVM IR file:
   ```bash
   ./bin/opt -load-pass-plugin=./lib/SimpleMergeReturn.so -passes="simple-mergereturn" <file.ll> -S -o <optimized.ll>
   ```
    In case of using the legacy pass manager, use:
   ```bash
   ./bin/opt -load ./lib/SimpleMergeReturn.so -enable-new-pm=0 -simple-mergereturn <file.ll> -S -o <optimized.ll>
   ```