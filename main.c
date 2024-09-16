
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"

int main()
{

    ///////encode
    const unsigned char *input = "EUNJUNG";

    printf("Oi: %s\n", input);

    unsigned char *base64encode_out = encode64((const unsigned char *)input);
    if (base64encode_out == NULL)
    {
        fprintf(stderr, "Encode fail\n");
        return 1;
    }

    printf("Oi: %s\n", base64encode_out);



    //////////decode
    unsigned char *base64decode_out = decode64((const unsigned char *)base64encode_out);
    if (base64decode_out == NULL)
    {
        fprintf(stderr, "Decode fail\n");
        return 1;
    }

    printf("Oi: %s\n", base64decode_out);




    /////////encode image

    const char *filename = "eunjung.jpg";  // Path to the image file

    // Read the image file into a byte array
    unsigned char *image_data = encode_image(filename);
    
    if (image_data == NULL)
    {
        fprintf(stderr, "Failed to read image file\n");
        return 1;
    }

    printf("Oi: %s\n", image_data);

    free(base64decode_out);

    free(base64encode_out);

    clear_decoding_table();

    return 0;
}