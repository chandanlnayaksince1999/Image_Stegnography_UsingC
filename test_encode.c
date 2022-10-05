#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>


int main(int argc, char **argv)
{
    if(argc > 1)
    {
        if(check_operation_type( argv ) == e_encode )
        {
            printf("INFO:\tSelected encoding\n");
            EncodeInfo encInfo;
            if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
            {
                printf("INFO:\tRead and validate arguments is a success\n");
                printf("INFO:\tStarted encoding\n");
                if( do_encoding(&encInfo) == e_success)
                {
                    printf("INFO:\tEncoded Successfully\n");
                }
                else
                {
                    printf("INFO:\tEncoding failed\n");
                    return -1;
                }
            }
            else
            {
                printf("INFO:\tRead and validate arguments is a failure\n");
                return -1;
            }
        }
        else if(check_operation_type( argv ) == e_decode )
        {
            printf("INFO:\tSelected decoding\n");
            DecodeInfo decInfo;
            if(read_and_validate_decode_args( argc, argv, &decInfo) == e_success)
            {
            
                printf("INFO:\tRead and validate arguments is a success\n");
                printf("INFO:\tDecoding procedure started\n");

                if(do_decoding(&decInfo) == e_success)
                {
                    printf("INFO:\tDecoded successfully\n");
                }
                else 
                {
                    printf("INFO:\tDecoding failed\n");
                    return -1;
                }
            
            }
            else 
            {
                printf("INFO:\tRead and validate arguments is failure\n");
            }

        }
        else
        {
            printf("Invalid Option\nPlease pass...\n For Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nFor Decoding: ./a.out -d stego.bmp decode.txt\n");
        }
    }
    else
    {
        printf("Invalid number of arguments\nPlease pass...\n For Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nFor Decoding: ./a.out -d stego.bmp decode.txt\n");
    }
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e" ) == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d" ) == 0)
    {
        return e_decode;
    }
    else
    {
        printf("Invalid arguments\n\nPass 2nd arguments as:  1. ""-e"" for encoding\v2. ""-d"" for decoding\n");
        return e_unsupported;
    }
}
