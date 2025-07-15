#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))       //selecting the encoding
    {
        return e_encode;            //return 0
    }
    else if(!strcmp(argv[1],"-d"))  //selecting the decode
    {
        return e_decode;            //return 1
    }
    else                            //invalid input
    {
          return e_unsupported;     //return 2
    }
}

int main(int argc,char *argv[])
{
        if(argc<3)
    {
        printf("Enter atleast two arguments like ./a.out source.bmp secret.txt\n");
        return e_failure;
    }
   switch(check_operation_type(argv))
   {
      case e_encode:
            EncodeInfo encode;
            printf("Selected Encoding\n");
           if(read_and_validate_encode_args(argv,&encode)==e_success)
           {
            /*perform for encoding*/
            do_encoding(&encode);
           }
           else
           {
             printf("Read and valid is not completed successfully in encoding\n");
              return 0;
           }
            break;

      case e_decode:
               DecodeInfo decode;
            printf("Selected Decoding\n");
            if(read_and_validate_decode_args(argv,&decode)==e_success)
            {
                do_decoding(&decode);
            }
            else
            {
                printf("Read and valid is not completed successfully in decoding");
                return 0;
            }
            break;

      default:
           printf("Encoding: ./lsb_steg -e <. bmp file> <. txt file> [output file]\n");
           printf("Decoding: ./lsb_steg -d <. bmp file> [output file]\n");
           break;
   }
}
