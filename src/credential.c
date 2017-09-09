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

#include <ecdaa/credential.h>

#include "schnorr.h"
#include "explicit_bzero.h"
#include "pairing_curve_utils.h"

#include <ecdaa/member_keypair.h>
#include <ecdaa/issuer_keypair.h>
#include <ecdaa/group_public_key.h>

#include "pairing_curve_utils.h"

size_t serialized_credential_length()
{
    return SERIALIZED_CREDENTIAL_LENGTH;
}

size_t serialized_credential_signature_length()
{
    return SERIALIZED_CREDENTIAL_SIGNATURE_LENGTH;
}

int ecdaa_generate_credential(struct ecdaa_credential *cred,
                              struct ecdaa_credential_signature *cred_sig_out,
                              struct ecdaa_issuer_secret_key *isk,
                              struct ecdaa_member_public_key *member_pk,
                              csprng *rng)
{
    int ret = 0;

    BIG_256_56 curve_order;
    BIG_256_56_rcopy(curve_order, CURVE_Order_BN254);

    // 1) Choose random l <- Z_p
    BIG_256_56 l;
    random_num_mod_order(&l, rng);

    // 2) Multiply generator by l and save to cred->A (A = l*P)
    set_to_basepoint(&cred->A);
    ECP_BN254_mul(&cred->A, l);

    // 3) Multiply A by my secret y and save to cred->B (B = y*A)
    ECP_BN254_copy(&cred->B, &cred->A);
    ECP_BN254_mul(&cred->B, isk->y);

    // 4) Mod-multiply l and y
    BIG_256_56 ly;
    BIG_256_56_modmul(ly, l, isk->y, curve_order);

    // 5) Multiply member's public_key by ly and save to cred->D (D = ly*Q)
    ECP_BN254_copy(&cred->D, &member_pk->Q);
    ECP_BN254_mul(&cred->D, ly);

    // 6) Multiply A by my secret x (store in cred->C temporarily)
    ECP_BN254_copy(&cred->C, &cred->A);
    ECP_BN254_mul(&cred->C, isk->x);

    // 7) Mod-multiply ly (see step 4) by my secret x
    BIG_256_56 xyl;
    BIG_256_56_modmul(xyl, ly, isk->x, curve_order);

    // 8) Multiply member's public_key by xyl
    ECP_BN254 Qxyl;
    ECP_BN254_copy(&Qxyl, &member_pk->Q);
    ECP_BN254_mul(&Qxyl, xyl);

    // 9) Add Ax and xyl*Q and save to cred->C (C = x*A + xyl*Q)
    ECP_BN254_add(&cred->C, &Qxyl);
    // Nb. No need to call ECP_BN254_affine here,
    // as C always gets multiplied during signing (which implicitly converts to affine)

    // 10) Perform a Schnorr-like signature,
    //  to prove the credential was properly constructed by someone with knowledge of y.
    int schnorr_ret = credential_schnorr_sign(&cred_sig_out->c,
                                          &cred_sig_out->s,
                                          &cred->B,
                                          &member_pk->Q,
                                          &cred->D,
                                          isk->y,
                                          l,
                                          rng);
    if (0 != schnorr_ret)
        ret = -1;

    // Clear sensitive intermediate memory
    explicit_bzero(&l, sizeof(BIG_256_56));

    return ret;
}

int ecdaa_validate_credential(struct ecdaa_credential *credential,
                              struct ecdaa_credential_signature *credential_signature,
                              struct ecdaa_member_public_key *member_pk,
                              struct ecdaa_group_public_key *gpk)
{
    int ret = 0;

    // 1) Check A,B,C,D for membership in group, and A for !=inf
    if (0 != check_point_membership(&credential->A)
            || 0 != check_point_membership(&credential->B)
            || 0 != check_point_membership(&credential->C)
            || 0 != check_point_membership(&credential->D))
        ret = -1;
    if (ECP_BN254_isinf(&credential->A))
        ret = -1;
    
    // 2) Verify schnorr-like signature
    int schnorr_ret = credential_schnorr_verify(credential_signature->c,
                                            credential_signature->s,
                                            &credential->B,
                                            &member_pk->Q,
                                            &credential->D);
    if (0 != schnorr_ret)
        ret = -1;

    ECP2_BN254 basepoint2;
    set_to_basepoint2(&basepoint2);

    // 3) Check e(A, Y) == e(B, P_2)
    FP12_BN254 pairing_one;
    FP12_BN254 pairing_one_prime;
    compute_pairing(&pairing_one, &credential->A, &gpk->Y);
    compute_pairing(&pairing_one_prime, &credential->B, &basepoint2);
    if (!FP12_BN254_equals(&pairing_one, &pairing_one_prime))
        ret = -1;

    // 4) Compute A+D
    ECP_BN254 AD;
    ECP_BN254_copy(&AD, &credential->A);
    ECP_BN254_add(&AD, &credential->D);

    // 5) Check e(C, P_2) == e(A+D, X)
    FP12_BN254 pairing_two;
    FP12_BN254 pairing_two_prime;
    compute_pairing(&pairing_two, &credential->C, &basepoint2);
    compute_pairing(&pairing_two_prime, &AD, &gpk->X);
    if (!FP12_BN254_equals(&pairing_two, &pairing_two_prime))
        ret = -1;

    return ret;
}

void ecdaa_serialize_credential(uint8_t *buffer_out,
                                struct ecdaa_credential *credential)
{
    serialize_point(buffer_out, &credential->A);
    serialize_point(buffer_out + serialized_point_length(), &credential->B);
    serialize_point(buffer_out + 2*serialized_point_length(), &credential->C);
    serialize_point(buffer_out + 3*serialized_point_length(), &credential->D);
}

void ecdaa_serialize_credential_signature(uint8_t *buffer_out,
                                          struct ecdaa_credential_signature *cred_sig)
{
    BIG_256_56_toBytes((char*)buffer_out, cred_sig->c);
    BIG_256_56_toBytes((char*)(buffer_out + MODBYTES_256_56), cred_sig->s);
}

int ecdaa_deserialize_credential_with_signature(struct ecdaa_credential *credential_out,
                                                struct ecdaa_member_public_key *member_pk,
                                                struct ecdaa_group_public_key *gpk,
                                                uint8_t *buffer_in)
{
    int ret = 0;

    // 1) De-serialize the credential
    ret = ecdaa_deserialize_credential(credential_out, buffer_in);

    // 2) De-serialize the credential signature
    struct ecdaa_credential_signature cred_sig;
    BIG_256_56_fromBytes(cred_sig.c, (char*)(buffer_in + 4*serialized_point_length()));
    BIG_256_56_fromBytes(cred_sig.c, (char*)(buffer_in + 4*serialized_point_length() + MODBYTES_256_56));

    if (0 == ret) {
        int valid_ret = ecdaa_validate_credential(credential_out, &cred_sig, member_pk, gpk);
        if (0 != valid_ret)
            ret = -2;
    }

    return ret;
}

int ecdaa_deserialize_credential(struct ecdaa_credential *credential_out,
                                 uint8_t *buffer_in)
{
    int ret = 0;

    if (0 != deserialize_point(&credential_out->A, buffer_in))
        ret = -1;

    if (0 != deserialize_point(&credential_out->B, buffer_in + serialized_point_length()))
        ret = -1;

    if (0 != deserialize_point(&credential_out->C, buffer_in + 2*serialized_point_length()))
        ret = -1;

    if (0 != deserialize_point(&credential_out->D, buffer_in + 3*serialized_point_length()))
        ret = -1;

    return ret;
}
