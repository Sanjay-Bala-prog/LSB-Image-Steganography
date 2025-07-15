#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);  
    return ftell(fptr);
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity < encInfo->size_secret_file * 8 + 54)
    {
        printf("Image is not enough space\n");
        return e_failure;
    }
    else
    {
       //  printf("Checking capacity is successfully executed\n");
        return e_success;
    }
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("Opened %s\n", encInfo->src_image_fname);
    printf("Opened %s\n", encInfo->secret_fname);
    printf("Opened %s\n", encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*first condition -> check the .bmp present or not (argv[2])*/
    /*True -> store the file in scr_image_fname varaible;
     False -> return e_failure;*/
    if(strcasestr(argv[2],".bmp"))
    {
          encInfo->src_image_fname=argv[2];
    }
    else
    {
           printf("Only bmp files\n");
           return e_failure;
    }
    
    /*Second condition -> check the extension based on your idea like (if,array)(Like : ".txt",".c",".sh")*/
    /* True -> store the file in  scr_image_fname varaible;
      False -> return e_failure;
    */
     char *arr[3]={".txt",".c",".sh"};
     int found=0;
     for(int i=0;i<3;i++)
     {
        if(strcasestr(argv[3],arr[i]))
        {
            encInfo->secret_fname=argv[3];
            found=1;
            break;
        }
    }
            if(!found)
            {
                printf("Extension should be .txt, .c, .sh\n");
                return e_failure;
            }

    /*Third condition -> check the avg[4] is NULL or not*/
    /* True -> check the .bmp present or not(in argv[4])
            True -> store the file in stego_image_fname varaible
            False -> return e_failure ;
       False -> store the default filename in the stego_image_fname varaible
                stego_image_fname="stegno.bmp";
    */
    if(argv[4])
    {
        if(strcasestr(argv[4],".bmp"))
        {
            encInfo->stego_image_fname=argv[4];
        }
        else
        {
            printf("Output image is should be in bmp\n");
            return e_failure;
        }
    }
    else
    {
         encInfo->stego_image_fname="stegno.bmp";  
    }
   // printf("Read and validation is successfully executed\n");
    return e_success;

}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)!=e_success)
    {
        return e_failure;
    }
    printf("Done\n");
    printf("## Encoding Procedure Started ##\n");
    printf("\n");
    printf("Checking for secret.txt size\n");
    if (encInfo->fptr_secret != NULL)
    {
        printf("Done. Not Empty\n");
    }
     printf("Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(check_capacity(encInfo)!=e_success)
    { 
        return e_failure;
    }
    printf("Done. Found OK\n");
    printf("Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)!=e_success)
    {
        return e_failure;
    }
    printf("Done\n");
    printf("Encoding Magic String Signature\n");
    if(encode_magic_string("#*",encInfo)!=e_success)
    {
        return e_failure;
    }
    printf("Done\n");
        // Extract file extension from secret file name
    char *file_ext = strrchr(encInfo->secret_fname, '.');
    if (!file_ext)
    {
        printf("Secret file has no extension\n");
        return e_failure;
    }
     printf("Encoding %s File Extenstion\n",encInfo->secret_fname);
    if (encode_secret_file_extn(file_ext, encInfo) != e_success)
    {
        return e_failure;
    }
     printf("Done\n");
     printf("Encoding %s File Size\n",encInfo->secret_fname);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        return e_failure;
    }
    printf("Done\n");

    printf("Encoding secret file of size: %ld bytes\n", encInfo->size_secret_file);
     printf("Encoding secret.txt File Data\n");
    if (encode_secret_file_data(encInfo) != e_success)
    {
        return e_failure;
    }
    printf("Done\n");
   
    printf("Copying Left Over Data\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        return e_failure;
    }
    printf("Done\n");
   
    printf("## Encoding Done Successfully ##\n");
    return e_success;
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
      rewind(fptr_src_image);   //beautiful.bmp  //sources image //rewind is used to zero the file
      char buffer[54];                     //header=54 bytes
      fread(buffer,54,1,fptr_src_image);   // header=buffer //byte by byte collect // /*read a data from one file and stored in another file*/
      fwrite(buffer,54,1,fptr_dest_image);  //here write a data from the another file getting
     // printf("copy bmp header is successfully executed\n");
      return e_success;
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if( encode_data_to_image((char*) magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success)
    {
        printf("Magic string is unable to encoding\n");
        return e_failure;
    }   

    return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
      // 1. Encode the length of the extension
      char extn_len = strlen(file_extn);
      if (encode_data_to_image(&extn_len, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
      {
        printf("Secret file extension is not encoding\n");
        return e_failure;
      }

      // 2. Encode each character of the extension
      if (encode_data_to_image((char *)file_extn, extn_len, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
      {
         printf("Secret file extension is not encoding\n");
        return e_failure;
      }

      return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     char size[32];
     for(int i=0;i<32;i++)
     {
        size[i]=(file_size >> (31-i)) & 1;
     }
     if(encode_data_to_image(size,32,encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success)
     {
        printf("Secret file size is not encoding");
        return e_failure;
     }
     return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
   rewind(encInfo->fptr_secret);
    while (fread(&ch, 1, 1, encInfo->fptr_secret) > 0)
    {
        if (encode_data_to_image(&ch, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        {
            printf("secrete file data is not encoding");
            return e_failure;
        }
    }
    return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
      char buffer[8];
      for(int i=0;i<size;i++)
      {
        
        fread(buffer,1,8,fptr_src_image);              // Read 8 bytes from source image
        encode_byte_to_lsb(data[i], buffer);           // Encode 1 byte of secret into these 8 bytes
        fwrite(buffer, 1, 8, fptr_stego_image);        // Write 8 modified bytes into stego image

      }
      return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)  
    {
        image_buffer[i]=image_buffer[i] & ~1;   //clear the lsb of image_buffer
        unsigned char get=data & (1<<(7-i));    //get the MSB 1 bit front data
        get=(get >> (7-i));                     //shift the 1 bit of data to lsb side
        image_buffer[i]=image_buffer[i] | get;  //merge the both data
    }
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src))
    {
        fwrite(&ch,1,1,fptr_dest);
    }
     return e_success;
}

Status encode_size_to_lsb(char data, char *image_buffer)
{
   for(int i=0;i<32;i++)   
    {
        image_buffer[i]=image_buffer[i] & ~1;   //clear the lsb of image_buffer
        unsigned int get=data & (1<<(31-i));    //get the MSB 1 bit front data
        get=(get >> (31-i));                     //shift the 1 bit of data to lsb side
        image_buffer[i]=image_buffer[i] | get;  //merge the both data
    } 
    return e_success;
}