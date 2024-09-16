//message to binary
//divide binary into 6 bit chunks
//convert each chunk to decimal(which is ascii)
//use decimal as index to look up character in base64 table
//append each chunk character to output string
//if number of bits in binary data is not a multiple of 3, pad with = sign
//handle data 3bytes at a time, 24 is EKOK of 6 and 8
//build decoding table when needed in order to save some ROM on the controller


//todo;
//catching nulls
//back propogation
//dedicated read image function
//fix image encoder
//try if image functions work with sound and other files as well, rename the functions accordingly

//NOTE: //image encoder works half the way, for some reason it adds unwanted paddings when compared to correct output

#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stdlib.h>


unsigned char *encode64(const unsigned char *input);
unsigned char *build_decodding_table();
unsigned char *decode64(const unsigned char *input);
void clear_decoding_table();
unsigned char *encode_image(const char *filepath);

//array width 65 maybe?
static unsigned char base64_table[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

static unsigned char *decoding_table = NULL;
static int mod[4] = {0, 2, 1}; //for padding 



unsigned char *encode64(const unsigned char *input){



    size_t input_length = strlen(input);
    unsigned char encoded_length = 4*(input_length + 2)/3; //create 4 byte chunk of output

    if (input_length>encoded_length) {
        return NULL; //input overflow, try catching this in future
    }

    unsigned char *encoded_out = malloc(encoded_length+1);
    if (encoded_out == NULL) {
        return NULL; //try catching this
    }

    //poutition indices
    unsigned char *pout=encoded_out;
    const unsigned char *pin= input;

    //             pin[0]                           pin[1]                          pin[2]
    // input b7 b6 b5 b4 b3 b2 b1 b0 - b15 b14 b13 b12 b11 b10 b9 b8 - b23 b22 b21 b20 b19 b18 b17 b16  
    //basic principle here is isolating bits through and operations by two and four(in hex 0x03=3 and 0x0f=16) and shifting them to create 6 bit chunks 
    //storing a new character in *pout every line

    uint32_t i=0;
    while(input_length>=3){     // three bytes at a time
        *pout++= base64_table[pin[i] >> 2]; // shift right 2 bits 0 0 b7 b6 b5 b4 b3 b2   
        *pout++= base64_table[((pin[i] & 0x03) << 4) | (pin[i+1] >> 4)]; //  b1 b0 0 0 0 0 0 0 | 0 0 0 0 b15 b14 b13 b12 == b1 b0 b15 b14 b13 b12 0 0
        *pout++= base64_table[((pin[i+1] & 0x0f) << 2) | (pin[i+2] >> 6)]; //  b11 b10 b9 b8 0 0 0 0 | 0 0 0 0 0 0 b23 b22 == b11 b10 b9 b8 b23 b22 0 0
        *pout++=base64_table[pin[2] & 0x03f]; // b21 b20 b19 b18 b17 b16 0 0
        i+=3;
        input_length-=3;
    }

    if(input_length!=0){
        *pout++=base64_table[pin[i]>>2];
        if(input_length==1){
            *pout++=base64_table[(pin[i] & 0x03) << 4];
            *pout++='=';
        }else{
            *pout++=base64_table[((pin[i] & 0x03) << 4) | (pin[i+1] >> 4)];
            *pout++=base64_table[(pin[i+1] & 0x0f) << 2];
        }
        *pout++='=';
    }
    *pout = '\0';
    return encoded_out;
}

unsigned char *decode64(const unsigned char *input){

    size_t input_length = strlen(input);

    if (decoding_table == NULL) {
        decoding_table=build_decodding_table(); //try catching the null
    }

    if (input_length % 4 != 0) {
        return NULL; //try catching this
    }

    unsigned char decoded_length = (input_length/4)*3;

    //remove paddings
    if (input[input_length - 1] == '='){
    (decoded_length)--;
    }
    if (input[input_length - 2] == '='){
    (decoded_length)--;
    }

    unsigned char *decoded_out = malloc(decoded_length+1);
    if(decoded_out == NULL){
        return NULL; //try catching this
    }


    const unsigned char *pin= input;
    unsigned char *pout=decoded_out;

    //             pin[0]                           pin[1]                          pin[2]
    // input b7 b6 b5 b4 b3 b2 b1 b0 - b15 b14 b13 b12 b11 b10 b9 b8 - b23 b22 b21 b20 b19 b18 b17 b16  

    //              pin[0]                    pin[1]                       pin[2]                      pin[3]
    // input 0 0 b7 b6 b5 b4 b3 b2 - b1 b0 b15 b14 b13 b12 0 0 - b11 b10 b9 b8 b23 b22 0 0 -b21 b20 b19 b18 b17 b16 0 0

    //incomplete
    uint32_t i=0;
    //while (input_length >= 4) ???
    while (input_length!=0){ 
    
        *pout++ = (decoding_table[pin[0]] << 2) | (decoding_table[pin[1]] >> 4);
            if (pin[2] != '=') {
                *pout++ = ((decoding_table[pin[1]] & 0x0F) << 4) | (decoding_table[pin[2]] >> 2);
            }
            if (pin[3] != '=') {
                *pout++ = ((decoding_table[pin[2]] & 0x03) << 6) | decoding_table[pin[3]];
            }

        // Move to the next 4 input characters
        pin += 4;
        input_length -= 4;
    }


    *pout = '\0';
    return decoded_out;
}




unsigned char *build_decodding_table(){

    decoding_table = malloc(256); 
    if (decoding_table == NULL) {
        return NULL; // try catching this
    }

    //set all values to -1 to exlclude invalid characters
    //weird enough, decoding also workds without this, tho when I print the decoding table
    //it shows weird(somehow invalid kind of) symbols mapped, when the table should be basically inverse of base64 table
    //I will leave this here for now
    for (int i = 0; i < 256; i++) {
        decoding_table[i] = -1;  
    }

    for (uint32_t i = 0; i < 64; i++) {
        decoding_table[(unsigned char)base64_table[i]] = i;
    }

    return decoding_table;
}

//filepath or filename(provided theyre in the same directory), redo in my convinience
//this function encodes image in 4 byte chunks for better performance with arm and risc-v microcontrollers and lora transceivers
//in future, try transmitting the every 4 byte chunk in a packet  
unsigned char *encode_image(const char *filepath){

    FILE *file = fopen(filepath, "rb");  // open the file in binary mode

    if (file == NULL) {
        return NULL; //try catching this
    }

    //get the size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);  
    fseek(file, 0, SEEK_SET);  // reset thepointer to the beginning


    unsigned char buffer[4];  
    unsigned char *image_output = (unsigned char *)malloc( (4 * ((file_size + 2) / 3)) + 1);  // +1 for null terminator
    if (image_output == NULL) {
        fclose(file);
        return NULL;  //try catching this
    }

    //image_output[0] = '\0';  // init final as empty string
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, 3, file)) > 0) {
        // null-terminate the buffer if it's less than 3 bytes
        buffer[bytes_read] = '\0';

        // Encode the current 32-bit (4-byte or less) chunk
        unsigned char *encoded_chunk = encode64(buffer);
        if (encoded_chunk == NULL) {
            fclose(file);
            free(image_output);
            return NULL;  // try catching this
        }

        // Append the encoded chunk to the final output
        strcat(image_output, encoded_chunk);
        free(encoded_chunk);  
    }

    fclose(file);
    return image_output; 

}


void clear_decoding_table(){
    free(decoding_table);
}

#endif // BASE64_H