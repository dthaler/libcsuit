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
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"

#define MAX_FILE_BUFFER_SIZE            2048

#define NUM_PUBLIC_KEYS                 2
/* TC signer's public_key */
const uint8_t der_public_keys[NUM_PUBLIC_KEYS][PRIME256V1_PUBLIC_KEY_DER_SIZE] = {
    { /* TC signer's public key */
        0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
        0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
        0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x84, 0x96, 0x81,
        0x1a, 0xae, 0x0b, 0xaa, 0xab, 0xd2, 0x61, 0x57, 0x18, 0x9e,
        0xec, 0xda, 0x26, 0xbe, 0xaa, 0x8b, 0xf1, 0x1b, 0x6f, 0x3f,
        0xe6, 0xe2, 0xb5, 0x65, 0x9c, 0x85, 0xdb, 0xc0, 0xad, 0x3b,
        0x1f, 0x2a, 0x4b, 0x6c, 0x09, 0x81, 0x31, 0xc0, 0xa3, 0x6d,
        0xac, 0xd1, 0xd7, 0x8b, 0xd3, 0x81, 0xdc, 0xdf, 0xb0, 0x9c,
        0x05, 0x2d, 0xb3, 0x39, 0x91, 0xdb, 0x73, 0x38, 0xb4, 0xa8,
        0x96
    },/* TAM's public key */
    {
        0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
        0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
        0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x4d, 0x5e, 0x5f,
        0x33, 0x67, 0xec, 0x6e, 0x41, 0x1f, 0x0e, 0xc3, 0x97, 0x45,
        0x2a, 0xc0, 0x2e, 0x65, 0x41, 0xb2, 0x12, 0x76, 0x13, 0x14,
        0x54, 0x8a, 0x62, 0x93, 0x79, 0x26, 0x4c, 0x5a, 0x44, 0x30,
        0x8a, 0xef, 0xfc, 0x28, 0x5e, 0x45, 0x2e, 0xde, 0x34, 0x3c,
        0x0f, 0x35, 0xd2, 0x1e, 0x0e, 0x2d, 0x37, 0x51, 0xf8, 0xbd,
        0x32, 0x49, 0x6f, 0x90, 0xaf, 0x26, 0x4d, 0x68, 0x6e, 0xcd,
        0xed
    }
};

size_t read_file(const char *file_path, const size_t write_buf_len, uint8_t *write_buf) {
    size_t read_len = 0;
    FILE* fp = fopen(file_path, "rb");
    if (fp == NULL) {
        return 0;
    }
    read_len = fread(write_buf, 1, write_buf_len, fp);
    fclose(fp);
    return read_len;
}


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
};
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
    0x2F, 0x58, 0x20, 0x8A, 0xDC, 0x99, 0x55, 0x73, 0x63, 0x16,
    0x39, 0xC3, 0xC6, 0xD5, 0xFC, 0x40, 0x26, 0x16, 0x0C, 0x8A,
    0x32, 0xC5, 0xAA, 0xDF, 0xBE, 0xEC, 0x9F, 0xA4, 0x9E, 0x02,
    0x6F, 0xDD, 0x74, 0xCA, 0xB3, 0x58, 0x4A, 0xD2, 0x84, 0x43,
    0xA1, 0x01, 0x26, 0xA0, 0xF6, 0x58, 0x40, 0xC7, 0xC2, 0xE2,
    0x04, 0x57, 0xBC, 0x28, 0x23, 0x9D, 0x63, 0x00, 0x71, 0x04,
    0xDE, 0x67, 0x6A, 0xD4, 0x86, 0x40, 0x92, 0x95, 0x99, 0x44,
    0x80, 0xF9, 0xF1, 0xE4, 0xB1, 0xD5, 0x11, 0x72, 0x2F, 0xF1,
    0xF2, 0x9C, 0xE3, 0x34, 0x5F, 0xF9, 0x77, 0x78, 0xB8, 0x1D,
    0xA2, 0xED, 0x88, 0x92, 0xAC, 0xDA, 0x52, 0x1B, 0x9E, 0x3D,
    0x3F, 0x61, 0x17, 0x72, 0x54, 0x74, 0x9D, 0x72, 0x79, 0xB9,
    0x1B, 0x03, 0x59, 0x01, 0x1D, 0xA5, 0x01, 0x01, 0x02, 0x03,
    0x03, 0x58, 0x84, 0xA2, 0x02, 0x81, 0x84, 0x4B, 0x54, 0x45,
    0x45, 0x50, 0x2D, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x48,
    0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x46, 0x53, 0x50, 0x8D,
    0x82, 0x57, 0x3A, 0x92, 0x6D, 0x47, 0x54, 0x93, 0x53, 0x32,
    0xDC, 0x29, 0x99, 0x7F, 0x74, 0x42, 0x74, 0x61, 0x04, 0x58,
    0x54, 0x86, 0x13, 0xA4, 0x01, 0x50, 0xC0, 0xDD, 0xD5, 0xF1,
    0x52, 0x43, 0x56, 0x60, 0x87, 0xDB, 0x4F, 0x5B, 0x0A, 0xA2,
    0x6C, 0x2F, 0x02, 0x50, 0xDB, 0x42, 0xF7, 0x09, 0x3D, 0x8C,
    0x55, 0xBA, 0xA8, 0xC5, 0x26, 0x5F, 0xC5, 0x82, 0x0F, 0x4E,
    0x03, 0x58, 0x24, 0x82, 0x2F, 0x58, 0x20, 0x8C, 0xF7, 0x1A,
    0xC8, 0x6A, 0xF3, 0x1B, 0xE1, 0x84, 0xEC, 0x7A, 0x05, 0xA4,
    0x11, 0xA8, 0xC3, 0xA1, 0x4F, 0xD9, 0xB7, 0x7A, 0x30, 0xD0,
    0x46, 0x39, 0x74, 0x81, 0x46, 0x94, 0x68, 0xEC, 0xE8, 0x0E,
    0x14, 0x01, 0x0F, 0x02, 0x0F, 0x09, 0x58, 0x40, 0x86, 0x13,
    0xA1, 0x15, 0x78, 0x36, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A,
    0x2F, 0x2F, 0x74, 0x63, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x38,
    0x64, 0x38, 0x32, 0x35, 0x37, 0x33, 0x61, 0x2D, 0x39, 0x32,
    0x36, 0x64, 0x2D, 0x34, 0x37, 0x35, 0x34, 0x2D, 0x39, 0x33,
    0x35, 0x33, 0x2D, 0x33, 0x32, 0x64, 0x63, 0x32, 0x39, 0x39,
    0x39, 0x37, 0x66, 0x37, 0x34, 0x2E, 0x74, 0x61, 0x15, 0x0F,
    0x03, 0x0F, 0x0D, 0x58, 0x4B, 0xA1, 0x84, 0x4B, 0x54, 0x45,
    0x45, 0x50, 0x2D, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x48,
    0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x46, 0x53, 0x50, 0x8D,
    0x82, 0x57, 0x3A, 0x92, 0x6D, 0x47, 0x54, 0x93, 0x53, 0x32,
    0xDC, 0x29, 0x99, 0x7F, 0x74, 0x42, 0x74, 0x61, 0xA2, 0x02,
    0x75, 0x52, 0x65, 0x66, 0x65, 0x72, 0x65, 0x6E, 0x63, 0x65,
    0x20, 0x54, 0x45, 0x45, 0x50, 0x2D, 0x44, 0x65, 0x76, 0x69,
    0x63, 0x65, 0x03, 0x66, 0x74, 0x63, 0x2E, 0x6F, 0x72, 0x67
};
const uint8_t config_uri[] = {
    0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x74, 0x63,
    0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x63, 0x6F, 0x6E, 0x66, 0x69,
    0x67, 0x2E, 0x6A, 0x73, 0x6F, 0x6E
}; // "https://tc.org/config.json";
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
    const uint8_t *data;
    const size_t data_len;
};
#define SUIT_NAME_DATA_LEN 3
const struct name_data name_data[] = {
    {.name = tc_uri, .data = tc_data, .data_len = sizeof(tc_data)},
    {.name = depend_uri, .data = depend_suit, .data_len = sizeof(depend_suit)},
    {.name = config_uri, .data = config_data, .data_len = sizeof(config_data)},
};

suit_err_t __real_suit_fetch_callback(suit_fetch_args_t fetch_args);
suit_err_t __wrap_suit_fetch_callback(suit_fetch_args_t fetch_args)
{
    suit_err_t result = __real_suit_fetch_callback(fetch_args);
    if (result != SUIT_SUCCESS) {
        return result;
    }

    size_t i = 0;
    for (i = 0; i < SUIT_NAME_DATA_LEN; i++) {
        if (memcmp(name_data[i].name, fetch_args.uri, fetch_args.uri_len) == 0) {
            if (*fetch_args.buf_len < name_data[i].data_len) {
                return SUIT_ERR_NO_MEMORY;
            }
            memcpy(fetch_args.ptr, name_data[i].data, name_data[i].data_len);
            *fetch_args.buf_len = name_data[i].data_len;
            printf("fetched %s\n\n", name_data[i].name);
            break;
        }
    }
    if (i == SUIT_NAME_DATA_LEN) {
        /* not found */
        /* ignore this for testing example 0-5 only */
        //return SUIT_ERR_NOT_FOUND;
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
    char char_public_keys[NUM_PUBLIC_KEYS][PRIME256V1_PUBLIC_KEY_CHAR_SIZE + 1];
    struct t_cose_key public_keys[NUM_PUBLIC_KEYS];

    suit_inputs_t suit_inputs = {0};
    suit_inputs.left_len = SUIT_MAX_DATA_SIZE;

    suit_inputs.key_len = NUM_PUBLIC_KEYS;
    suit_inputs.public_keys = public_keys;

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read public key from DER file.\n");
    for (i = 0; i < NUM_PUBLIC_KEYS; i++) {
        read_prime256v1_public_key(der_public_keys[i], char_public_keys[i]);
        printf("%s\n", char_public_keys[i]);
        result = suit_create_es256_public_key(char_public_keys[i], &suit_inputs.public_keys[i]);
    }
    // Read manifest file.
    printf("\nmain : Read Manifest file.\n");
    suit_inputs.manifest.len = read_file(argv[1], MAX_FILE_BUFFER_SIZE, suit_inputs.buf);
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
