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

#include "./ecp2_BN254.h"

size_t ecp2_BN254_length()
{
    return ECP2_BN254_LENGTH;
}

void ecp2_BN254_set_to_generator(ECP2_BN254 *point)
{
    BIG_256_56 xa, xb, ya, yb;
    FP2_BN254 x, y;

    BIG_256_56_rcopy(xa, CURVE_Pxa_BN254);
    BIG_256_56_rcopy(xb, CURVE_Pxb_BN254);
    BIG_256_56_rcopy(ya, CURVE_Pya_BN254);
    BIG_256_56_rcopy(yb, CURVE_Pyb_BN254);

    FP2_BN254_from_BIGs(&x, xa, xb);
    FP2_BN254_from_BIGs(&y, ya, yb);

    ECP2_BN254_set(point, &x, &y);
}

int ecp2_BN254_check_membership(ECP2_BN254 *point)
{
    // TODO: Check if this is correct!
    ECP2_BN254 point_copy;
    ECP2_BN254_copy(&point_copy, point);

    BIG_256_56 curve_order;
    BIG_256_56_rcopy(curve_order, CURVE_Order_BN254);

    /* Check point is not in wrong group */
    int nb = BIG_256_56_nbits(curve_order);
    BIG_256_56 k;
    BIG_256_56_one(k);
    BIG_256_56_shl(k, (nb+4)/2);
    BIG_256_56_add(k, curve_order, k);
    BIG_256_56_sdiv(k, curve_order); /* get co-factor */

    while (BIG_256_56_parity(k) == 0) {
        ECP2_BN254_dbl(&point_copy);
        BIG_256_56_fshr(k,1);
    }

    if (!BIG_256_56_isunity(k))
        ECP2_BN254_mul(&point_copy,k);
    if (ECP2_BN254_isinf(&point_copy))
        return -1;

    return 0;
}

void ecp2_BN254_serialize(uint8_t *buffer_out,
                         ECP2_BN254 *point)
{
    buffer_out[0] = 0x04;

    octet as_oct = {.len = 0,
                    .max = ECP2_BN254_LENGTH,
                    .val = (char*)buffer_out + 1};

    ECP2_BN254_toOctet(&as_oct, point);
}

int ecp2_BN254_deserialize(ECP2_BN254 *point_out,
                           const uint8_t *buffer)
{
    int ret = 0;

    if (0x04 != buffer[0])
        ret = -1;

    octet as_oct = {.len = 0,
                    .max = ECP2_BN254_LENGTH,
                    .val = (char*)(buffer + 1)};

    int from_ret = ECP2_BN254_fromOctet(point_out, &as_oct);
    if (1 != from_ret)
        ret = -1;

    return ret;
}
