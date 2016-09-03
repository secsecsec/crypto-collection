/**
 * Copyright (C) 2016 Alexander Scheel
 *
 * Tests for the des symmetric encryption algorithm.
**/

#include "des.h"
#include "stdio.h"

int main()
{
    struct des d;
    uint64_t key = 0x8000000000000000ull;
    uint64_t plain = 0x0000000000000000ull;
    uint64_t expected = 0x95A8D72813DAA94Dull;

    des_init(&d, key);

    printf("Key:       %16llx\n", key);
    printf("Plaintext: %16llx\n", plain);
    printf("Actual:    %16llx\n", des_encrypt_block(&d, plain));
    printf("Expected:  %16llx\n", expected);
    printf("Decrypted: %16llx\n", des_decrypt_block(&d, expected));

    return 0;
}
