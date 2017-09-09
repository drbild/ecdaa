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

#ifndef XAPTUM_ECDAA_SK_REVOCATION_LIST_H
#define XAPTUM_ECDAA_SK_REVOCATION_LIST_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ecdaa_member_secret_key;

#include <stddef.h>

/*
 * Secret-key revocation list.
 *
 * `list` is an array of `member_join_secret_key`s.
 * `length` is the size of `list`.
 */
struct ecdaa_sk_revocation_list {
    size_t length;
    struct ecdaa_member_secret_key *list;
};

#ifdef __cplusplus
}
#endif

#endif
