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

#include "ecdaa-test-utils.h"

#include "src/amcl-extensions/ecp_ZZZ.h"

#include <stdio.h>
#include <string.h>

static void g1_basepoint_not_inf();
static void g1_serialize_then_deserialize_basepoint();
static void g1_lengths_same();
static void g1_deserialize_badformat_fails();
static void g1_deserialize_badcoords_fails();

int main()
{
    g1_basepoint_not_inf();
    g1_serialize_then_deserialize_basepoint();
    g1_lengths_same();
    g1_deserialize_badformat_fails();
    g1_deserialize_badcoords_fails();

    return 0;
}

void g1_basepoint_not_inf()
{
    printf("Starting ecp_ZZZ::g1_basepoint_not_inf...\n");

    ECP_ZZZ point;
    ecp_ZZZ_set_to_generator(&point);

    TEST_ASSERT(!point.inf);

    printf("\tsuccess\n");
}

static void g1_serialize_then_deserialize_basepoint()
{
    printf("Starting ecp_ZZZ::g1_serialize_then_deserialize_basepoint...\n");

    ECP_ZZZ point;
    ecp_ZZZ_set_to_generator(&point);

    uint8_t buffer[ECP_ZZZ_LENGTH];

    ecp_ZZZ_serialize(buffer, &point);

    ECP_ZZZ deserialized_point;
    TEST_ASSERT(0 == ecp_ZZZ_deserialize(&deserialized_point, buffer));

    TEST_ASSERT(ECP_ZZZ_equals(&point, &deserialized_point));

    printf("\tsuccess\n");
}

static void g1_lengths_same()
{
    printf("Starting ecp_ZZZ::g1_lengths_same...\n");

    TEST_ASSERT(ECP_ZZZ_LENGTH == ecp_ZZZ_length());

    printf("\tsuccess\n");
}

static void g1_deserialize_badformat_fails()
{
    printf("Starting ecp_ZZZ::g1_deserialize_badformat_fails...\n");

    uint8_t buffer[ECP_ZZZ_LENGTH] = {0};
    buffer[0] = 0x3;

    ECP_ZZZ point;
    TEST_ASSERT(-2 == ecp_ZZZ_deserialize(&point, buffer));

    printf("\tsuccess\n");
}

static void g1_deserialize_badcoords_fails()
{
    printf("Starting ecp_ZZZ::g1_deserialize_badcoords_fails...\n");

    uint8_t buffer[ECP_ZZZ_LENGTH] = {0};
    memset(buffer, 1, sizeof(buffer));  // All 1's
    buffer[0] = 0x4;

    ECP_ZZZ point;
    TEST_ASSERT(-1 == ecp_ZZZ_deserialize(&point, buffer));

    printf("\tsuccess\n");
}