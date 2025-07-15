#include <stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"

// Read and validate input arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    decInfo->stego_image_fname=argv[2];          //here storing the image input file 
    decInfo->output_file_name="Outputfile";      //default output file name
    return e_success;
}

// Open necessary files for decoding
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"rb");

    if(decInfo->fptr_stego_image == NULL)
    {
        printf("The Secret file is empty");
        return e_failure;
    }
    printf("opened %s\n",decInfo->stego_image_fname);
    return e_success;
}

// Perform the full decoding operation
Status do_decoding(DecodeInfo *decInfo)
{

    printf("## Decoding Procedure Started ##\n");
    printf("Opening required files\n");
    if (open_files_decode(decInfo)!=e_success)
    {
        return e_failure;
    }      

    // Skip BMP header (first 54 bytes)
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    
    printf("Decoding Magic String Signature\n");
    if(decode_magic_string(decInfo)!=e_success)
    {
        return e_failure;
    }
    printf("Done\n");

    int extn_size=0;
    if(decode_secret_file_extn_size(decInfo,&extn_size)!=e_success)
    {
        printf("Secret file extension size is not decoding\n");
        return e_failure;
    }
        //   printf("Secret file extension size is decoding\n");
    printf("Decoding Output File Extenstion\n");
    if(decode_secret_file_extn(decInfo,extn_size)!=e_success)
    {
         printf("Secret file extension is not decoding\n");
        return e_failure;
    }
     printf("Done\n"); 

    
    printf("Output File not mentioned. Creating Outputfile%s as default\n",decInfo->secret_file_extn);
    printf("Opened Outputfile%s\n",decInfo->secret_file_extn);
    printf("Done. Opened all required files\n");

    printf("Decoding  Outputfile%s File Size\n",decInfo->secret_file_extn);
     if(decode_secret_file_size(decInfo)!=e_success)
     {
        printf("Secret file size is not decoding\n");
        return e_failure;
     }
      printf("Done\n");
     //   printf("Secret file size is decoding\n");
      printf("Decoding  Outputfile%s File data\n",decInfo->secret_file_extn);
     if(decode_secret_file_data(decInfo)!=e_success)
     {
        printf("Secret file data is not decoding\n");
        return e_failure;
     }
     printf("Done\n");
     //   printf("Secret file data is decoding\n");
     printf("## Decoding Done Successfully ##\n");
     return e_success;
}

// Decode the magic string to verify embedded data
Status decode_magic_string(DecodeInfo *decInfo)
{
        char size[3];

        if(decode_data_from_image(size,2,decInfo->fptr_stego_image)!=e_success)
        {
            return e_failure;
        }
        size[2]='\0';
        if(strcmp(size,"#*")!=0)
        {
           printf("Magic srting is mismatching\n");
           return e_failure;
        }
        else
        {
           // printf("Magci string is matching %s\n",size);
            return e_success;
        }
}

// Decode the size (length) of the secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size)
{
    char size;
    if(decode_data_from_image(&size,1,decInfo->fptr_stego_image)!=e_success)  //& is address of the size //it decode the extention size from image 
    {
        printf("File extension size is not decoding\n");
        return e_failure;
    }
    *extn_size=(unsigned char)size;        //no negative size will be send //extenstion size storing
    printf("The extension size is %d\n",*extn_size);                              
    return e_success;
}


// Decode the actual secret file extension (e.g., ".txt")
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size)
{
     if(decode_data_from_image(decInfo->secret_file_extn,extn_size,decInfo->fptr_stego_image)!=e_success)
     {
        printf("File extension is not decoding\n");
        return e_failure;
     }
     decInfo->secret_file_extn[extn_size]='\0';
     return e_success;
}


// Decode the size of the secret file (stored in 4 bytes, big-endian)
Status decode_secret_file_size(DecodeInfo *decInfo)
{
     char buffer[32];  //buffer to hold the 4 bytes
     if(decode_data_from_image(buffer,32,decInfo->fptr_stego_image)!=e_success)
     {
        return e_failure;
     }
    int file_size=0;
    for(int i=0;i<32;i++)
    {
        buffer[i]=buffer[i] & 1;
        file_size=(file_size<<1) | buffer[i];
    }
    decInfo->secret_file_size=file_size;
    // printf("Decoded secret file size: %d bytes\n", file_size);
    return e_success;
}

// Decode the actual content of the secret file and write it to output
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer;
    char output_filename[100];

    sprintf(output_filename,"%s%s",decInfo->output_file_name,decInfo->secret_file_extn);   // Create output filename like "decoded_output.txt"
    decInfo->fptr_output=fopen(output_filename,"wb");    // Open output file for writing

    if(decInfo->fptr_output == NULL)
    {
        printf("Output is not detcted\n");
        return e_failure;
    }
   
     for(int i=0;i<decInfo->secret_file_size;i++)
     {
          if(decode_data_from_image(&buffer,1,decInfo->fptr_stego_image)!=e_success)
          {
            return e_failure;
          }
          fwrite(&buffer,1,1,decInfo->fptr_output);
     }
     fclose(decInfo->fptr_output);
     return e_success;

}

// Decode multiple bytes from image using LSBs
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(buffer,1,8,fptr_stego_image);      //read a 8 byte from the image

        decode_byte_from_lsb(buffer,&data[i]);   //extract 1 byte from the lsb
    }
    return e_success;
}

// Decode a single byte from 8 LSBs in image buffer
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    *data=0;
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=image_buffer[i] & 1;
        *data= (*data << 1);                    //extract LSB bits
        *data=*data | image_buffer[i];
    }
    return e_success;
}
