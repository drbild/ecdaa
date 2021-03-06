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

#ifndef XAPTUM_ECDAA_BIG_256_56_EXTENSIONS_H
#define XAPTUM_ECDAA_BIG_256_56_EXTENSIONS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <amcl/big_256_56.h>
#include <amcl/amcl.h>
#include <amcl/randapi.h>

#include <stdint.h>

/*
 * Hash the supplied message and convert to a BIG_256_56.
 *
 * Output BIG_256_56 is un-normalized (and _not_ modulo any group order).
 *
 * Intermediate memory is cleared.
 */
void big_256_56_from_hash(BIG_256_56 *big_out,
                          const uint8_t *msg_in,
                          uint32_t msg_len);

/*
 * Same as big_256_56_from_hash, but with two input messages.
 */
void big_256_56_from_two_message_hash(BIG_256_56 *big_out,
                                      const uint8_t *msg1_in,
                                      uint32_t msg1_len,
                                      const uint8_t *msg2_in,
                                      uint32_t msg2_len);

/*
 * Multiply two BIG_256_56's, then add the product to a third BIG_256_56, all modulo a given modulus.
 *
 * BIG_256_56_out = [ summand + (multiplicand1 * multiplicand2) ] mod modulus
 *
 * Inputs don't need to be normalized, and will be normalized (and reduced modulo `modulus`) after return.
 * Output is normalized.
 */
void big_256_56_mod_mul_and_add(BIG_256_56 *big_out,
                                BIG_256_56 summand,
                                BIG_256_56 multiplicand1,
                                BIG_256_56 multiplicand2,
                                BIG_256_56 modulus);

/*
 * Generate a uniformly-distributed pseudo-random number,
 * between [0, n], where n is the order of the EC group.
 *
 * Output is normalized.
 */
void big_256_56_random_mod_order(BIG_256_56 *big_out,
                                 csprng *rng);

#ifdef __cplusplus
}
#endif

#endif
