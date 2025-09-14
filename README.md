# Embedded Byte Stuffing Library

## Introduction

This library contains C .h and .c files for byte stuffing. The library implements the following protocols:

- High-Level Data Link Control (HDLC) byte stuffing (part of PPP or Point to Point Protocol)
- Consistent Overhead Bytes Stuffing (COBS) 

## Usage

Either pull the headers into your project directly and add to your build environment (recommended), or add this library as a submodule (also recommended).

CMake support with FetchContent pending.

## Unit Tests

To run the unit tests, install ceedling and simply run: `ceedling test:all`
