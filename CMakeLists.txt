cmake_minimum_required(VERSION 3.12)
project(byte_stuffing C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# C-COBS library
add_library(cobs STATIC
    C-COBS/cobs.c
)

target_include_directories(cobs PUBLIC
    C-COBS
)

# HDLC/PPP library
add_library(PPP STATIC
    HDLC/PPP.c
)

target_include_directories(PPP PUBLIC
    HDLC
)

# Configure for C++ compatibility
set_target_properties(cobs PROPERTIES
    C_STANDARD 99
    POSITION_INDEPENDENT_CODE ON
)

set_target_properties(PPP PROPERTIES
    C_STANDARD 99
    POSITION_INDEPENDENT_CODE ON
)

# Optional: Create an umbrella target to build both libraries
add_custom_target(all_libs DEPENDS cobs PPP)