/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/*!
    \file   suit_manifest_process_main.c

    \brief  A sample to use libcsuit processing
 */

#include <stdio.h>
#include <stdlib.h>
#include "qcbor/qcbor.h"
#include "csuit/suit_manifest_process.h"
#include "csuit/suit_manifest_print.h"
#include "suit_examples_common.h"
#include "trust_anchor_prime256v1_pub.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"

#define MAX_FILE_BUFFER_SIZE            2048

#define NUM_PUBLIC_KEYS                 1
/* TC signer's public_key */
const uint8_t *public_keys[NUM_PUBLIC_KEYS] = {
    trust_anchor_prime256v1_public_key,
};

const uint8_t tc_uri[] = {
    0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x65, 0x78,
    0x61, 0x6D, 0x70, 0x6C, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x2F,
    0x38, 0x64, 0x38, 0x32, 0x35, 0x37, 0x33, 0x61, 0x2D, 0x39,
    0x32, 0x36, 0x64, 0x2D, 0x34, 0x37, 0x35, 0x34, 0x2D, 0x39,
    0x33, 0x35, 0x33, 0x2D, 0x33, 0x32, 0x64, 0x63, 0x32, 0x39,
    0x39, 0x39, 0x37, 0x66, 0x37, 0x34, 0x2E, 0x74, 0x61
}; // "https://example.org/8d82573a-926d-4754-9353-32dc29997f74.ta";
const uint8_t tc_data[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x53, 0x65, 0x63,
    0x75, 0x72, 0x65, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21
}; // "Hello, Secure World!"
const uint8_t depend_uri[] = {
    0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x65, 0x78,
    0x61, 0x6D, 0x70, 0x6C, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x2F,
    0x38, 0x64, 0x38, 0x32, 0x35, 0x37, 0x33, 0x61, 0x2D, 0x39,
    0x32, 0x36, 0x64, 0x2D, 0x34, 0x37, 0x35, 0x34, 0x2D, 0x39,
    0x33, 0x35, 0x33, 0x2D, 0x33, 0x32, 0x64, 0x63, 0x32, 0x39,
    0x39, 0x39, 0x37, 0x66, 0x37, 0x34, 0x2E, 0x73, 0x75, 0x69,
    0x74
}; // "https://example.org/8d82573a-926d-4754-9353-32dc29997f74.suit";
const uint8_t depend_suit[] = {
    0xD8, 0x6B, 0xA2, 0x02, 0x58, 0x73, 0x82, 0x58, 0x24, 0x82,
    0x2F, 0x58, 0x20, 0xDB, 0x60, 0x1A, 0xDE, 0x73, 0x09, 0x2B,
    0x58, 0x53, 0x2C, 0xA0, 0x3F, 0xBB, 0x66, 0x3D, 0xE4, 0x95,
    0x32, 0x43, 0x53, 0x36, 0xF1, 0x55, 0x8B, 0x49, 0xBB, 0x62,
    0x27, 0x26, 0xA2, 0xFE, 0xDD, 0x58, 0x4A, 0xD2, 0x84, 0x43,
    0xA1, 0x01, 0x26, 0xA0, 0xF6, 0x58, 0x40, 0x7A, 0xF7, 0x99,
    0x3A, 0x77, 0x22, 0xBA, 0x75, 0x6F, 0x05, 0xBB, 0xA3, 0x01,
    0x4F, 0x0C, 0xC4, 0x47, 0x9D, 0x7F, 0x65, 0x6D, 0x18, 0xFD,
    0xB6, 0xAD, 0xF8, 0xFD, 0x6E, 0x2A, 0x75, 0x60, 0x0A, 0x69,
    0x1D, 0x98, 0x45, 0xBC, 0x91, 0x55, 0x23, 0x60, 0xA1, 0xD3,
    0x51, 0x6F, 0x50, 0x3A, 0x06, 0x67, 0x1F, 0x3D, 0x46, 0x69,
    0xB0, 0x59, 0x12, 0x46, 0xFD, 0x45, 0xE9, 0xC1, 0x76, 0x9C,
    0x2B, 0x03, 0x58, 0xD4, 0xA4, 0x01, 0x01, 0x02, 0x03, 0x03,
    0x58, 0x84, 0xA2, 0x02, 0x81, 0x84, 0x4B, 0x54, 0x45, 0x45,
    0x50, 0x2D, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x48, 0x53,
    0x65, 0x63, 0x75, 0x72, 0x65, 0x46, 0x53, 0x50, 0x8D, 0x82,
    0x57, 0x3A, 0x92, 0x6D, 0x47, 0x54, 0x93, 0x53, 0x32, 0xDC,
    0x29, 0x99, 0x7F, 0x74, 0x42, 0x74, 0x61, 0x04, 0x58, 0x54,
    0x86, 0x14, 0xA4, 0x01, 0x50, 0xC0, 0xDD, 0xD5, 0xF1, 0x52,
    0x43, 0x56, 0x60, 0x87, 0xDB, 0x4F, 0x5B, 0x0A, 0xA2, 0x6C,
    0x2F, 0x02, 0x50, 0xDB, 0x42, 0xF7, 0x09, 0x3D, 0x8C, 0x55,
    0xBA, 0xA8, 0xC5, 0x26, 0x5F, 0xC5, 0x82, 0x0F, 0x4E, 0x03,
    0x58, 0x24, 0x82, 0x2F, 0x58, 0x20, 0x8C, 0xF7, 0x1A, 0xC8,
    0x6A, 0xF3, 0x1B, 0xE1, 0x84, 0xEC, 0x7A, 0x05, 0xA4, 0x11,
    0xA8, 0xC3, 0xA1, 0x4F, 0xD9, 0xB7, 0x7A, 0x30, 0xD0, 0x46,
    0x39, 0x74, 0x81, 0x46, 0x94, 0x68, 0xEC, 0xE8, 0x0E, 0x14,
    0x01, 0x0F, 0x02, 0x0F, 0x09, 0x58, 0x45, 0x86, 0x14, 0xA1,
    0x15, 0x78, 0x3B, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F,
    0x2F, 0x65, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x2E, 0x6F,
    0x72, 0x67, 0x2F, 0x38, 0x64, 0x38, 0x32, 0x35, 0x37, 0x33,
    0x61, 0x2D, 0x39, 0x32, 0x36, 0x64, 0x2D, 0x34, 0x37, 0x35,
    0x34, 0x2D, 0x39, 0x33, 0x35, 0x33, 0x2D, 0x33, 0x32, 0x64,
    0x63, 0x32, 0x39, 0x39, 0x39, 0x37, 0x66, 0x37, 0x34, 0x2E,
    0x74, 0x61, 0x15, 0x0F, 0x03, 0x0F
}; // suit_manifest_expU.md
const uint8_t config_uri[] = {
    0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x65, 0x78,
    0x61, 0x6D, 0x70, 0x6C, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x2F,
    0x63, 0x6F, 0x6E, 0x66, 0x69, 0x67, 0x2E, 0x6A, 0x73, 0x6F,
    0x6E
}; // "https://example.org/config.json";
const uint8_t config_data[] = {
    0x7B, 0x22, 0x6E, 0x61, 0x6D, 0x65, 0x22, 0x3A, 0x20, 0x22,
    0x46, 0x4F, 0x4F, 0x20, 0x42, 0x61, 0x72, 0x22, 0x2C, 0x20,
    0x22, 0x74, 0x6F, 0x6B, 0x65, 0x6E, 0x22, 0x3A, 0x20, 0x22,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x61, 0x62, 0x66, 0x63, 0x64, 0x65, 0x66, 0x30, 0x31, 0x32,
    0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63,
    0x64, 0x65, 0x22, 0x7D
};// "{\"name\": \"FOO Bar\", \"token\": \"0123456789abfcdef0123456789abcde\"}";

struct name_data {
    const uint8_t *name;
    size_t name_len;
    const uint8_t *data;
    size_t data_len;
};
#define SUIT_NAME_DATA_LEN 3
const struct name_data name_data[] = {
    {.name = tc_uri, .name_len = sizeof(tc_uri), .data = tc_data, .data_len = sizeof(tc_data)},
    {.name = depend_uri, .name_len = sizeof(depend_uri), .data = depend_suit, .data_len = sizeof(depend_suit)},
    {.name = config_uri, .name_len = sizeof(config_uri), .data = config_data, .data_len = sizeof(config_data)},
};

suit_err_t __real_suit_fetch_callback(suit_fetch_args_t fetch_args, suit_fetch_ret_t *fetch_ret);
suit_err_t __wrap_suit_fetch_callback(suit_fetch_args_t fetch_args, suit_fetch_ret_t *fetch_ret)
{
    suit_err_t result = __real_suit_fetch_callback(fetch_args, fetch_ret);
    if (result != SUIT_SUCCESS) {
        return result;
    }

    size_t i = 0;
    for (i = 0; i < SUIT_NAME_DATA_LEN; i++) {
        if (name_data[i].name_len == fetch_args.uri_len &&
            memcmp(name_data[i].name, fetch_args.uri, fetch_args.uri_len) == 0) {
            if (fetch_args.buf_len < name_data[i].data_len) {
                return SUIT_ERR_NO_MEMORY;
            }
            memcpy(fetch_args.ptr, name_data[i].data, name_data[i].data_len);
            fetch_ret->buf_len = name_data[i].data_len;
            printf("fetched %s\n\n", name_data[i].name);
            break;
        }
    }
    if (i == SUIT_NAME_DATA_LEN) {
        /* not found */
        /* ignore this for testing example 0-5 only */
        //return SUIT_ERR_NOT_FOUND;
        fetch_ret->buf_len = fetch_args.buf_len;
    }
    return SUIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    // check arguments.
    if (argc < 2) {
        printf("%s <manifest file path>", argv[0]);
        return EXIT_FAILURE;
    }
    int32_t result = 0;
    int i;
    suit_key_t cose_keys[NUM_PUBLIC_KEYS];

    suit_inputs_t suit_inputs = {0};
    suit_inputs.left_len = SUIT_MAX_DATA_SIZE;
    suit_inputs.ptr = suit_inputs.buf;

    suit_inputs.key_len = NUM_PUBLIC_KEYS;
    suit_inputs.public_keys = cose_keys;

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read public key from DER file.\n");
    for (i = 0; i < NUM_PUBLIC_KEYS; i++) {
        result = suit_key_init_es256_public_key(public_keys[i], &suit_inputs.public_keys[i]);
    }
    // Read manifest file.
    printf("\nmain : Read Manifest file.\n");
    suit_inputs.manifest.len = read_from_file(argv[1], MAX_FILE_BUFFER_SIZE, suit_inputs.buf);
    if (suit_inputs.manifest.len <= 0) {
        printf("main : Can't read Manifest file.\n");
        return EXIT_FAILURE;
    }
    suit_inputs.manifest.ptr = suit_inputs.buf;
    suit_inputs.left_len -= suit_inputs.manifest.len;

    // Decode manifest file.
    printf("\nmain : Decode Manifest file.\n");
    result = suit_process_envelope(&suit_inputs);
    if (result != SUIT_SUCCESS) {
        printf("main : Can't parse Manifest file. err=%d\n", result);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
