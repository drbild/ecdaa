/******************************************************************************
 *
 * Copyright 2017 Xaptum, Inc.
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License
 *
 *****************************************************************************/

#include "xaptum-test-utils.h"

#include "../src/amcl-extensions/big_256_56.h"

#include <amcl/ecp_BN254.h>

#include <stdio.h>
#include <string.h>

static void hash_not_zero();
static void hash_two_not_zero();
static void hash_ok_with_no_msg();
static void hash_same_message();
static void mul_and_add_all_zeros();
static void mul_and_add_all_ones();
static void mul_and_add_modulus_two();
static void mul_and_add_normalization_works();
static void mul_and_add_greater_than_modulus_ok();
static void mul_and_add_small_sanity_check();
static void random_num_mod_order_is_valid();

int main()
{
    hash_not_zero();
    hash_two_not_zero();
    hash_ok_with_no_msg();
    hash_same_message();
    mul_and_add_all_zeros();
    mul_and_add_all_ones();
    mul_and_add_modulus_two();
    mul_and_add_normalization_works();
    mul_and_add_greater_than_modulus_ok();
    mul_and_add_small_sanity_check();
    random_num_mod_order_is_valid();
}

void hash_not_zero()
{
    printf("Starting mpi_utils::hash_not_zero...\n");

    BIG_256_56 mpi;
    uint8_t msg[1024];
    uint32_t msg_len = 1024;
    big_256_56_from_hash(&mpi, msg, msg_len);

    TEST_ASSERT(0 == BIG_256_56_iszilch(mpi));

    TEST_ASSERT(0 == BIG_256_56_isunity(mpi));

    printf("\tsuccess!\n");
}

void hash_two_not_zero()
{
    printf("Starting mpi_utils::hash_two_not_zero...\n");

    BIG_256_56 mpi;
    uint8_t msg1[1024];
    uint32_t msg1_len = 1024;
    uint8_t msg2[1024];
    uint32_t msg2_len = 1024;
    big_256_56_from_two_message_hash(&mpi, msg1, msg1_len, msg2, msg2_len);

    TEST_ASSERT(0 == BIG_256_56_iszilch(mpi));

    TEST_ASSERT(0 == BIG_256_56_isunity(mpi));

    printf("\tsuccess!\n");
}

void hash_ok_with_no_msg()
{
    printf("Starting mpi_utils::hash_ok_with_no_msg...\n");

    BIG_256_56 mpi;
    uint8_t *msg = NULL;
    uint32_t msg_len = 0;
    big_256_56_from_hash(&mpi, msg, msg_len);

    TEST_ASSERT(0 == BIG_256_56_iszilch(mpi));

    TEST_ASSERT(0 == BIG_256_56_isunity(mpi));

    printf("\tsuccess!\n");
}

void hash_same_message()
{
    printf("Starting mpi_utils::hash_same_message...\n");

    BIG_256_56 mpi1, mpi2;
    uint8_t *msg = (uint8_t*) "Test message";
    uint32_t msg_len = strlen((char*) msg);
    big_256_56_from_hash(&mpi1, msg, msg_len);
    big_256_56_from_hash(&mpi2, msg, msg_len);

    TEST_ASSERT(0 == BIG_256_56_comp(mpi1, mpi2));

    printf("\tsuccess!\n");
}

void hash_two_same_messages()
{
    printf("Starting mpi_utils::hash_two_same_messages...\n");

    BIG_256_56 mpi1, mpi2;
    uint8_t *msg1 = (uint8_t*) "Test message";
    uint32_t msg1_len = strlen((char*) msg1);
    uint8_t *msg2 = (uint8_t*) "Test message numero dos";
    uint32_t msg2_len = strlen((char*) msg2);

    big_256_56_from_two_message_hash(&mpi1, msg1, msg1_len, msg2, msg2_len);
    big_256_56_from_two_message_hash(&mpi2, msg1, msg1_len, msg2, msg2_len);

    TEST_ASSERT(0 == BIG_256_56_comp(mpi1, mpi2));

    printf("\tsuccess!\n");
}

void mul_and_add_all_zeros()
{
    printf("Starting mpi_utils::mul_and_add_all_zeros...\n");

    BIG_256_56 modulus;
    BIG_256_56_rcopy(modulus, CURVE_Order_BN254);
    TEST_ASSERT(0 == BIG_256_56_iszilch(modulus));

    BIG_256_56 m, x, b, y;
    BIG_256_56_zero(m);
    BIG_256_56_zero(x);
    BIG_256_56_zero(b);

    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);
    TEST_ASSERT(1 == BIG_256_56_iszilch(b));

    printf("\tsuccess\n");
}

void mul_and_add_all_ones()
{
    printf("Starting mpi_utils::mul_and_add_all_ones...\n");

    BIG_256_56 modulus;
    BIG_256_56_rcopy(modulus, CURVE_Order_BN254);
    TEST_ASSERT(0 == BIG_256_56_iszilch(modulus));

    BIG_256_56 m, x, b, y;
    BIG_256_56_one(m);
    BIG_256_56_one(x);
    BIG_256_56_one(b);

    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);

    BIG_256_56 expected = {0};
    expected[0] = 2;

    TEST_ASSERT(0 == BIG_256_56_comp(expected, y));

    printf("\tsuccess\n");
}

void mul_and_add_modulus_two()
{
    printf("Starting mpi_utils::mul_and_add_modulus_two...\n");

    BIG_256_56 modulus = {0};
    modulus[0] = 2; // Any even number should be congruent to 0 mod 2.

    BIG_256_56 m={0}, x={0}, b={0};
    m[0] = 109238;
    x[0] = 892001;
    b[0] = 2;   // Should make final answer even.

    BIG_256_56 y;
    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);

    TEST_ASSERT(1 == BIG_256_56_iszilch(y));

    printf("\tsuccess\n");
}

void mul_and_add_normalization_works()
{
    printf("Starting mpi_utils::mul_and_add_normalization_works...\n");

    BIG_256_56 modulus;
    BIG_256_56_rcopy(modulus, CURVE_Order_BN254);
    TEST_ASSERT(0 == BIG_256_56_iszilch(modulus));

    // Just some random numbers, which make these non-normalized.
    BIG_256_56 m={12301239048798, INT64_MAX, 2034019249817234, INT64_MAX};
    TEST_ASSERT(-1 == BIG_256_56_comp(m, modulus));
    BIG_256_56 x={918741923908734908, 3984712409814, INT64_MAX, 2394871290871};
    TEST_ASSERT(-1 == BIG_256_56_comp(x, modulus));
    BIG_256_56 b={INT64_MAX, 13946129908734, 0, 19867128968128741};
    TEST_ASSERT(-1 == BIG_256_56_comp(b, modulus));

    BIG_256_56 m_norm, x_norm, b_norm;
    BIG_256_56_copy(m_norm, m);
    BIG_256_56_norm(m_norm);
    BIG_256_56_copy(x_norm, x);
    BIG_256_56_norm(x_norm);
    BIG_256_56_copy(b_norm, b);
    BIG_256_56_norm(b_norm);

    BIG_256_56 y;
    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);

    BIG_256_56 y_expected;
    big_256_56_mod_mul_and_add(&y_expected, b_norm, m_norm, x_norm, modulus);

    TEST_ASSERT(0 == BIG_256_56_comp(m, m_norm));
    TEST_ASSERT(0 == BIG_256_56_comp(x, x_norm));
    TEST_ASSERT(0 == BIG_256_56_comp(b, b_norm));

    TEST_ASSERT(0 == BIG_256_56_comp(y, y_expected));

    printf("\tsuccess\n");
}

void mul_and_add_greater_than_modulus_ok()
{
    printf("Starting mpi_utils::mul_and_add_greater_than_modulus_ok...\n");

    BIG_256_56 modulus;
    BIG_256_56_rcopy(modulus, CURVE_Order_BN254);
    TEST_ASSERT(0 == BIG_256_56_iszilch(modulus));

    BIG_256_56 excess = {0};
    excess[0] = 5239;   // amount by which MPIs will be > modulus (i.e. congruent to excess)

    // Three MPIs that are > modulus (but still normalized):
    BIG_256_56 m;
    BIG_256_56_copy(m, modulus);
    BIG_256_56_add(m, m, excess);
    BIG_256_56_norm(m);
    TEST_ASSERT(1 == BIG_256_56_comp(m, modulus));

    BIG_256_56 x;
    BIG_256_56_copy(x, modulus);
    BIG_256_56_add(x, x, excess);
    BIG_256_56_norm(x);
    TEST_ASSERT(1 == BIG_256_56_comp(x, modulus));

    BIG_256_56 b;
    BIG_256_56_copy(b, modulus);
    BIG_256_56_add(b, b, excess);
    BIG_256_56_norm(b);
    TEST_ASSERT(1 == BIG_256_56_comp(b, modulus));

    // Those MPIs mod'd by modulus:
    BIG_256_56 m_mod, x_mod, b_mod;
    BIG_256_56_copy(m_mod, m);
    BIG_256_56_mod(m_mod, modulus);
    BIG_256_56_copy(x_mod, x);
    BIG_256_56_mod(x_mod, modulus);
    BIG_256_56_copy(b_mod, b);
    BIG_256_56_mod(b_mod, modulus);

    BIG_256_56 y;
    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);

    BIG_256_56 y_expected;
    big_256_56_mod_mul_and_add(&y_expected, b_mod, m_mod, x_mod, modulus);

    TEST_ASSERT(0 == BIG_256_56_comp(y, y_expected));

    printf("\tsuccess\n");
}

void mul_and_add_small_sanity_check()
{
    printf("Starting mpi_utils::mul_and_add_small_sanity_check...\n");

    BIG_256_56 modulus = {0};
    modulus[0] = 13;

    BIG_256_56 m={0}, x={0}, b={0};
    m[0] = 9;
    x[0] = 15;
    b[0] = 4;

    BIG_256_56 y;
    BIG_256_56 y_expected = {0};
    y_expected[0] = 9;  // 4 + 9*15 mod 13 = 9

    big_256_56_mod_mul_and_add(&y, b, m, x, modulus);

    TEST_ASSERT(0 == BIG_256_56_comp(y_expected, y));

    printf("\tsuccess\n");
}

void random_num_mod_order_is_valid()
{
    printf("Starting pairing_curve_utils::random_num_mod_order_is_valid...\n");

    BIG_256_56 curve_order;
    BIG_256_56_rcopy(curve_order, CURVE_Order_BN254);

    csprng rng;
    create_test_rng(&rng);

    BIG_256_56 num;
    for (int i = 0; i < 50000; ++i) {
        big_256_56_random_mod_order(&num, &rng);

        TEST_ASSERT(BIG_256_56_iszilch(num) == 0);
        TEST_ASSERT(BIG_256_56_isunity(num) == 0);

        TEST_ASSERT(BIG_256_56_comp(num, curve_order) == -1);
    }

    destroy_test_rng(&rng);

    printf("\tsuccess\n");
}
