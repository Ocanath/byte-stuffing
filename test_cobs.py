"""
Unit tests for COBS implementation.

Mirrors the test cases from the C implementation in test/test_cobs.c
"""

import pytest
from cobs import encode, decode, COBSStreamDecoder


class TestCOBSEncode:
    """Test COBS encoding functionality."""

    def test_encode_empty_raises(self):
        """Test that encoding empty data raises ValueError."""
        with pytest.raises(ValueError):
            encode(bytes())

    def test_encode_single_zero(self):
        """Test encoding: [0x00] -> [0x01 0x01 0x00]"""
        result = encode(bytes([0x00]))
        assert result == bytes([0x01, 0x01, 0x00])

    def test_encode_two_zeros(self):
        """Test encoding: [0x00 0x00] -> [0x01 0x01 0x01 0x00]"""
        result = encode(bytes([0x00, 0x00]))
        assert result == bytes([0x01, 0x01, 0x01, 0x00])

    def test_encode_zero_nonzero_zero(self):
        """Test encoding: [0x00 0x0B 0x00] -> [0x01 0x02 0x0B 0x01 0x00]"""
        result = encode(bytes([0x00, 0x0B, 0x00]))
        assert result == bytes([0x01, 0x02, 0x0B, 0x01, 0x00])

    def test_encode_with_zero_in_middle(self):
        """Test encoding: [0x0B 0x16 0x00 0x21] -> [0x03 0x0B 0x16 0x02 0x21 0x00]"""
        result = encode(bytes([0x0B, 0x16, 0x00, 0x21]))
        assert result == bytes([0x03, 0x0B, 0x16, 0x02, 0x21, 0x00])

    def test_encode_no_zeros(self):
        """Test encoding: [0x0B 0x16 0x21 0x2C] -> [0x05 0x0B 0x16 0x21 0x2C 0x00]"""
        result = encode(bytes([0x0B, 0x16, 0x21, 0x2C]))
        assert result == bytes([0x05, 0x0B, 0x16, 0x21, 0x2C, 0x00])

    def test_encode_zeros_at_beginning(self):
        """Test encoding: [0x0B 0x00 0x00 0x00] -> [0x02 0x0B 0x01 0x01 0x01 0x00]"""
        result = encode(bytes([0x0B, 0x00, 0x00, 0x00]))
        assert result == bytes([0x02, 0x0B, 0x01, 0x01, 0x01, 0x00])

    def test_encode_max_block_254_bytes(self):
        """Test encoding 254 non-zero bytes (max block size)."""
        data = bytes(range(1, 255))  # 1 to 254
        result = encode(data)

        # Should be: 0xFF (full block marker) + 254 bytes + 0x01 (final pointer) + 0x00 (delimiter)
        assert len(result) == 257
        assert result[0] == 0xFF  # Full block pointer
        assert result[1:255] == data
        assert result[255] == 0x01  # Final pointer (no more data)
        assert result[-1] == 0x00  # Delimiter

    def test_encode_255_bytes_with_full_block(self):
        """Test encoding 255 bytes (demonstrating 254-byte block handling)."""
        data = bytes(range(1, 256))  # 1 to 255
        result = encode(data)

        assert len(result) == 258
        assert result[0] == 0xFF  # Full block marker
        assert result[1:255] == bytes(range(1, 255))
        assert result[255] == 0x02  # Pointer to last byte
        assert result[256] == 0xFF
        assert result[-1] == 0x00

    def test_encode_large_message(self):
        """Test encoding large message with multiple full blocks."""
        data = bytes([((i % 255) + 1) & 0xFF for i in range(1018)])
        result = encode(data)
        # Should succeed without error
        assert result[-1] == 0x00  # Ends with delimiter


class TestCOBSDecode:
    """Test COBS decoding functionality."""

    def test_decode_empty_raises(self):
        """Test that decoding empty data raises ValueError."""
        with pytest.raises(ValueError):
            decode(bytes())

    def test_decode_encode_roundtrip(self):
        """Test that decode(encode(x)) == x for various inputs."""
        test_messages = [
            bytes([0x0B, 0x16, 0x00, 0x21]),
            bytes([0x0B, 0x16, 0x21, 0x2C]),
            bytes([0x0B, 0x00, 0x00, 0x00]),
            bytes([0x0B, 0x00, 0x00, 0x0D, 0x0F, 0x10]),
            bytes([0x0B, 0x00, 0x00, 0x00, 0x01, 0x07, 0x10, 0x10, 0x00, 0x00,
                   0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00]),
            bytes([0x0B, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00]),
            bytes([0x0B, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF]),
        ]

        for original_msg in test_messages:
            encoded = encode(original_msg)
            decoded = decode(encoded)
            assert decoded == original_msg

    def test_decode_large_messages(self):
        """Test decoding large messages with full blocks."""
        # Single full block
        msg1 = bytes([((i % 255) + 1) & 0xFF for i in range(256)])

        # Multiple full blocks
        msg2 = bytes([((i % 255) + 1) & 0xFF for i in range(1024)])

        for original_msg in [msg1, msg2]:
            encoded = encode(original_msg)
            decoded = decode(encoded)
            assert decoded == original_msg

    def test_decode_single_zero(self):
        """Test decoding: [0x01 0x01 0x00] -> [0x00]"""
        result = decode(bytes([0x01, 0x01, 0x00]))
        assert result == bytes([0x00])

    def test_decode_no_zeros(self):
        """Test decoding: [0x05 0x0B 0x16 0x21 0x2C 0x00] -> [0x0B 0x16 0x21 0x2C]"""
        result = decode(bytes([0x05, 0x0B, 0x16, 0x21, 0x2C, 0x00]))
        assert result == bytes([0x0B, 0x16, 0x21, 0x2C])


class TestCOBSStream:
    """Test COBS stream decoder."""

    def test_stream_decode_single_frame(self):
        """Test streaming decode of a single COBS frame."""
        original = bytes([0x0B, 0x00, 0x0C, 0x0D])
        encoded = encode(original)

        decoder = COBSStreamDecoder()

        # Feed all but last byte - should return None
        for byte in encoded[:-1]:
            result = decoder.feed_byte(byte)
            assert result is None

        # Feed last byte (delimiter) - should return decoded frame
        result = decoder.feed_byte(encoded[-1])
        assert result == original

    def test_stream_decode_multiple_frames(self):
        """Test streaming multiple frames in sequence."""
        test_messages = [
            bytes([0x0B, 0x00, 0x00, 0x00]),
            bytes([0x0B, 0x0C, 0x0D, 0x00]),
            bytes([0x0B, 0x00, 0x0C, 0x0D]),
            bytes([0x0B, 0x00, 0x00, 0x0D, 0x0F, 0x10]),
            bytes([0x0B, 0x00, 0x00, 0x00, 0x01, 0x07, 0x10, 0x10, 0x00, 0x00,
                   0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00]),
            bytes([0x0B, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00]),
            bytes([0x0B, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF]),
            bytes([(i % 255) + 1 for i in range(1024)]),
        ]

        decoder = COBSStreamDecoder(max_frame_size=2048)

        for original_msg in test_messages:
            encoded = encode(original_msg)

            # Stream decode byte by byte
            decoded_msg = None
            for byte in encoded:
                frame = decoder.feed_byte(byte)
                if frame is not None:
                    decoded_msg = frame

            assert decoded_msg == original_msg

    def test_stream_feed_bytes(self):
        """Test feeding multiple bytes at once."""
        decoder = COBSStreamDecoder(max_frame_size=1024)

        msg1 = bytes([0x01, 0x02, 0x03])
        msg2 = bytes([0x00, 0x00, 0xFF])
        msg3 = bytes([0xAA, 0xBB, 0xCC, 0xDD])

        # Encode all messages into one stream
        encoded_stream = b''.join(encode(msg) for msg in [msg1, msg2, msg3])

        # Decode stream
        frames = decoder.feed_bytes(encoded_stream)

        assert len(frames) == 3
        assert frames[0] == msg1
        assert frames[1] == msg2
        assert frames[2] == msg3

    def test_stream_buffer_overrun(self):
        """Test that buffer overrun raises ValueError."""
        decoder = COBSStreamDecoder(max_frame_size=10)

        # Feed more bytes than max_frame_size without delimiter
        with pytest.raises(ValueError, match="Buffer overrun"):
            for i in range(15):
                decoder.feed_byte(i + 1)

    def test_stream_reset(self):
        """Test resetting the stream decoder."""
        decoder = COBSStreamDecoder()

        # Feed some bytes
        decoder.feed_byte(0x01)
        decoder.feed_byte(0x02)

        # Reset
        decoder.reset()

        # Buffer should be empty
        assert len(decoder._buffer) == 0


class TestHighLevelAPI:
    """Test high-level encode/decode functions."""

    def test_encode_decode_simple(self):
        """Test simple encode/decode using high-level API."""
        original = bytes([0x01, 0x00, 0x02, 0x00, 0x03])
        encoded = encode(original)
        decoded = decode(encoded)
        assert decoded == original

    def test_encode_no_zeros(self):
        """Test encoding data with no zeros."""
        original = bytes([0x01, 0x02, 0x03, 0x04])
        encoded = encode(original)
        decoded = decode(encoded)
        assert decoded == original

    def test_encode_all_zeros(self):
        """Test encoding data that's all zeros."""
        original = bytes([0x00, 0x00, 0x00, 0x00])
        encoded = encode(original)
        decoded = decode(encoded)
        assert decoded == original

    def test_encode_large_data(self):
        """Test encoding large data."""
        original = bytes([(i % 256) for i in range(1000)])
        encoded = encode(original)
        decoded = decode(encoded)
        assert decoded == original

    def test_wikipedia_examples(self):
        """Test examples from Wikipedia COBS article."""
        # Example 1: [0x00] -> [0x01 0x01 0x00]
        assert encode(bytes([0x00])) == bytes([0x01, 0x01, 0x00])

        # Example 2: [0x00 0x00] -> [0x01 0x01 0x01 0x00]
        assert encode(bytes([0x00, 0x00])) == bytes([0x01, 0x01, 0x01, 0x00])

        # Example 3: [0x11 0x22 0x00 0x33] -> [0x03 0x11 0x22 0x02 0x33 0x00]
        assert encode(bytes([0x11, 0x22, 0x00, 0x33])) == bytes([0x03, 0x11, 0x22, 0x02, 0x33, 0x00])

        # Example 4: [0x11 0x22 0x33 0x44] -> [0x05 0x11 0x22 0x33 0x44 0x00]
        assert encode(bytes([0x11, 0x22, 0x33, 0x44])) == bytes([0x05, 0x11, 0x22, 0x33, 0x44, 0x00])

        # Example 5: [0x11 0x00 0x00 0x00] -> [0x02 0x11 0x01 0x01 0x01 0x00]
        assert encode(bytes([0x11, 0x00, 0x00, 0x00])) == bytes([0x02, 0x11, 0x01, 0x01, 0x01, 0x00])


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
