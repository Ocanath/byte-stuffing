# PPP Byte Stuffing Unit Tests

This project contains unit tests for the PPP (Point-to-Point Protocol) byte stuffing implementation, converted from the original test.c to use the Ceedling testing framework.

## Prerequisites

To run these tests, you need:

1. **Ceedling** - A unit testing framework for C
   ```bash
   gem install ceedling
   ```

2. **GCC** - GNU Compiler Collection
   ```bash
   # On Ubuntu/Debian
   sudo apt-get install gcc
   
   # On macOS with Homebrew
   brew install gcc
   
   # On Windows with MinGW
   # Download and install MinGW
   ```

3. **gcov** - For code coverage (usually comes with GCC)

## Project Structure

```
.
├── PPP.c          # Implementation file
├── PPP.h          # Header file
├── test_PPP.c     # Unit tests (converted from test.c)
├── project.yml    # Ceedling configuration
├── Makefile       # Build automation
└── README.md      # This file
```

## Running Tests

### Using Makefile (Recommended)
```bash
# Run all tests
make test

# Run only PPP tests
make test-ppp

# Clean build artifacts
make clean

# Generate coverage report
make coverage

# Show help
make help
```

### Using Ceedling directly
```bash
# Run all tests
ceedling test:all

# Run specific test file
ceedling test:test_PPP

# Generate coverage
ceedling gcov:all

# Clean
ceedling clean
```

## Test Coverage

The converted unit tests cover the following scenarios from the original test.c:

1. **Basic PPP stuffing** - Tests stuffing with random data
2. **Frame character handling** - Tests stuffing when data contains frame characters (0x7E)
3. **PPP unstuffing** - Tests the reverse operation
4. **Stream parsing** - Tests byte-by-byte stream parsing (simulating serial communication)
5. **Buffer overflow protection** - Tests error handling for small buffers
6. **Invalid frame detection** - Tests handling of malformed data
7. **ESC character handling** - Tests stuffing when data contains ESC characters (0x7D)

## Key Improvements Over Original Test

1. **Proper test isolation** - Each test runs independently with fresh fixtures
2. **Assertions** - Uses Unity framework assertions for proper test validation
3. **Error case testing** - Includes tests for edge cases and error conditions
4. **Code coverage** - Can generate coverage reports to ensure comprehensive testing
5. **Modular structure** - Tests are organized into logical, focused test functions
6. **Build automation** - Easy to run tests with simple commands

## Original vs Converted Test

The original `test.c` was a single monolithic test that:
- Generated random data
- Performed stuffing operations
- Printed results
- Simulated stream parsing

The converted `test_PPP.c` breaks this into:
- 7 focused unit tests
- Proper setup/teardown with `setUp()` and `tearDown()`
- Assertions to verify expected behavior
- Error case testing
- Maintains the same core functionality but with better structure

## Output Example

When you run the tests, you'll see output like:
```
Test 'test_PPP.c'
--------------------
Running PPP unit tests...

test_PPP.c:test_PPP_stuff_basic_random_data:PASS
Original Data: 0x7A 3B 8F 2D 1E 9C 4A 5F
Stuffed data: 0x7E 7A 3B 8F 2D 1E 9C 4A 5F 7E
Size: 10

test_PPP.c:test_PPP_stuff_with_frame_character:PASS
Data with frame char: 0x7A 3B 8F 7E
Stuffed data: 0x7E 7A 3B 8F 7D 5E 7E
Size: 7

...

OVERALL TEST SUMMARY
===================
TESTED:  7
PASSED:  7
FAILED:  0
IGNORED: 0 