#include "unity.h"
#include "PPP.h"
#include <stdlib.h>
#include <time.h>

#define PAYLOAD_SIZE 8
#define BUFFER_SIZE 128

// Test fixtures
static uint8_t stuffing_buffer[BUFFER_SIZE];
static uint8_t unstuffing_buffer[BUFFER_SIZE];
static uint8_t example_data[PAYLOAD_SIZE];
static buffer_t stuffed_buffer;
static buffer_t input_buffer;
static buffer_t unstuffed_buffer;

void setUp(void)
{
    // Initialize buffers
    memset(stuffing_buffer, 0, BUFFER_SIZE);
    memset(unstuffing_buffer, 0, BUFFER_SIZE);
    memset(example_data, 0, PAYLOAD_SIZE);
    
    stuffed_buffer.buf = stuffing_buffer;
    stuffed_buffer.size = BUFFER_SIZE;
    stuffed_buffer.length = 0;
    
    input_buffer.buf = unstuffing_buffer;
    input_buffer.size = BUFFER_SIZE;
    input_buffer.length = 0;
    
    unstuffed_buffer.buf = example_data;
    unstuffed_buffer.size = PAYLOAD_SIZE;
    unstuffed_buffer.length = 0;
}

void tearDown(void)
{
    // Clean up if needed
}

// Helper function to print hex buffer (for debugging)
void print_hex_buffer(uint8_t * arr, int size)
{
    printf("0x");
    for(int i = 0; i < size; i++)
    {
        if(i < size-1)
            printf("%02X ", arr[i]);
        else
            printf("%02X", arr[i]);
    }
}

// Test 1: Basic PPP stuffing with random data
void test_PPP_stuff_basic_random_data(void)
{
    // Generate random test data
    srand(time(NULL));
    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        example_data[i] = rand() % 0x100;
    }
    unstuffed_buffer.length = PAYLOAD_SIZE;
    
    // Perform stuffing
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
    
    // Verify stuffing was successful
    TEST_ASSERT_GREATER_THAN(0, stuffed_size);
    TEST_ASSERT_LESS_OR_EQUAL(BUFFER_SIZE, stuffed_size);
    
    // Verify frame characters are present
    TEST_ASSERT_EQUAL(FRAME_CHAR, stuffed_buffer.buf[0]);
    TEST_ASSERT_EQUAL(FRAME_CHAR, stuffed_buffer.buf[stuffed_size - 1]);
    
    printf("Original Data: ");
    print_hex_buffer(example_data, PAYLOAD_SIZE);
    printf("\nStuffed data: ");
    print_hex_buffer(stuffed_buffer.buf, stuffed_size);
    printf("\nSize: %d\n", stuffed_size);
}

// Test 2: PPP stuffing with frame character in data
void test_PPP_stuff_with_frame_character(void)
{
    // Create test data with frame character
    for (int i = 0; i < 3; i++)
    {
        example_data[i] = rand() % 0x100;
    }
    example_data[3] = FRAME_CHAR;
    unstuffed_buffer.length = 4;
    
    // Perform stuffing
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
    
    // Verify stuffing was successful
    TEST_ASSERT_GREATER_THAN(0, stuffed_size);
    TEST_ASSERT_LESS_OR_EQUAL(BUFFER_SIZE, stuffed_size);
    
    // Verify frame characters are present
    TEST_ASSERT_EQUAL(FRAME_CHAR, stuffed_buffer.buf[0]);
    TEST_ASSERT_EQUAL(FRAME_CHAR, stuffed_buffer.buf[stuffed_size - 1]);
    
    // Verify the frame character was escaped
    TEST_ASSERT_EQUAL(ESC_CHAR, stuffed_buffer.buf[4]);
    TEST_ASSERT_EQUAL(FRAME_CHAR ^ ESC_MASK, stuffed_buffer.buf[5]);
    
    printf("Data with frame char: ");
    print_hex_buffer(example_data, 4);
    printf("\nStuffed data: ");
    print_hex_buffer(stuffed_buffer.buf, stuffed_size);
    printf("\nSize: %d\n", stuffed_size);
}

// Test 3: PPP unstuffing
void test_PPP_unstuff_basic(void)
{
    // Create test data
    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        example_data[i] = rand() % 0x100;
    }
    unstuffed_buffer.length = PAYLOAD_SIZE;
    
    // Stuff the data first
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
    TEST_ASSERT_GREATER_THAN(0, stuffed_size);
    TEST_ASSERT_GREATER_THAN(0, stuffed_buffer.length);
    TEST_ASSERT_EQUAL(stuffed_buffer.length, stuffed_size);
    
    // Reset unstuffed buffer
    memset(example_data, 0, PAYLOAD_SIZE);
    unstuffed_buffer.length = 0;
    
    // Unstuff the data
    int unstuffed_size = PPP_unstuff(&unstuffed_buffer, &stuffed_buffer);
    
    // Verify unstuffing was successful
    TEST_ASSERT_EQUAL(PAYLOAD_SIZE, unstuffed_size);
    
    // Verify data matches original
    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(example_data[i], unstuffed_buffer.buf[i]);
    }
    
    printf("Original: ");
    print_hex_buffer(example_data, PAYLOAD_SIZE);
    printf("\nUnstuffed: ");
    print_hex_buffer(unstuffed_buffer.buf, unstuffed_size);
    printf("\nSize: %d\n", unstuffed_size);
}

// Test 4: PPP stream parsing
void test_PPP_stream_parsing(void)
{
    // Create test data
    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        example_data[i] = rand() % 0x100;
    }
    unstuffed_buffer.length = PAYLOAD_SIZE;
    
    // Stuff the data
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
    TEST_ASSERT_GREATER_THAN(0, stuffed_size);
    
    // Reset buffers for stream parsing
    memset(example_data, 0, PAYLOAD_SIZE);
    unstuffed_buffer.length = 0;
    input_buffer.length = 0;
    
    // Simulate stream parsing byte by byte
    int total_payloads_found = 0;
    for(int i = 0; i < stuffed_size; i++)
    {
        uint8_t new_byte = stuffed_buffer.buf[i];
        int pld_size = parse_PPP_stream(new_byte, &unstuffed_buffer, &input_buffer);
        
        if(pld_size != 0)
        {
            total_payloads_found++;
            printf("Stream payload found: ");
            print_hex_buffer(unstuffed_buffer.buf, pld_size);
            printf("\nSize: %d\n", pld_size);
            
            // Verify payload size matches original
            TEST_ASSERT_EQUAL(PAYLOAD_SIZE, pld_size);
        }
    }
    
    // Verify we found exactly one payload
    TEST_ASSERT_EQUAL(1, total_payloads_found);
}

// Test 5: Buffer overflow protection
void test_PPP_stuff_buffer_overflow(void)
{
    // Create a buffer that's too small
    uint8_t small_buffer[4];
    buffer_t small_stuffed_buffer = {
        .buf = small_buffer,
        .size = 4,
        .length = 0
    };
    
    // Fill with data that would cause overflow
    for (int i = 0; i < PAYLOAD_SIZE; i++)
    {
        example_data[i] = 0xAA;
    }
    unstuffed_buffer.length = PAYLOAD_SIZE;
    
    // Attempt stuffing - should fail
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &small_stuffed_buffer);
    TEST_ASSERT_EQUAL(0, stuffed_size);
}

// Test 6: Invalid frame detection
void test_PPP_unstuff_invalid_frame(void)
{
    // Create invalid stuffed data (no frame character at start)
    stuffed_buffer.buf[0] = 0xAA;  // Not FRAME_CHAR
    stuffed_buffer.buf[1] = 0xBB;
    stuffed_buffer.buf[2] = 0xCC;
    stuffed_buffer.length = 3;
    
    // Attempt unstuffing - should fail
    int unstuffed_size = PPP_unstuff(&unstuffed_buffer, &stuffed_buffer);
    TEST_ASSERT_EQUAL(0, unstuffed_size);
}

// Test 7: ESC character handling
void test_PPP_stuff_with_esc_character(void)
{
    // Create test data with ESC character
    example_data[0] = ESC_CHAR;
    example_data[1] = 0xAA;
    example_data[2] = 0xBB;
    unstuffed_buffer.length = 3;
    
    // Perform stuffing
    int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
    
    // Verify stuffing was successful
    TEST_ASSERT_GREATER_THAN(0, stuffed_size);
    
    // Verify the ESC character was escaped
    TEST_ASSERT_EQUAL(ESC_CHAR, stuffed_buffer.buf[1]);
    TEST_ASSERT_EQUAL(ESC_CHAR ^ ESC_MASK, stuffed_buffer.buf[2]);
    
    printf("Data with ESC char: ");
    print_hex_buffer(example_data, 3);
    printf("\nStuffed data: ");
    print_hex_buffer(stuffed_buffer.buf, stuffed_size);
    printf("\nSize: %d\n", stuffed_size);
}

