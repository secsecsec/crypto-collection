/**
 * Copyright (C) 2016 Alexander Scheel
 *
 * Tests for the RC4 symmetric encryption algorithm.
**/

#include "rc4.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"

void test_rfc_6229()
{
    struct rc4 r;
    size_t t = 0;
    uint8_t key[2][32] = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    size_t keylen[2] = {5, 7};
    uint8_t keystream[32];
    uint8_t ciphertext[2][32] = {{0xb2, 0x39, 0x63, 0x05, 0xf0, 0x3d, 0xc0, 0x27, 0xcc, 0xc3, 0x52, 0x4a, 0x0a, 0x11, 0x18, 0xa8, 0x69, 0x82, 0x94, 0x4f, 0x18, 0xfc, 0x82, 0xd5, 0x89, 0xc4, 0x03, 0xa4, 0x7a, 0x0d, 0x09, 0x19}, {0x29, 0x3f, 0x02, 0xd4, 0x7f, 0x37, 0xc9, 0xb6, 0x33, 0xf2, 0xaf, 0x52, 0x85, 0xfe, 0xb4, 0x6b, 0xe6, 0x20, 0xf1, 0x39, 0x0d, 0x19, 0xbd, 0x84, 0xe2, 0xe0, 0xfd, 0x75, 0x20, 0x31, 0xaf, 0xc1}};

    for (t = 0; t < 2; t++) {
        printf("Test case: %zu\n", t);
        rc4_init(&r, key[t], keylen[t]);
        rc4_stream(&r, keystream, 32);

        printf("Actual:  \n");
        for (size_t i = 0; i < 32; i++) {
            printf("%02x ", keystream[i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");

        printf("Expected: \n");
        for (size_t i = 0; i < 32; i++) {
            printf("%02x ", ciphertext[t][i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n\n");
    }
}

int main()
{
    test_rfc_6229();
    return 0;
}
