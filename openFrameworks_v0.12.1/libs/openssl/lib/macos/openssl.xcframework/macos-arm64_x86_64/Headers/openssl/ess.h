/*
 * {- join("\n * ", @autowarntext) -}
 *
 * Copyright 2019-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */





#ifndef OPENSSL_ESS_H
# define OPENSSL_ESS_H
# pragma once

# include <openssl/opensslconf.h>

# include <openssl/safestack.h>
# include <openssl/x509.h>
# include <openssl/esserr.h>

# ifdef  __cplusplus
extern "C" {
# endif


typedef struct ESS_issuer_serial ESS_ISSUER_SERIAL;
typedef struct ESS_cert_id ESS_CERT_ID;
typedef struct ESS_signing_cert ESS_SIGNING_CERT;



SKM_DEFINE_STACK_OF_INTERNAL(ESS_CERT_ID, ESS_CERT_ID, ESS_CERT_ID)
#define sk_ESS_CERT_ID_num(sk) OPENSSL_sk_num(ossl_check_const_ESS_CERT_ID_sk_type(sk))
#define sk_ESS_CERT_ID_value(sk, idx) ((ESS_CERT_ID *)OPENSSL_sk_value(ossl_check_const_ESS_CERT_ID_sk_type(sk), (idx)))
#define sk_ESS_CERT_ID_new(cmp) ((STACK_OF(ESS_CERT_ID) *)OPENSSL_sk_new(ossl_check_ESS_CERT_ID_compfunc_type(cmp)))
#define sk_ESS_CERT_ID_new_null() ((STACK_OF(ESS_CERT_ID) *)OPENSSL_sk_new_null())
#define sk_ESS_CERT_ID_new_reserve(cmp, n) ((STACK_OF(ESS_CERT_ID) *)OPENSSL_sk_new_reserve(ossl_check_ESS_CERT_ID_compfunc_type(cmp), (n)))
#define sk_ESS_CERT_ID_reserve(sk, n) OPENSSL_sk_reserve(ossl_check_ESS_CERT_ID_sk_type(sk), (n))
#define sk_ESS_CERT_ID_free(sk) OPENSSL_sk_free(ossl_check_ESS_CERT_ID_sk_type(sk))
#define sk_ESS_CERT_ID_zero(sk) OPENSSL_sk_zero(ossl_check_ESS_CERT_ID_sk_type(sk))
#define sk_ESS_CERT_ID_delete(sk, i) ((ESS_CERT_ID *)OPENSSL_sk_delete(ossl_check_ESS_CERT_ID_sk_type(sk), (i)))
#define sk_ESS_CERT_ID_delete_ptr(sk, ptr) ((ESS_CERT_ID *)OPENSSL_sk_delete_ptr(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr)))
#define sk_ESS_CERT_ID_push(sk, ptr) OPENSSL_sk_push(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr))
#define sk_ESS_CERT_ID_unshift(sk, ptr) OPENSSL_sk_unshift(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr))
#define sk_ESS_CERT_ID_pop(sk) ((ESS_CERT_ID *)OPENSSL_sk_pop(ossl_check_ESS_CERT_ID_sk_type(sk)))
#define sk_ESS_CERT_ID_shift(sk) ((ESS_CERT_ID *)OPENSSL_sk_shift(ossl_check_ESS_CERT_ID_sk_type(sk)))
#define sk_ESS_CERT_ID_pop_free(sk, freefunc) OPENSSL_sk_pop_free(ossl_check_ESS_CERT_ID_sk_type(sk),ossl_check_ESS_CERT_ID_freefunc_type(freefunc))
#define sk_ESS_CERT_ID_insert(sk, ptr, idx) OPENSSL_sk_insert(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr), (idx))
#define sk_ESS_CERT_ID_set(sk, idx, ptr) ((ESS_CERT_ID *)OPENSSL_sk_set(ossl_check_ESS_CERT_ID_sk_type(sk), (idx), ossl_check_ESS_CERT_ID_type(ptr)))
#define sk_ESS_CERT_ID_find(sk, ptr) OPENSSL_sk_find(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr))
#define sk_ESS_CERT_ID_find_ex(sk, ptr) OPENSSL_sk_find_ex(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr))
#define sk_ESS_CERT_ID_find_all(sk, ptr, pnum) OPENSSL_sk_find_all(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_type(ptr), pnum)
#define sk_ESS_CERT_ID_sort(sk) OPENSSL_sk_sort(ossl_check_ESS_CERT_ID_sk_type(sk))
#define sk_ESS_CERT_ID_is_sorted(sk) OPENSSL_sk_is_sorted(ossl_check_const_ESS_CERT_ID_sk_type(sk))
#define sk_ESS_CERT_ID_dup(sk) ((STACK_OF(ESS_CERT_ID) *)OPENSSL_sk_dup(ossl_check_const_ESS_CERT_ID_sk_type(sk)))
#define sk_ESS_CERT_ID_deep_copy(sk, copyfunc, freefunc) ((STACK_OF(ESS_CERT_ID) *)OPENSSL_sk_deep_copy(ossl_check_const_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_copyfunc_type(copyfunc), ossl_check_ESS_CERT_ID_freefunc_type(freefunc)))
#define sk_ESS_CERT_ID_set_cmp_func(sk, cmp) ((sk_ESS_CERT_ID_compfunc)OPENSSL_sk_set_cmp_func(ossl_check_ESS_CERT_ID_sk_type(sk), ossl_check_ESS_CERT_ID_compfunc_type(cmp)))





typedef struct ESS_signing_cert_v2_st ESS_SIGNING_CERT_V2;
typedef struct ESS_cert_id_v2_st ESS_CERT_ID_V2;



SKM_DEFINE_STACK_OF_INTERNAL(ESS_CERT_ID_V2, ESS_CERT_ID_V2, ESS_CERT_ID_V2)
#define sk_ESS_CERT_ID_V2_num(sk) OPENSSL_sk_num(ossl_check_const_ESS_CERT_ID_V2_sk_type(sk))
#define sk_ESS_CERT_ID_V2_value(sk, idx) ((ESS_CERT_ID_V2 *)OPENSSL_sk_value(ossl_check_const_ESS_CERT_ID_V2_sk_type(sk), (idx)))
#define sk_ESS_CERT_ID_V2_new(cmp) ((STACK_OF(ESS_CERT_ID_V2) *)OPENSSL_sk_new(ossl_check_ESS_CERT_ID_V2_compfunc_type(cmp)))
#define sk_ESS_CERT_ID_V2_new_null() ((STACK_OF(ESS_CERT_ID_V2) *)OPENSSL_sk_new_null())
#define sk_ESS_CERT_ID_V2_new_reserve(cmp, n) ((STACK_OF(ESS_CERT_ID_V2) *)OPENSSL_sk_new_reserve(ossl_check_ESS_CERT_ID_V2_compfunc_type(cmp), (n)))
#define sk_ESS_CERT_ID_V2_reserve(sk, n) OPENSSL_sk_reserve(ossl_check_ESS_CERT_ID_V2_sk_type(sk), (n))
#define sk_ESS_CERT_ID_V2_free(sk) OPENSSL_sk_free(ossl_check_ESS_CERT_ID_V2_sk_type(sk))
#define sk_ESS_CERT_ID_V2_zero(sk) OPENSSL_sk_zero(ossl_check_ESS_CERT_ID_V2_sk_type(sk))
#define sk_ESS_CERT_ID_V2_delete(sk, i) ((ESS_CERT_ID_V2 *)OPENSSL_sk_delete(ossl_check_ESS_CERT_ID_V2_sk_type(sk), (i)))
#define sk_ESS_CERT_ID_V2_delete_ptr(sk, ptr) ((ESS_CERT_ID_V2 *)OPENSSL_sk_delete_ptr(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr)))
#define sk_ESS_CERT_ID_V2_push(sk, ptr) OPENSSL_sk_push(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr))
#define sk_ESS_CERT_ID_V2_unshift(sk, ptr) OPENSSL_sk_unshift(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr))
#define sk_ESS_CERT_ID_V2_pop(sk) ((ESS_CERT_ID_V2 *)OPENSSL_sk_pop(ossl_check_ESS_CERT_ID_V2_sk_type(sk)))
#define sk_ESS_CERT_ID_V2_shift(sk) ((ESS_CERT_ID_V2 *)OPENSSL_sk_shift(ossl_check_ESS_CERT_ID_V2_sk_type(sk)))
#define sk_ESS_CERT_ID_V2_pop_free(sk, freefunc) OPENSSL_sk_pop_free(ossl_check_ESS_CERT_ID_V2_sk_type(sk),ossl_check_ESS_CERT_ID_V2_freefunc_type(freefunc))
#define sk_ESS_CERT_ID_V2_insert(sk, ptr, idx) OPENSSL_sk_insert(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr), (idx))
#define sk_ESS_CERT_ID_V2_set(sk, idx, ptr) ((ESS_CERT_ID_V2 *)OPENSSL_sk_set(ossl_check_ESS_CERT_ID_V2_sk_type(sk), (idx), ossl_check_ESS_CERT_ID_V2_type(ptr)))
#define sk_ESS_CERT_ID_V2_find(sk, ptr) OPENSSL_sk_find(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr))
#define sk_ESS_CERT_ID_V2_find_ex(sk, ptr) OPENSSL_sk_find_ex(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr))
#define sk_ESS_CERT_ID_V2_find_all(sk, ptr, pnum) OPENSSL_sk_find_all(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_type(ptr), pnum)
#define sk_ESS_CERT_ID_V2_sort(sk) OPENSSL_sk_sort(ossl_check_ESS_CERT_ID_V2_sk_type(sk))
#define sk_ESS_CERT_ID_V2_is_sorted(sk) OPENSSL_sk_is_sorted(ossl_check_const_ESS_CERT_ID_V2_sk_type(sk))
#define sk_ESS_CERT_ID_V2_dup(sk) ((STACK_OF(ESS_CERT_ID_V2) *)OPENSSL_sk_dup(ossl_check_const_ESS_CERT_ID_V2_sk_type(sk)))
#define sk_ESS_CERT_ID_V2_deep_copy(sk, copyfunc, freefunc) ((STACK_OF(ESS_CERT_ID_V2) *)OPENSSL_sk_deep_copy(ossl_check_const_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_copyfunc_type(copyfunc), ossl_check_ESS_CERT_ID_V2_freefunc_type(freefunc)))
#define sk_ESS_CERT_ID_V2_set_cmp_func(sk, cmp) ((sk_ESS_CERT_ID_V2_compfunc)OPENSSL_sk_set_cmp_func(ossl_check_ESS_CERT_ID_V2_sk_type(sk), ossl_check_ESS_CERT_ID_V2_compfunc_type(cmp)))




DECLARE_ASN1_ALLOC_FUNCTIONS(ESS_ISSUER_SERIAL)
DECLARE_ASN1_ENCODE_FUNCTIONS_only(ESS_ISSUER_SERIAL, ESS_ISSUER_SERIAL)
DECLARE_ASN1_DUP_FUNCTION(ESS_ISSUER_SERIAL)

DECLARE_ASN1_ALLOC_FUNCTIONS(ESS_CERT_ID)
DECLARE_ASN1_ENCODE_FUNCTIONS_only(ESS_CERT_ID, ESS_CERT_ID)
DECLARE_ASN1_DUP_FUNCTION(ESS_CERT_ID)

DECLARE_ASN1_FUNCTIONS(ESS_SIGNING_CERT)
DECLARE_ASN1_DUP_FUNCTION(ESS_SIGNING_CERT)

DECLARE_ASN1_ALLOC_FUNCTIONS(ESS_CERT_ID_V2)
DECLARE_ASN1_ENCODE_FUNCTIONS_only(ESS_CERT_ID_V2, ESS_CERT_ID_V2)
DECLARE_ASN1_DUP_FUNCTION(ESS_CERT_ID_V2)

DECLARE_ASN1_FUNCTIONS(ESS_SIGNING_CERT_V2)
DECLARE_ASN1_DUP_FUNCTION(ESS_SIGNING_CERT_V2)

ESS_SIGNING_CERT *OSSL_ESS_signing_cert_new_init(const X509 *signcert,
                                                 const STACK_OF(X509) *certs,
                                                 int set_issuer_serial);
ESS_SIGNING_CERT_V2 *OSSL_ESS_signing_cert_v2_new_init(const EVP_MD *hash_alg,
                                                       const X509 *signcert,
                                                       const
                                                       STACK_OF(X509) *certs,
                                                       int set_issuer_serial);
int OSSL_ESS_check_signing_certs(const ESS_SIGNING_CERT *ss,
                                 const ESS_SIGNING_CERT_V2 *ssv2,
                                 const STACK_OF(X509) *chain,
                                 int require_signing_cert);

# ifdef  __cplusplus
}
# endif
#endif
