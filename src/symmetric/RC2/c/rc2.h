/*
 * Copyright (C) 2016 Alexander Scheel
 *
 * Implementation of the RC2 encryption algorithm. See docs for the
 * specification.
*/

#pragma once
#ifndef CC_RC2_H
#define CC_RC2_H

#include "stdlib.h"
#include "stdio.h"

const uint8_t rc2_initial_pitable[256] = {
    0xd9, 0x78, 0xf9, 0xc4, 0x19, 0xdd, 0xb5, 0xed, 0x28, 0xe9, 0xfd, 0x79, 0x4a, 0xa0, 0xd8, 0x9d, 0xc6, 0x7e, 0x37, 0x83, 0x2b, 0x76, 0x53, 0x8e, 0x62, 0x4c, 0x64, 0x88, 0x44, 0x8b, 0xfb, 0xa2, 0x17, 0x9a, 0x59, 0xf5, 0x87, 0xb3, 0x4f, 0x13, 0x61, 0x45, 0x6d, 0x8d, 0x09, 0x81, 0x7d, 0x32, 0xbd, 0x8f, 0x40, 0xeb, 0x86, 0xb7, 0x7b, 0x0b, 0xf0, 0x95, 0x21, 0x22, 0x5c, 0x6b, 0x4e, 0x82, 0x54, 0xd6, 0x65, 0x93, 0xce, 0x60, 0xb2, 0x1c, 0x73, 0x56, 0xc0, 0x14, 0xa7, 0x8c, 0xf1, 0xdc, 0x12, 0x75, 0xca, 0x1f, 0x3b, 0xbe, 0xe4, 0xd1, 0x42, 0x3d, 0xd4, 0x30, 0xa3, 0x3c, 0xb6, 0x26, 0x6f, 0xbf, 0x0e, 0xda, 0x46, 0x69, 0x07, 0x57, 0x27, 0xf2, 0x1d, 0x9b, 0xbc, 0x94, 0x43, 0x03, 0xf8, 0x11, 0xc7, 0xf6, 0x90, 0xef, 0x3e, 0xe7, 0x06, 0xc3, 0xd5, 0x2f, 0xc8, 0x66, 0x1e, 0xd7, 0x08, 0xe8, 0xea, 0xde, 0x80, 0x52, 0xee, 0xf7, 0x84, 0xaa, 0x72, 0xac, 0x35, 0x4d, 0x6a, 0x2a, 0x96, 0x1a, 0xd2, 0x71, 0x5a, 0x15, 0x49, 0x74, 0x4b, 0x9f, 0xd0, 0x5e, 0x04, 0x18, 0xa4, 0xec, 0xc2, 0xe0, 0x41, 0x6e, 0x0f, 0x51, 0xcb, 0xcc, 0x24, 0x91, 0xaf, 0x50, 0xa1, 0xf4, 0x70, 0x39, 0x99, 0x7c, 0x3a, 0x85, 0x23, 0xb8, 0xb4, 0x7a, 0xfc, 0x02, 0x36, 0x5b, 0x25, 0x55, 0x97, 0x31, 0x2d, 0x5d, 0xfa, 0x98, 0xe3, 0x8a, 0x92, 0xae, 0x05, 0xdf, 0x29, 0x10, 0x67, 0x6c, 0xba, 0xc9, 0xd3, 0x00, 0xe6, 0xcf, 0xe1, 0x9e, 0xa8, 0x2c, 0x63, 0x16, 0x01, 0x3f, 0x58, 0xe2, 0x89, 0xa9, 0x0d, 0x38, 0x34, 0x1b, 0xab, 0x33, 0xff, 0xb0, 0xbb, 0x48, 0x0c, 0x5f, 0xb9, 0xb1, 0xcd, 0x2e, 0xc5, 0xf3, 0xdb, 0x47, 0xe5, 0xa5, 0x9c, 0x77, 0x0a, 0xa6, 0x20, 0x68, 0xfe, 0x7f, 0xc1, 0xad
};

struct rc2 {
    union {
        uint16_t K[64];
        uint8_t L[128];
    } key;

    uint16_t R[4];
    uint16_t s[4];
    size_t j;
};

extern inline uint16_t rc2_rotl16(uint16_t data, uint16_t count)
{
    return ((data << count) | (data >> (16 - count)));
}

extern inline uint16_t rc2_rotr16(uint16_t data, uint16_t count)
{
    return ((data << (16 - count)) | (data >> count));
}

extern inline void rc2_init(struct rc2* r, uint8_t* key, size_t len,
                            size_t effective)
{
    size_t t = 0;
    size_t i = 0;
    size_t T8 = (effective + 7) / 8;
    size_t TM = 255 % (2 << (8 + effective - 8 * T8));

    for (t = 0; t < len; t++) {
        r->key.L[t] = key[t];
    }

    for (t = len; t < 128; t++) {
        r->key.L[t] = rc2_initial_pitable[(r->key.L[t - 1] + r->key.L[i]) % 256];
        i += 1;
    }

    r->key.L[128 - T8] = rc2_initial_pitable[r->key.L[128 - T8] & TM];

    for (t = 127 - T8; t > 0; t--) {
        r->key.L[t] = rc2_initial_pitable[r->key.L[t + 1] ^ r->key.L[t + T8]];
    }
    r->key.L[0] = rc2_initial_pitable[r->key.L[1] ^ r->key.L[T8]];


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t tmp;
    for (t = 0; t < 64; t++) {
        tmp = r->key.L[(t * 2) + 0];
        r->key.L[(t * 2) + 0] = r->key.L[(t * 2) + 1];
        r->key.L[(t * 2) + 1] = tmp;
    }
#endif
}

extern inline void rc2_mix(struct rc2* r, size_t i)
{
    r->R[i] = r->R[i] + r->key.K[r->j]
              + (r->R[((i - 1) + 4) % 4] & r->R[((i - 2) + 4) % 4])
              + ((~(r->R[((i - 1) + 4) % 4])) & r->R[((i - 3) + 4) % 4]);
    r->j = r->j + 1;

    r->R[i] = rc2_rotl16(r->R[i], r->s[i]);
}

extern inline void rc2_mix_round(struct rc2* r)
{
    rc2_mix(r, 0);
    rc2_mix(r, 1);
    rc2_mix(r, 2);
    rc2_mix(r, 3);
}

extern inline void rc2_mash(struct rc2* r, size_t i)
{
    r->R[i] = r->R[i] + r->key.K[r->R[((i - 1) + 4) % 4] & 63];
}

extern inline void rc2_mash_round(struct rc2* r)
{
    rc2_mash(r, 0);
    rc2_mash(r, 1);
    rc2_mash(r, 2);
    rc2_mash(r, 3);
}

extern inline void rc2_encrypt(struct rc2* r, uint16_t* data)
{
    r->R[0] = data[0];
    r->R[1] = data[1];
    r->R[2] = data[2];
    r->R[3] = data[3];

    r->s[0] = 1;
    r->s[1] = 2;
    r->s[2] = 3;
    r->s[3] = 5;

    r->j = 0;

    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);

    rc2_mash_round(r);

    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);

    rc2_mash_round(r);

    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
    rc2_mix_round(r);
}


extern inline void rc2_r_mix(struct rc2* r, size_t i)
{
    r->R[i] = rc2_rotr16(r->R[i], r->s[i]);
    r->R[i] = r->R[i] - r->key.K[r->j]
              - (r->R[((i - 1) + 4) % 4] & r->R[((i - 2) + 4) % 4])
              - ((~(r->R[((i - 1) + 4) % 4])) & r->R[((i - 3) + 4) % 4]);
    r->j = r->j - 1;

}

extern inline void rc2_r_mix_round(struct rc2* r)
{
    rc2_r_mix(r, 3);
    rc2_r_mix(r, 2);
    rc2_r_mix(r, 1);
    rc2_r_mix(r, 0);
}

extern inline void rc2_r_mash(struct rc2* r, size_t i)
{
    r->R[i] = r->R[i] - r->key.K[r->R[((i - 1) + 4) % 4] & 63];
}

extern inline void rc2_r_mash_round(struct rc2* r)
{
    rc2_r_mash(r, 3);
    rc2_r_mash(r, 2);
    rc2_r_mash(r, 1);
    rc2_r_mash(r, 0);
}

extern inline void rc2_decrypt(struct rc2* r, uint16_t* data)
{
    r->R[0] = data[0];
    r->R[1] = data[1];
    r->R[2] = data[2];
    r->R[3] = data[3];

    r->s[0] = 1;
    r->s[1] = 2;
    r->s[2] = 3;
    r->s[3] = 5;

    r->j = 63;

    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);

    rc2_r_mash_round(r);

    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);

    rc2_r_mash_round(r);

    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
    rc2_r_mix_round(r);
}


#endif
