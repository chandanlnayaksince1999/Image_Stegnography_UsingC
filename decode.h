#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char image_data[MAX_IMAGE_BUF_SIZE];
    char *magic_data;

    char *decode_extn_secret_file;
    int secret_file_size;
    char *secret_fname;
    FILE *fptr_secret;

    FILE *fptr_dest_image;

} DecodeInfo;


Status read_and_validate_decode_args(int argc, char **argv, DecodeInfo *decInfo);

Status do_decoding ( DecodeInfo *decInfo );

Status decode_open_files ( DecodeInfo *decInfo );

Status decode_magic_string ( DecodeInfo *decInfo );

Status decode_data_from_image (int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

Status decode_byte_from_lsb ( char *data, char *image_buffer );

Status decode_file_extn_size ( int size, FILE *fptr_stego_image );

Status decode_size_from_lsb ( char *buffer, int *size );

Status decode_secret_file_extn ( char *file_ext, DecodeInfo *decInfo );

Status decode_extn_data_from_image ( int size, FILE *fptr_stego_image, DecodeInfo *decInfo );

Status decode_secret_file_size ( int file_size, DecodeInfo *decInfo );

Status decode_secret_file_data ( DecodeInfo *decInfo );

#endif
