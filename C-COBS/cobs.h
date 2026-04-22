#ifndef COBS_STUFFING_H
#define COBS_STUFFING_H
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COBS_MIN_BUF_SIZE	3

enum {COBS_DECODED, COBS_ENCODED};  //encoding state

//Return values
enum 
{
	COBS_SUCCESS = 0, 	//success message, zero always

	//error fields
	COBS_ERROR_NULL_POINTER = -1,
	COBS_ERROR_SIZE = -2,
	COBS_ERROR_POINTER_OVERFLOW = -4,
	COBS_ERROR_SERIAL_OVERRUN = -5,
	COBS_ERROR_STREAMING_FRAME_DROPPED = -6,

	//state fields (not errors, not 'success')
	COBS_STREAMING_IN_PROGRESS = -7
};


typedef struct cobs_buf_t
{
    unsigned char * buf;        ///< Pointer to the backing byte array.
    size_t size;                ///< Capacity of buf in bytes.
    size_t length;              ///< Number of valid bytes currently in buf.
    uint8_t encoded_state;      ///< Encoding state of buf: COBS_ENCODED or COBS_DECODED. Must match actual content or decode/encode calls will fail.
} cobs_buf_t;

/**
 * @brief COBS-decode an encoded buffer into a separate output buffer.
 * Saves a copy operation versus in-place decode and relaxes the timing window for consuming
 * the decoded message, which is best practice for efficient serial streaming.
 * @param encoded_msg Input: COBS-encoded buffer. encoded_state must be COBS_ENCODED.
 * @param decoded_msg Output: decoded payload written here. buf and size must be set.
 * @return COBS_SUCCESS on success, negative error code otherwise.
 */
int cobs_decode_double_buffer(cobs_buf_t * encoded_msg, cobs_buf_t * decoded_msg);

/**
 * @brief COBS-encode a payload in-place.
 * Encodes msg in-place. msg->encoded_state must be COBS_DECODED on entry and is updated to
 * COBS_ENCODED on success.
 * @param msg Buffer containing the payload to encode. buf, size, and length must be set.
 * @return COBS_SUCCESS on success, negative error code otherwise.
 */
int cobs_encode_single_buffer(cobs_buf_t * msg);

/**
 * @brief Feed one byte of an incoming serial stream and decode a COBS frame when complete.
 * Accumulates bytes into encoded_msg using zero as the frame delimiter. On delimiter receipt,
 * decodes into decoded_msg via cobs_decode_double_buffer.
 * @param new_byte Next byte from the incoming stream.
 * @param encoded_msg Accumulation buffer for the raw encoded stream. Must persist across calls.
 * @param decoded_msg Output payload buffer. Valid only when COBS_SUCCESS is returned.
 * @return COBS_SUCCESS when a complete frame is decoded, COBS_STREAMING_IN_PROGRESS while accumulating,
 *         negative error code on failure.
 */
int cobs_stream(unsigned char new_byte, cobs_buf_t * encoded_msg, cobs_buf_t * decoded_msg);

/**
 * @brief Prepend a zero delimiter byte to a buffer in-place.
 * Shifts existing content forward by one byte and writes 0x00 at position 0. Useful for ensuring
 * a message is properly framed before handing it to a zero-delimited stream decoder.
 * @param msg Buffer to modify. Must have at least one byte of free headroom (size > length).
 * @return COBS_SUCCESS on success, negative error code if the buffer is full or input is invalid.
 */
int cobs_prepend_zero_single_buffer(cobs_buf_t * msg);

#ifdef __cplusplus
}
#endif

#endif