#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

// Structure to hold decoding information
typedef struct _DecodeInfo
{
    // Stego image file name and pointer
    char *stego_image_fname;
    FILE *fptr_stego_image;

    // Output file name and pointer
    char *output_file_name;
    FILE *fptr_output;

    // Decoded secret file extension
    char secret_file_extn[10];

    // Decoded size of the secret file
    int secret_file_size;

} DecodeInfo;

/* Function Prototypes */

// Read and validate input arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// Open necessary files for decoding
Status open_files_decode(DecodeInfo *decInfo);

// Perform the full decoding operation
Status do_decoding(DecodeInfo *decInfo);

// Decode the magic string to verify embedded data
Status decode_magic_string(DecodeInfo *decInfo);

// Decode the size (length) of the secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size);

// Decode the actual secret file extension (e.g., ".txt")
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size);

// Decode the size of the secret file (stored in 4 bytes, big-endian)
Status decode_secret_file_size(DecodeInfo *decInfo);

// Decode the actual content of the secret file and write it to output
Status decode_secret_file_data(DecodeInfo *decInfo);

// Decode multiple bytes from image using LSBs
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

// Decode a single byte from 8 LSBs in image buffer
Status decode_byte_from_lsb(char *image_buffer, char *data);

#endif 
