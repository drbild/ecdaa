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

#ifndef XAPTUM_ECDAA_ECP2_BN254_EXTENSIONS_H
#define XAPTUM_ECDAA_ECP2_BN254_EXTENSIONS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <amcl/ecp2_BN254.h>

#include <stddef.h>

#define ECP2_BN254_LENGTH (4*MODBYTES_256_56 + 1)
size_t ecp2_BN254_length();

/*
 * Initialize ECP2_BN254 point to G2 generator.
 */
void ecp2_BN254_set_to_generator(ECP2_BN254 *point);

/*
 * Check if the given ECP2_BN254 point is a member of G2.
 *
 * Returns:
 * 0 on success
 * -1 if the point is _not_ in G2
 */
int ecp2_BN254_check_membership(ECP2_BN254 *point);

/*
 * Serialize an ECP2_BN254 point.
 *
 * Format: ( 0x04 | x-coordinate | y-coordinate )
 */
void ecp2_BN254_serialize(uint8_t *buffer_out,
                         ECP2_BN254 *point);

/*
 * De-serialize an ECP2_BN254 point.
 *
 * Format: ( 0x04 | x-coordinate | y-coordinate )
 *
 * Returns:
 * 0 on success
 * -1 if the point is not on the curve
 */
int ecp2_BN254_deserialize(ECP2_BN254 *point_out,
                           const uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif

