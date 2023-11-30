#ifndef COBS_STUFFING_H
#define COBS_STUFFING_H
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

size_t cobsDecode(const uint8_t *buffer, size_t length, void *data);
size_t cobsEncode(const void *data, size_t length, uint8_t *buffer);

#endif