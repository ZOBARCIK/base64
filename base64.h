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
//decoding

//NOTE: decoding is incomplete(while loop)

#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stdlib.h>


unsigned char *encode64(const unsigned char *input, uint32_t input_length);
unsigned char *build_decodding_table();

//array width 65 maybe?
static char base64_table[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

static char *decoding_table = NULL;
static int mod[4] = {0, 2, 1}; //for padding 


unsigned char *encode64(const unsigned char *input, uint32_t input_length){




    unsigned char encoded_length = 4*(input_length + 2)/3; //create 4 byte chunk of output

    if (input_length>encoded_length) {
        return NULL; //input overflow, try catching this in future
    }

    char *encoded_out = malloc(encoded_length+1);
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

unsigned char *decode64(const unsigned char *input, uint32_t input_length){

    if (decoding_table == NULL) {
        decoding_table=build_decodding_table(); //try catching the null
    }

    if (input_length % 4 != 0) {
        return NULL; //try catching this
    }

    decoded_length = (input_length/4)*3;

    //remove paddings
    if (data[input_length - 1] == '='){(decoded_length)--;
    if (data[input_length - 2] == '='){(decoded_length)--;

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
    while (input_length!=0){
    
    *pout++= base64_table[pin[i] | (pin[i+1] & 0x03)]; //0 0 b7 b6 b5 b4 b3 b2 | b1 b0 0 0 0 0 0 0 = b7 b6 b5 b4 b3 b2 b1 b0


    i+=4;
    }
//        *pout++= base64_table[pin[i] >> 2]; // shift right 2 bits 0 0 b7 b6 b5 b4 b3 b2   
//       *pout++= base64_table[((pin[i] & 0x03) << 4) | (pin[i+1] >> 4)]; //  b1 b0 0 0 0 0 0 0 | 0 0 0 0 b15 b14 b13 b12 == b1 b0 b15 b14 b13 b12 0 0
//       *pout++= base64_table[((pin[i+1] & 0x0f) << 2) | (pin[i+2] >> 6)]; //  b11 b10 b9 b8 0 0 0 0 | 0 0 0 0 0 0 b23 b22 == b11 b10 b9 b8 b23 b22 0 0
//        *pout++=base64_table[pin[2] & 0x03f]; // b21 b20 b19 b18 b17 b16 0 0
//        i+=3;

        input_length-=4;
    }
}


unsigned char *build_decodding_table(){

    decoding_table=malloc(256);
    if (decoding_table == NULL) {
        return NULL; //try catching this
    }

    uint32_t i=0;
    while(i<=64){
        decoding_table[(unsigned char) base64_table[i]]=i;
        i++;
    }
    return decoding_table;
}

void clear_decoding_table(unsigned char *decoding_table){
    free(decoding_table);
}

#endif // BASE64_H