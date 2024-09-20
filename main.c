
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"

int main()
{

    ///////encode
    const unsigned char *input = "EUNJUNG";

    printf("Oi: %s\n", input);

    unsigned char *base64encode_out = encode_message((const unsigned char *)input);
    if (base64encode_out == NULL)
    {
        fprintf(stderr, "Encode fail\n");
        return 1;
    }

    printf("Oi: %s\n", base64encode_out);



    //////////decode
    unsigned char *base64decode_out = decode_message((const unsigned char *)base64encode_out);
    if (base64decode_out == NULL)
    {
        fprintf(stderr, "Decode fail\n");
        return 1;
    }

    printf("Oi: %s\n", base64decode_out);




    /////////encode image

    const char *filename_tobecoded = "eunjung.jpg";  // path to the image file to be encoded
    const char *filename_tobedecoded = "eunjung.txt";  // path to the text file to be decoded

    // Read the image file into a byte array
    encode_image(filename_tobecoded);

    decode_image(filename_tobedecoded);

    unsigned char *decoded_image_data = decode_image("encoded_image.txt");  // Decoded binary image data
    size_t image_data_length = strlen((const char *)decoded_image_data);    // Get the length of the decoded data

    // build the image from the decoded binary data
    build_image(decoded_image_data, image_data_length, "output_image");


    free(base64decode_out);

    free(base64encode_out);

    clear_decoding_table();

    return 0;
}