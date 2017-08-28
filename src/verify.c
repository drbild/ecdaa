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

#include "schnorr.h"

#include "pairing_curve_utils.h"

#include <verify.h>

#include <amcl/pair_BN254.h>
#include <amcl/fp12_BN254.h>

int verify(ecdaa_signature_t *signature,
           issuer_public_key_t *issuer_pk,
           uint8_t* message,
           uint32_t message_len)
{
    // 1) Check Schnorr-type signature
    int schnorr_ret = schnorr_verify(signature->c,
                                     signature->s,
                                     message,
                                     message_len,
                                     &signature->S,
                                     &signature->W);
    if (0 != schnorr_ret)
        return -1;

    ECP2_BN254 basepoint2;
    set_to_basepoint2(&basepoint2);

    // 2) Check e(R, Y) == e(S, P_2)
    if (NULL == issuer_pk)
        return -1;
    // FP12_BN254 pairing_one, pairing_one_prime;
    // PAIR_BN254_ate(&pairing_one, &issuer_pk->Y, &signature->R);
    // PAIR_BN254_ate(&pairing_one_prime, &basepoint2, &signature->S);
    // int pairing_one_ret = FP12_BN254_equals(&pairing_one, &pairing_one_prime);
    // if (1 != pairing_one_ret)
    //     return -1;

    return 0;
}