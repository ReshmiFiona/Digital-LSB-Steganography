#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;		  			//Pointer to store address of stego file name.
    FILE *fptr_stego_image;		  				//Pointer to store address of stego bmp file.			
    /* Output File Info */
    char output_file_fname[10];					//Pointer to store address of output file name.
    FILE *fptr_output_file;						//Pointer to store address of output file.
    int output_file_size;						//Stores size of output file
    
    int file_extn_size;							//Stores size of extension of output file
    char output_file_extn[MAX_FILE_SUFFIX + 1];	//Array to store extension of output file.

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate files */
Status read_and_validate_decode(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for bmp file */
Status open_bmp_file(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/* Decode data to image */
Status decode_data_from_image(int size, char *char_data, DecodeInfo *decInfo);

/* Decode a byte from lsb of image data */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode size from lsb of image data */
Status decode_size_from_lsb(int* size, DecodeInfo *decInfo);

/* Get File pointers for secret file */
Status open_secret_file(DecodeInfo *decInfo);

#endif
