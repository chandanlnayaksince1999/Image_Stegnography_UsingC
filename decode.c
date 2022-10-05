#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>

Status do_decoding (DecodeInfo *decInfo)
{
    if(decode_open_files(decInfo) == e_success )
    {
        printf("INFO:\tOpening files function is success\n");
        printf("\n<-------------------Started Decoding------------------->\n");

        if(decode_magic_string(decInfo) == e_success)
        {
            printf("INFO:\tDecoding magic string is success\n");

            if(decode_file_extn_size(strlen(".txt"), decInfo->fptr_stego_image) == e_success)
            {
                printf("INFO:\tDecoding file extension size is success\n");
                
                if(decode_secret_file_extn(decInfo->decode_extn_secret_file, decInfo) == e_success)
                {
                    printf("INFO:\tDecoding secret file extension successful\n");

                    if( decode_secret_file_size ( decInfo->secret_file_size, decInfo) == e_success)
                    {
                        printf("INFO:\tDecoding secret file size is success\n");

                        if(decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("INFO:\tDecoding secret file data is success\n");
                        }
                        else 
                        {
                            printf("INFO:\tDecoding secret file data is failure\n");
                        }

                    }
                    else
                    {
                        printf("INFO:\tDecoding secret file size is success\n");           
                    }
                }
                else
                {
                    printf("INFO:\tDecoding secret file extesion is failure\n");
                }
            }
            else 
            {
                printf("INFO:\tDecoding file extension size is failure\n");
            }
        }
        else
        {
            printf("INFO:\tDecoding magic string is failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO:\tOpening files function is failure\n");
        return e_failure;
    }
    return e_success;

}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if(decInfo->fptr_secret == NULL)
    {
        fprintf(stderr, "Can't open file", decInfo->secret_fname);
        return e_failure;
    }
    else
    {
        for(int i = 0; i < decInfo->secret_file_size; i++)
        {
            fread (decInfo->stego_image_fname, 8, sizeof(char), decInfo->fptr_stego_image);
            decode_byte_from_lsb(&ch, decInfo->stego_image_fname);
            fputc(ch, decInfo->fptr_secret);
        }
        return e_success;
    }
}

Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];
    fread (str, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(str, &file_size);
    decInfo->secret_file_size = file_size;
    return e_success;
}
Status decode_extn_data_from_image( int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread ( decInfo->stego_image_fname, sizeof(char), 8, fptr_stego_image);
        decode_byte_from_lsb(&decInfo->decode_extn_secret_file[i], decInfo->stego_image_fname);
    }
    return e_success;
}
Status decode_secret_file_extn(char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt";
    int i = strlen (file_ext);
    decInfo->decode_extn_secret_file = malloc ( i + 1);
    decode_extn_data_from_image( strlen(file_ext), decInfo->fptr_stego_image, decInfo);
    decInfo->decode_extn_secret_file[i] = '\0';
    if ( strcmp(decInfo->decode_extn_secret_file, file_ext) == 0 )
        return e_success;
    else
        return e_failure;
}

Status decode_size_from_lsb (char *buffer, int *size)
{
    int j = 31;
    int num = 0x00;
    for(int i = 0; i < 32; i++ )
    {
        num = ( (buffer[i] & 0x01 ) << j-- ) | num;
    }
    *size = num;
}

Status decode_file_extn_size(int size, FILE *fptr_stego_image)
{
    char str[32];
    int length;

    fread (str, 32, sizeof(char), fptr_stego_image);
    decode_size_from_lsb(str, &length);

    if(length == size)
        return e_success;
    else
        return e_failure;
}

Status decode_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo )
{
    int i;
    char str[8];
    for(i=0; i < size; i++)
    {
        fread(str, 8, sizeof(char), fptr_stego_image);
        decode_byte_from_lsb (&decInfo->magic_data[i],str);
    }
    decInfo->magic_data[i] = '\0';
    return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    int shift = 7;
    unsigned char ch = 0x00;
    for(int i = 0; i < 8; i++)
    {
        ch = ((image_buffer[i] & 0x01) << shift-- ) | ch;
    }
    *data = ch;
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek ( decInfo->fptr_stego_image, 54L, SEEK_SET );
    int i = strlen ( MAGIC_STRING);
    decInfo->magic_data = malloc( strlen(MAGIC_STRING) + 1);

    decode_data_from_image(strlen(MAGIC_STRING), decInfo->fptr_stego_image, decInfo);

    if(strcmp (decInfo->magic_data, MAGIC_STRING ) == 0)
        return e_success;
    else
        return e_failure;
}

Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    //do error handling
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open the file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

Status read_and_validate_decode_args (int argc, char **argv, DecodeInfo *decInfo)
{
    if( argc <= 4 )
    {
        if(argv[2] != NULL)
        {
            if(strstr(argv[2], ".") != NULL)
            {
                if( strcmp((strstr(argv[2], ".")), ".bmp") == 0 )
                {
                    decInfo->stego_image_fname = argv[2];
                }
                else
                {
                    printf("INFO:\tError: Third argument should be .bmp extension stego file\n\tUsage: Decoding: ./a.out -d stego.bmp\n");
                    return e_failure;
                }
            }
            else
            {
                    printf("INFO:\tError: Third argument should be .bmp extension stego file\n\tUsage: Decoding: ./a.out -d stego.bmp\n");
                    return e_failure;
            }
        }
        else
        {
            printf("INFO:\tError: Third argument should be .bmp extension stego file\n\tUsage: Decoding: ./a.out -d stego.bmp\n");
            return e_failure;
        }
        if( argv[3] != NULL )
        {
            if( strstr(argv[3],".") != NULL )
            {
                if(strcmp((strstr(argv[3], ".")), ".txt") == 0)
                {
                    decInfo->secret_fname = argv[3];
                }
                else
                {
                    printf("INFO:\tInvalid option:\nUsage: decode: ./a.out -d stego.bmp decode_secret.txt\n");
                    return e_failure;
                }
            }
            else
            {
                    printf("INFO:\tInvalid option:\nUsage: decode: ./a.out -d stego.bmp decode_secret.txt\n");
                    return e_failure;
            }
        }
        else 
        {
            printf("INFO:\tAs there is no txt file passed, decode_secret.txt file is created\n");
            decInfo->secret_fname = "decoded_secret.txt";
        }
        return e_success;
    }
    else
    {
        printf("INFO:\tError: Invlalid number of arguments\nUsage for decoding: ./a.out -d <encoded bmp file> <text file to decoded the secret message>\n\tPassing of text file is optional\n");
    }
}
