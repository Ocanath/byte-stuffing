"""
Consistent Overhead Byte Stuffing (COBS) implementation in Python.

This module provides encoding and decoding functionality for COBS,
a byte stuffing algorithm that eliminates zero bytes from arbitrary data.
"""

from typing import Optional


def encode(data: bytes) -> bytes:
    """
    Encode data using COBS algorithm.

    Args:
        data: Raw bytes to encode

    Returns:
        Encoded bytes with zero delimiter appended

    Raises:
        ValueError: if data is empty

    Example:
        >>> encode(bytes([0x00]))
        b'\\x01\\x01\\x00'
        >>> encode(bytes([0x11, 0x22, 0x00, 0x33]))
        b'\\x03\\x11\\x22\\x02\\x33\\x00'
    """
    if not data:
        raise ValueError("Cannot encode empty data")

    result = bytearray()
    result.append(0)  # Placeholder for first pointer

    pointer_idx = 0
    block_start = 1
    i = 0

    while i < len(data):
        # Copy the byte
        result.append(data[i])
        current_idx = len(result) - 1

        if data[i] == 0:
            # Found a zero - set pointer and update
            pointer_value = current_idx - pointer_idx
            if pointer_value > 0xFF:
                raise ValueError("Pointer overflow")
            result[pointer_idx] = pointer_value
            pointer_idx = current_idx
            block_start = len(result)
            i += 1

        elif current_idx - block_start >= 254:
            # Full block (254 bytes) - insert pointer
            pointer_value = 0xFF
            result[pointer_idx] = pointer_value
            result.append(0)  # New pointer placeholder
            pointer_idx = len(result) - 1
            block_start = len(result)
            i += 1
        else:
            i += 1

    # Set final pointer and append delimiter
    final_pointer = len(result) - pointer_idx
    if final_pointer > 0xFF:
        raise ValueError("Pointer overflow")
    result[pointer_idx] = final_pointer
    result.append(0)  # Delimiter

    return bytes(result)


def decode(data: bytes) -> bytes:
    """
    Decode COBS-encoded data.

    Args:
        data: COBS-encoded bytes (should end with zero delimiter)

    Returns:
        Decoded bytes

    Raises:
        ValueError: if data is empty or invalid

    Example:
        >>> decode(bytes([0x01, 0x01, 0x00]))
        b'\\x00'
        >>> decode(bytes([0x03, 0x11, 0x22, 0x02, 0x33, 0x00]))
        b'\\x11\\x22\\x00\\x33'
    """
    if not data:
        raise ValueError("Cannot decode empty data")

    if len(data) < 2:
        raise ValueError("Invalid COBS data: too short")

    result = bytearray()

    pointer_value = data[0]
    pointer_idx = pointer_value
    i = 1

    while i < len(data):
        # Stop at delimiter
        if data[i] == 0:
            break

        if i != pointer_idx:
            # Normal byte - copy to output
            result.append(data[i])
            i += 1
        elif pointer_value != 255:
            # At pointer location, not a full block - decode a zero
            result.append(0)
            pointer_value = data[i]
            pointer_idx = pointer_value + i
            i += 1
        else:
            # At pointer location, full block (255) - skip without adding zero
            pointer_value = data[i]
            pointer_idx = pointer_value + i
            i += 1

    return bytes(result)


class COBSStreamDecoder:
    """
    Stateful stream decoder for COBS-encoded data.

    Handles incoming byte streams, accumulating bytes until a complete
    frame (delimited by zero) is received.

    Example:
        >>> decoder = COBSStreamDecoder()
        >>> # Feed encoded bytes one at a time
        >>> decoder.feed_byte(0x03)  # returns None (incomplete)
        >>> decoder.feed_byte(0x11)  # returns None (incomplete)
        >>> decoder.feed_byte(0x22)  # returns None (incomplete)
        >>> decoder.feed_byte(0x02)  # returns None (incomplete)
        >>> decoder.feed_byte(0x33)  # returns None (incomplete)
        >>> decoder.feed_byte(0x00)  # returns b'\\x11\\x22\\x00\\x33'
    """

    def __init__(self, max_frame_size: int = 1024):
        """
        Initialize stream decoder.

        Args:
            max_frame_size: Maximum size of a single frame
        """
        self.max_frame_size = max_frame_size
        self._buffer = bytearray()

    def feed_byte(self, byte: int) -> Optional[bytes]:
        """
        Feed a single byte to the stream decoder.

        Args:
            byte: Single byte value (0-255)

        Returns:
            Decoded frame if complete, None otherwise

        Raises:
            ValueError: if buffer overrun occurs
        """
        # Check for buffer overrun
        if len(self._buffer) >= self.max_frame_size:
            self._buffer.clear()
            raise ValueError("Buffer overrun: frame too large")

        # Buffer the new byte
        self._buffer.append(byte)

        # Check for delimiter (frame complete)
        if byte == 0:
            try:
                frame = decode(bytes(self._buffer))
                self._buffer.clear()
                return frame
            except ValueError as e:
                self._buffer.clear()
                raise ValueError(f"Failed to decode frame: {e}")

        return None

    def feed_bytes(self, data: bytes) -> list[bytes]:
        """
        Feed multiple bytes and return any complete frames.

        Args:
            data: Bytes to feed to decoder

        Returns:
            List of decoded frames (may be empty if no complete frames)

        Raises:
            ValueError: if a frame fails to decode
        """
        frames = []
        for byte in data:
            frame = self.feed_byte(byte)
            if frame is not None:
                frames.append(frame)
        return frames

    def reset(self):
        """Clear the internal buffer."""
        self._buffer.clear()
