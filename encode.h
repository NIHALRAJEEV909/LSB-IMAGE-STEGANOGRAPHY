#ifndef ENCODE_H

#define ENCODE_H

//contains user defined types
#include "types.h" 

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    //beautiful.bmp
    char *src_image_fname;
    //file address of beautiful.bmp file 
    FILE *fptr_src_image;
    //to store bmp file size 
    uint image_capacity; 
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    //secret.txt
    char *secret_fname;
   //file address of secret.txt 
    FILE *fptr_secret; 
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    //secret file size
    long size_secret_file; 

    /* Stego Image Info */
    //output.bmp
    char *stego_image_fname;
    //file address of output.bmp 
    FILE *fptr_stego_image; 

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/*Encode magic string size*/
Status encode_magic_string_size(int size,EncodeInfo *encInfo);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

Status encode_secret_file_extn_size(int size,EncodeInfo *encinfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer); //encode a char

Status encode_size_to_lsb(int data, char *image_buffer); //encode a int

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest); 

#endif
