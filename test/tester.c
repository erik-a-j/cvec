#include "cvec.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_cvec_init(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    TEST_ASSERT_NULL(v.data);
    TEST_ASSERT_EQUAL_size_t(sizeof(int), v.memb_size);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_cvec_init);

    UNITY_END();

    return 0;
}