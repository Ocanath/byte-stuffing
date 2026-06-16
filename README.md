# Embedded Byte Stuffing Library

A minimal C library for byte stuffing, intended for use in embedded systems. Implements two framing protocols:

- **COBS** (Consistent Overhead Byte Stuffing) — deterministic overhead, no sentinel byte collisions, well-suited for binary data.
- **PPP/HDLC** (High-Level Data Link Control asynchronous framing) — flag-byte delimited framing as used in Point-to-Point Protocol.

## Integration

### Git submodule (recommended)

Add as a git submodule, then in your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/byte-stuffing)
target_link_libraries(your_target PRIVATE cobs PPP)
```

### CMake FetchContent (also recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    byte_stuffing
    GIT_REPOSITORY https://github.com/Ocanath/byte-stuffing.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(byte_stuffing)
target_link_libraries(your_target PRIVATE cobs PPP)
```

### Direct copy

Copy the source files directly into your project and add them to your build environment.

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Testing

Unit tests run via ceedling (`ceedling test:all`). A Python COBS implementation with pytest-based tests is also included.
