#include "unity.h"
#include "cobs.h"
#include <stdlib.h>
#include <time.h>

void setUp(void)
{

}

void tearDown(void)
{

}

void test_cobs_encode_null_pointer(void)
{
    int rc = cobs_encode(NULL);
    TEST_ASSERT_EQUAL(rc, -1);
}