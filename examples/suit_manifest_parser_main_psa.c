/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 * Copyright (c) 2021 Arm Ltd. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#if defined(LIBCSUIT_PSA_CRYPTO_C)

#include <stdio.h>
#include "qcbor/qcbor.h"
#include "suit_common.h"
#include "suit_manifest_data.h"
#include "suit_manifest_print.h"
#include "suit_cose.h"
#include "suit_examples_common.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"

#define MAX_FILE_BUFFER_SIZE            2048

/* ECC public key (P256r1) */
static const uint8_t public_key[] =
    {
    0x04, 0x77, 0x72, 0x65, 0x6f, 0x81, 0x4b, 0x39,
    0x92, 0x79, 0xd5, 0xe1, 0xf1, 0x78, 0x1f, 0xac,
    0x6f, 0x09, 0x9a, 0x3c, 0x5c, 0xa1, 0xb0, 0xe3,
    0x53, 0x51, 0x83, 0x4b, 0x08, 0xb6, 0x5e, 0x0b,
    0x57, 0x25, 0x90, 0xcd, 0xaf, 0x8f, 0x76, 0x93,
    0x61, 0xbc, 0xf3, 0x4a, 0xcf, 0xc1, 0x1e, 0x5e,
    0x07, 0x4e, 0x84, 0x26, 0xbd, 0xde, 0x04, 0xbe,
    0x6e, 0x65, 0x39, 0x45, 0x44, 0x96, 0x17, 0xde,
    0x45
    };

//static const size_t public_key_len = sizeof(public_key);

static const uint8_t manifest[] =
{0xa2, 0x02, 0x58, 0x98, 0x82, 0x58, 0x24, 0x82, 0x02,
 0x58, 0x20, 0x40, 0x42, 0x6b, 0x53, 0xe5, 0x63, 0x2f,
 0xd2, 0x5a, 0xce, 0xa2, 0x07, 0x0e, 0x1d, 0xcd, 0x93,
 0x2f, 0xb7, 0xbe, 0xd7, 0x82, 0x23, 0xcd, 0x29, 0xc6,
 0x69, 0x3b, 0x43, 0xa2, 0x77, 0x0b, 0x29, 0x58, 0x6f,
 0xd2, 0x84, 0x43, 0xa1, 0x01, 0x26, 0xa0, 0x58, 0x24,
 0x82, 0x02, 0x58, 0x20, 0x40, 0x42, 0x6b, 0x53, 0xe5,
 0x63, 0x2f, 0xd2, 0x5a, 0xce, 0xa2, 0x07, 0x0e, 0x1d,
 0xcd, 0x93, 0x2f, 0xb7, 0xbe, 0xd7, 0x82, 0x23, 0xcd,
 0x29, 0xc6, 0x69, 0x3b, 0x43, 0xa2, 0x77, 0x0b, 0x29,
 0x58, 0x40, 0x64, 0x50, 0x5f, 0xf5, 0xf1, 0x9d, 0x95,
 0x46, 0x0c, 0x56, 0xf3, 0x08, 0x29, 0x97, 0xf6, 0x69,
 0xfe, 0xde, 0x8f, 0x8a, 0x1e, 0x24, 0xa5, 0x70, 0xa5,
 0x6d, 0xd4, 0xaa, 0x66, 0xbb, 0xa6, 0xa6, 0x19, 0x63,
 0xa2, 0x05, 0x97, 0x04, 0x39, 0xdf, 0x4f, 0xff, 0x47,
 0xb2, 0xd1, 0xa4, 0x8b, 0x16, 0x30, 0xd5, 0x55, 0x87,
 0xd5, 0xe9, 0x0b, 0xa1, 0x2b, 0xce, 0x57, 0x25, 0x76,
 0x3e, 0x53, 0xed, 0x03, 0x58, 0x94, 0xa5, 0x01, 0x01,
 0x02, 0x02, 0x03, 0x58, 0x5f, 0xa2, 0x02, 0x81, 0x81,
 0x41, 0x00, 0x04, 0x58, 0x56, 0x86, 0x14, 0xa4, 0x01,
 0x50, 0xfa, 0x6b, 0x4a, 0x53, 0xd5, 0xad, 0x5f, 0xdf,
 0xbe, 0x9d, 0xe6, 0x63, 0xe4, 0xd4, 0x1f, 0xfe, 0x02,
 0x50, 0x14, 0x92, 0xaf, 0x14, 0x25, 0x69, 0x5e, 0x48,
 0xbf, 0x42, 0x9b, 0x2d, 0x51, 0xf2, 0xab, 0x45, 0x03,
 0x58, 0x24, 0x82, 0x02, 0x58, 0x20, 0x00, 0x11, 0x22,
 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x23, 0x45, 0x67, 0x89,
 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54,
 0x32, 0x10, 0x0e, 0x19, 0x87, 0xd0, 0x01, 0x0f, 0x02,
 0x0f, 0x09, 0x58, 0x25, 0x86, 0x13, 0xa1, 0x15, 0x78,
 0x1b, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x65,
 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f,
 0x6d, 0x2f, 0x66, 0x69, 0x6c, 0x65, 0x2e, 0x62, 0x69,
 0x6e, 0x15, 0x0f, 0x03, 0x0f, 0x0a, 0x43, 0x82, 0x03,
 0x0f};

static const size_t manifest_len = sizeof(manifest);

int main(int argc, char *argv[])
{
    int         result;
    uint8_t     mode = SUIT_DECODE_MODE_STRICT;

/* 
    // TBD: We should be allocating the key here and only pass handles around.

    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t     key_handle = 0;

    result = psa_crypto_init();

    if(result != PSA_SUCCESS)
        return( EXIT_FAILURE );

    psa_set_key_usage_flags( &key_attributes,
                             PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_EXPORT );
    psa_set_key_algorithm( &key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256) );
    psa_set_key_type( &key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1) );

    result = psa_import_key(&key_attributes,
                            public_key,
                            public_key_len,
                            &key_handle);

    if (result != PSA_SUCCESS)
        return( EXIT_FAILURE );

*/

#ifdef SKIP_ERROR
    mode = SUIT_DECODE_MODE_SKIP_ANY_ERROR;
#endif

    suit_envelope_t envelope = (suit_envelope_t){ 0 };
    suit_buf_t buf = {.ptr = manifest, .len = manifest_len};

    struct t_cose_key cose_public_key;
    result = suit_create_es256_public_key((char *)public_key, &cose_public_key);

    result = suit_set_envelope(mode, &buf, &envelope, &cose_public_key);

    if (result != SUIT_SUCCESS) {
        printf("Can't parse Manifest.\n");
        return EXIT_FAILURE;
    }

    printf("\nPrint Manifest.\n\n");
    result = suit_print_envelope(mode, &envelope, 2);
    if (result != SUIT_SUCCESS) {
        printf("Can't print manifest.\n");
        return EXIT_FAILURE;
    }
    


    return EXIT_SUCCESS;
}

#endif /* LIBCSUIT_PSA_CRYPTO_C */
