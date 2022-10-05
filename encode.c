#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    if(argc == 4 || argc == 5)
    {
        if(argv[2] == NULL)
        {
            printf("Invalid option\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bm    p\n");
            return e_failure;
        }
        else
        {
            if(strstr(argv[2], ".") != NULL)
            {
                if(strcmp ( (strstr(argv[2], ".")), ".bmp" ) == 0 )
                {
                    encInfo->src_image_fname = argv[2];
                }
                else
                {
                    printf("Invalid option: Third argument should be beatiful.bmp file\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                    return e_failure;
                }
            }
            else
            {
                    printf("Invalid option: Third argument should be beatiful.bmp file\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                    return e_failure;
            }
        }

        if(argv[3] == NULL)
        {
            printf("Invalid option\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bm    p\n");
            return e_failure;
        }
        else
        {
            if(strstr(argv[3], ".") != NULL)
            {
                if(strcmp ( (strstr(argv[3], ".")), ".txt" ) == 0 )
                {
                    encInfo->secret_fname = argv[3];
                }
                else
                {
                    printf("Invalid option: Fourth argument should be secret.txt file\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                return e_failure;
                }
            }
            else
            {
                printf("Invalid option: Fourth argument should be secret.txt file\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                return e_failure;
            }
        }

        if(argv[4] != NULL)
        {
            if(strstr(argv[4], ".") != NULL)
            {
                if(strcmp( (strstr(argv[4], ".")), ".bmp" ) == 0 )
                {
                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    printf("Invalid option: Fifth argument should be with "".bmp"" extension\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Invalid option: Fifth argument should be with "".bmp"" extension\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO:\tCreating a BMP file to encode as the same is not passed as argument\n");
            encInfo->stego_image_fname = "stego.bmp";
        }
    }
    else
    {
        printf("Invalid option\nUsage : \nEncoding : ./a.out -e beautiful.bmp secret.txt <stego.bmp>\n");
        return e_failure;
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
   encInfo->image_capacity = get_image_size_for_bmp ( encInfo->fptr_src_image) ;
   printf("image size %u\n", encInfo->image_capacity);
   encInfo->size_secret_file = get_file_size( encInfo->fptr_secret ) ;

   printf("secret file size %lu\n", encInfo->size_secret_file);
   if( (encInfo->image_capacity) > ((2 + 4 + 4 + 4 + (encInfo->size_secret_file) ) * 8))
   {
       printf("INFO:\tImage size is enough to fit the secret message\n");
       return e_success;
   }
   else
   {
       return e_failure;
   }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread (str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

Status encode_magic_string (const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo );
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo )
{
    int i;
    for( i = 0; i < size; i++ )
    {
        fread(encInfo->image_data, 8, 1, fptr_src_image);
        encode_byte_to_lsb( data[i], encInfo->image_data );
        fwrite(encInfo->image_data, 8, 1, fptr_stego_image);
    }
}

Status encode_byte_to_lsb ( char data, char *image_buffer )
{
    unsigned int mask = 0x80, i;
    for(i=0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7-i));
        mask = mask >> 1;
    }
}

Status encode_secret_file_extn_size (int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[32];
    fread (str, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite (str, 32, 1, fptr_stego_image);
    return e_success;
} 

Status encode_size_to_lsb ( int size, char *image_buffer )
{
    unsigned int mask = 1 << 31,  i;
    for(i=0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31-i));
        mask = mask >> 1;
    }

}

Status encode_secret_file_extn (char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;

}

Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread (str, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite (str, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while ((fread(&ch,1,1,fptr_src))>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
}


Status do_encoding(EncodeInfo *encInfo)
{
    if( open_files( encInfo ) == e_success )
    {
        printf("INFO:\tOpen file is succes\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("INFO:\tCheck capacity is succes\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success)
            {
                printf("INFO:\tCopying BMP header is succes\n");
                if( encode_magic_string( MAGIC_STRING, encInfo ) == e_success  )
                {
                    printf("INFO:\tEncoded magic string\n");
                    strcpy(encInfo->extn_secret_file ,  strstr (encInfo->secret_fname, "."));
                    printf("INFO:\tSecret file extension copied\n");

                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        printf("INFO:\tEncoding secret file extension size is successful\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("info:\tEncoding secret file extension is success\n");

                            if( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success  )
                            {
                                printf("INFO:\tEncode secret file size is success\n");

                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("INFO:\tEncoding secret file data is success\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("INFO:\tCopying remaining data is success\n");
                                    }
                                    else
                                    {
                                        printf("INFO:\tCopying remaining data is failure\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("INFO:\tEncoding secret file data is failure\n");
                                    return e_failure;
                                }


                            }
                            else
                            {
                                printf("INFO:\tEncode secret file size is failure\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("INFO:\tEncoding secret file extension is failure\n");
                            return e_failure;
                        }

                    }
                    else
                    {
                        printf("INFO:\tEncoding secret file extension size is Failed\n");
                        return e_failure;
                    }
                            
                }
                else
                {
                    printf("INFO:\tFailed to encode magic string\n");
                    return e_failure;
                }
            }
            else 
            {
                printf("INFO:\tCopying BMP header is failure\n");
                return e_failure;
            }
            return e_success;
        }
        else
        {
            printf("INFO:\tCheck capacity is failure");
            return e_failure;
        }
    }
    else
    {
        printf("INFO:\tOpen files is failure\n");
        return e_failure;
    }
    return e_success;
}
