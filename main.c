
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"

int main() {
    const unsigned char *input = "EUNJUNG";
    size_t input_length = strlen(input);

    unsigned char *base64_out = encode64((const unsigned char *)input, input_length);
    if (base64_out == NULL) {
        fprintf(stderr, "Encode fail\n");
        return 1;
    }

    printf("Oi: %s\n", base64_out);
    free(base64_out);

    return 0;
}