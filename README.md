# Embedded Byte Stuffing Library

## Introduction

This library contains C .h and .c files for byte stuffing. The library implements the following protocols:

- [High-Level Data Link Control](https://en.wikipedia.org/wiki/High-Level_Data_Link_Control#Asynchronous_framing) Asynchronous Framing protocol (part of PPP or Point to Point Protocol) - simply referred to as 'PPP' in this library.
- [Consistent Overhead Bytes Stuffing](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) (COBS) 

## Usage

Either copy the headers into your project directly and add to your build environment, or add this library as a submodule and link to the headers (recommended).

CMake support with FetchContent pending.

## Unit Tests

To run the unit tests, install ceedling and simply run: `ceedling test:all`
