#ifndef PPP_H
#define PPP_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FRAME_CHAR 0x7E
#define ESC_CHAR 0x7D
#define ESC_MASK 0x20

typedef struct ppp_buffer_t
{
    unsigned char * buf;    ///< Pointer to the backing byte array.
    size_t size;            ///< Capacity of buf in bytes.
    size_t length;          ///< Number of valid bytes currently in buf.
} ppp_buffer_t;


/**
 * @brief HDLC-encode a payload into a separate output buffer.
 * Escapes any FRAME_CHAR or ESC_CHAR bytes in the payload and appends a trailing FRAME_CHAR delimiter.
 * @param unstuffed_buffer Input payload. buf and length must be set.
 * @param stuffed_buffer Output buffer for encoded result. buf and size must be set; worst-case size is length*2+1.
 * @return Length of encoded output on success, 0 if stuffed_buffer is too small or either input is invalid.
 */
size_t PPP_stuff(ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer);

/**
 * @brief HDLC-encode a payload in-place within a single buffer.
 * Expands escaped bytes in-place using a forward-shift per insertion. O(n^2) worst case;
 * use PPP_stuff for performance-sensitive paths.
 * @param msg Buffer containing the payload to encode. Must have enough headroom for expansion
 *            (worst-case size: length*2+1).
 * @return Length of encoded output on success, 0 if the buffer has insufficient headroom or is invalid.
 */
size_t PPP_stuff_single_buffer(ppp_buffer_t * msg);

/**
 * @brief HDLC-decode a stuffed frame into a separate output buffer.
 * Unescapes ESC_CHAR sequences and returns the payload up to the first FRAME_CHAR delimiter.
 * No leading FRAME_CHAR is expected; a leading FRAME_CHAR is treated as a zero-length frame.
 * @param unstuffed_buffer Output buffer for decoded payload. buf and size must be set.
 * @param stuffed_buffer Input frame to decode. buf and length must be set; must terminate with FRAME_CHAR.
 * @return Length of decoded payload on success, 0 if no valid frame is found or either input is invalid.
 */
size_t PPP_unstuff(ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer);

/**
 * @brief Feed one byte of an incoming serial stream and decode a frame when complete.
 * Accumulates bytes into input_buffer until FRAME_CHAR is received, then calls PPP_unstuff.
 * Zero-length results (stray FRAME_CHAR or legacy dual-delimiter traffic) should be discarded by the caller.
 * @param new_byte Next byte from the incoming stream.
 * @param unstuffed_buffer Output payload buffer. Valid only when return value > 0.
 * @param input_buffer Accumulation buffer. Must persist across calls with length initialised to 0.
 * @return Decoded payload length when a complete frame is received, 0 otherwise.
 */
size_t parse_PPP_stream(uint8_t new_byte, ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * input_buffer);

#ifdef __cplusplus
}
#endif

#endif
