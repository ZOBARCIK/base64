//message to binary
//divide binary into 6 bit chunks
//convert each chunk to decimal(which is ascii)
//use decimal as index to look up character in base64 table
//append each chunk character to output string
//if number of bits in binary data is not a multiple of 3, pad with = sign
//handle data 3bytes at a time, 24 is EKOK of 6 and 8
//build decoding table when needed in order to save some ROM on the controller



#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


unsigned char *encode64(const unsigned char *input, size_t input_length);
unsigned char *build_decodding_table();
unsigned char *decode64(const unsigned char *input, size_t input_length);
void clear_decoding_table();
void *encode_image(const char *filepath);
void *decode_image(const char *filepath);
unsigned char *encode_message(const unsigned char *input);
unsigned char *decode_message(const unsigned char *input);
void export_data(const unsigned char *data, size_t data_length, const char *filename);
void build_image(const unsigned char *data, size_t data_length, const char *filename);
char* get_time();


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




unsigned char *encode64(const unsigned char *input, size_t input_length){



    //size_t input_length = strlen(input);
    size_t encoded_length = 4*(input_length + 2)/3; //create 4 byte chunk of output

    if (input_length>encoded_length) {
        printf("Error:1");
        return NULL; //input overflow, try catching this in future
    }

    unsigned char *encoded_out = malloc(encoded_length+1);
    if (encoded_out == NULL) {
        printf("Error:2");
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
        *pout++= base64_table[pin[0] >> 2]; // shift right 2 bits 0 0 b7 b6 b5 b4 b3 b2   
        *pout++= base64_table[((pin[0] & 0x03) << 4) | (pin[1] >> 4)]; //  b1 b0 0 0 0 0 0 0 | 0 0 0 0 b15 b14 b13 b12 == b1 b0 b15 b14 b13 b12 0 0
        *pout++= base64_table[((pin[1] & 0x0f) << 2) | (pin[2] >> 6)]; //  b11 b10 b9 b8 0 0 0 0 | 0 0 0 0 0 0 b23 b22 == b11 b10 b9 b8 b23 b22 0 0
        *pout++=base64_table[pin[2] & 0x03f]; // b21 b20 b19 b18 b17 b16 0 0
        pin +=3;
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

unsigned char *decode64(const unsigned char *input, size_t input_length){

    

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
void *encode_image(const char *filepath) {
    FILE *file = fopen(filepath, "rb");  // binary read mode
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filepath);
        return NULL;  // try catching this
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // reset the pointer to the beginning


    unsigned char *file_data = (unsigned char *)malloc(file_size);
    if (file_data == NULL) {
        printf("Error: Could not allocate memory for file data\n");
        fclose(file);
        return NULL;  // try catching this
    }

    size_t bytes_read = fread(file_data, 1, file_size, file);
    if (bytes_read != file_size) {
        printf("Error: Could not read the entire file\n");
        free(file_data); 
        fclose(file);
        return NULL;   // try catching this
    }


    fclose(file);

    unsigned char *encoded_output = encode64(file_data, file_size);
    export_data(encoded_output, strlen((const char *)encoded_output), "image_base64");
    free(file_data);

    
}

void *decode_image(const char *filepath){

    FILE *file = fopen(filepath, "r");  //read mode
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filepath);
        return NULL;  // try catching this
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // reset the pointer to the beginning
    

    printf("works till here \n");
    unsigned char *file_data = (unsigned char *)malloc(file_size+1);
    if (file_data == NULL) {
        printf("Error: Could not allocate memory for file data\n");
        fclose(file);
        return NULL;  // try catching this
    }

    size_t bytes_read = fread(file_data, 1, file_size, file);
    if (bytes_read != file_size) {
        printf("Error: Could not read the entire file\n");
        free(file_data); 
        fclose(file);
        return NULL;   // try catching this
    }

    size_t decoded_length = (file_size / 4) * 3;  // Approximate size for decoded data
    unsigned char *decoded_output = decode64(file_data, file_size);
    //size_t image_data_length = strlen((const char *)decoded_image_data);    // Get the length of the decoded data
    //build_image(decoded_output, strlen((const char *)decoded_output), "image_base64");
    free(file_data);
}


unsigned char *encode_message(const unsigned char *input) {

    size_t input_length = strlen((const char *)input);
    return encode64(input, input_length);
}

unsigned char *decode_message(const unsigned char *input) {
 
    size_t input_length = strlen((const char *)input);
    return decode64(input, input_length);
}



void clear_decoding_table(){
    free(decoding_table);
}


void export_data(const unsigned char *data, size_t data_length, const char *filename) {

    char output_name[256];
    snprintf(output_name, sizeof(output_name), "%s_%s.txt", filename, get_time());

    FILE *file = fopen(output_name, "wb");



    size_t written = fwrite(data, sizeof(unsigned char), data_length, file);

    fclose(file);

}

void build_image(const unsigned char *data, size_t data_length, const char *filename) {
    // Construct the full file name with ".jpg" extension
    char output_name[256];
    snprintf(output_name, sizeof(output_name), "%s_%s.jpg", filename, get_time());

    // Open the file in binary write mode
    FILE *file = fopen(output_name, "wb");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", output_name);
        return;  // Try catching this
    }

    // Write the binary data to the file
    size_t written = fwrite(data, sizeof(unsigned char), data_length, file);
    if (written != data_length) {
        printf("Error: Could not write the complete data to file %s\n", output_name);
        fclose(file);
        return;  // Try catching this
    }

    // Close the file
    fclose(file);
    printf("Image file %s created successfully\n", output_name);
}



char* get_time() {
    static char time_str[100];
    
    time_t current_time = time(NULL);
    
    if (current_time == ((time_t)-1)) {
        snprintf(time_str, sizeof(time_str), "Error getting the current time");
        return "nulltime";
    }

    struct tm *local_time = gmtime(&current_time);

    if (local_time == NULL) {
        snprintf(time_str, sizeof(time_str), "Error UTC time");
        return "nulltime";
    }

    strftime(time_str, sizeof(time_str), "%d%m%Y_%H%M%S", local_time);

    return time_str;
}



#endif // BASE64_H