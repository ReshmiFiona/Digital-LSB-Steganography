#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Read and validate command line arguments
 * Description: To check whether the file names are in correct formats.
 * Input: Command line Arguments (File names)
 * Output: File names are stored in encoded Info
 * Return: e_success or e_failure
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//Check the source file(argv[2]) is a .bmp file or not.
	if(strcmp(strstr(argv[2],"."), ".bmp") == 0)
	{
		//If yes, Store the address of the source file name.
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		//ERROR.
		fprintf(stderr,"Error : Source file %s format should be .bmp\n", argv[2]);
		printf("%s : Encoding: %s -e <.bmp file> <.txt file> [Output file]\n",argv[0],argv[0]);
		return e_failure;
	}
	//Check the secret file(argv[3]) is a .txt or .sh or .c file and copy the file extension in extn_secret_file.

	if(strcmp(strcpy(encInfo->extn_secret_file, strstr(argv[3], ".")), ".txt") == 0 || strcmp(strcpy(encInfo->extn_secret_file, strstr(argv[3], ".")), ".sh") == 0 || strcmp(strcpy(encInfo->extn_secret_file, strstr(argv[3], ".")), ".c") == 0)
	{
		//If yes, Store the address of the secret file name.
		encInfo->secret_fname = argv[3];	
	}
	else
	{
		//ERROR.
		fprintf(stderr,"Error : Secret file %s format should be .txt or .sh or .c\n", argv[3]);
		printf("%s : Encoding: %s -e <.bmp file> <.txt file> [Output file]\n",argv[0],argv[0]);
		return e_failure;
	}
	//Check if the output file name is passed or not.
	if(argv[4] != NULL)
	{
		//If it is passed, Check the output file is a .bmp file.
		if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
		{
			//if it is a bmp file store the address of the file name.
			encInfo->stego_image_fname = argv[4];
		}
		else
		{
			//If it is not bmp , Create a default file name and store it.
			printf("INFO: Output File is not a .bmp file. Creating stego_img.bmp as default\n");
			encInfo->stego_image_fname = "stego_image.bmp"; 
		}
	}
	else
	{
		printf("INFO: Output File not mentioned. Creating stego_img.bmp as default\n");		
		//If output file is not passed , Create a default file name and store it.
		encInfo->stego_image_fname = "stego_image.bmp"; 
	}
	return e_success;	
}

/*
 * Get the secret file size.
 * Description: To get the sizre of secret file.
 * Input: Secret file pointer.
 * Output: Size
 * Return: Size of files in bytes.
 */
uint get_file_size(FILE *fptr)
{
	uint secret_file_size;

	//Seek to the end of the file.
	fseek(fptr, 0L, SEEK_END);	//file pointer points to the end of the file.

	//Fetch the position of the file pointer and store it in a variable.
	secret_file_size = ftell(fptr);

	//Bring back the file pointer to point the starting position.
	rewind(fptr);

	//Return secret_file_size.
	return secret_file_size;
}

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
	fseek(fptr_image, 18, SEEK_SET);	//file pointer points to the 18 th byte.

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image); 	// Read 4bytes from the file and store in width.

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);	//Read 4bytes after width from the file and store it in height.

	// Return image capacity
	return width * height * 3;	// 3 for RGB.
}


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return: e_success or e_failure
 */
Status open_files(EncodeInfo *encInfo)
{
	// Opening Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");	

	//Error handling
	if (encInfo->fptr_src_image == NULL)	//Check if the file is open.
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}
	else
	{
		printf("INFO: Opened %s\n", encInfo->src_image_fname);
	}

	// Opening Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r"); 

	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}
	else
	{
		printf("INFO: Opened %s\n", encInfo->secret_fname);
	}

	// Opening Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");	

	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}
	else
	{
		printf("INFO: Opened %s\n", encInfo->stego_image_fname);
	}
	printf("INFO: Done\n");
	// No failure return e_success
	return e_success;

}

/* Encoding secret file data to stego image
 * Input: Encode File Information of source image, secret file and stego image
 * Output: Encodes the secret data to stego image
 * Return: e_success or e_failure
 */
Status do_encoding(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");

	//Opening required files.
	if(open_files(encInfo) == e_success)
	{
		printf("INFO: ## Encoding Procedure Started ##\n");
		printf("INFO: Checking for %s size\n", encInfo->secret_fname);

		//Check the capacity of the source image to handle the secret file data.
		if(check_capacity(encInfo) == e_success)
		{
			printf("INFO: Done. Found OK\n");

			//Copy bmp image header.
			printf("INFO: Copying Image Header\n");
			if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			{
				printf("INFO: Done\n");

				//Encoding magic string in stego image file.
				printf("INFO: Encoding Magic String Signature\n");

				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("INFO: Done\n");

					//Encoding secret file extension size.
					printf("INFO: Encoding %s File Extension Size\n", encInfo->secret_fname);
					if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
					{
						printf("INFO: Done\n");

						//Encoding secret file extension.
						printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
						if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							printf("INFO: Done\n");

							//Encoding secret file size.
							printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
							if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
							{
								printf("INFO: Done\n");

								//Encoding secret file data.
								printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("INFO: Done\n");

									//Copy the remaining data.
									printf("INFO: Copying Left Over Data\n");
									if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
									{
										fclose(encInfo->fptr_src_image);
										fclose(encInfo->fptr_stego_image);
										fclose(encInfo->fptr_secret);
										return e_success;
									}
									else
									{
										printf("INFO: Copying remaining data failed.\n");
										return e_failure;
									}
								}
								else
								{
									printf("INFO: Encoding secret file data failed.\n");
									return e_failure;
								}
							}
							else
							{
								printf("INFO: Encoding secret file size failed.\n");
								return e_failure;
							}
						}
						else
						{
							printf("INFO: Encoding secret file extn failed.\n");
							return e_failure;
						}
					}
					else
					{
						printf("INFO: Encoding secret file extension size failed.\n");
						return e_failure;
					}

				}
				else
				{
					printf("INFO: Encoding Magic String Failed.\n");
					return e_failure;
				}
			}
			else
			{
				printf("INFO: Bmp header not copied to output file");
				return e_failure;
			}
		}
		else
		{
			printf("ERROR: File capacity exceeded. Cannot encode %s file data into %s file\n", encInfo->secret_fname, encInfo->src_image_fname);
			return e_failure;
		}
	}
	else
	{
		return e_failure;
		printf("ERROR : Cannot open the files\n");
	}
}

/* Check the capacity of source image file to encode secret data
 * Input: File info source image, stego image and secret file
 * Output: Get Source image capacity and store in image_capacity
 * Return: e_success or e_failure
 */

Status check_capacity(EncodeInfo *encInfo)
{
	//Get the secret file size.		
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

	//Get the size of image file.
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

	//Check if the size of secret file is non empty.
	if (encInfo->size_secret_file)
	{
		printf("INFO: Done. Not Empty\n");
		printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);

		//Image capacity >= 8 * (magic sring size + 4 + secret file extension size + 4 + secret file size)+ 54
		if (encInfo->image_capacity >= (54 + MAX_IMAGE_BUF_SIZE * (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file)))
		{
			return e_success;
		}
		else
		{
			printf("INFO: Cannot hold secret file in image file.");
			return e_failure;
		}
	}
	else
	{
		//If secret file is empty print error message.
		printf("INFO: Secret file is empty.\n");
		return e_failure;
	}
}

/* Copy the bmp image header.
 * Description: Copy the first 54 bytes of header from source bmp image file to stego image file.
 * Input: FILE pointers source and stego image
 * Output: Copies header data of source image to stego image
 * Return: e_success or e_failure
 */

Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
	char bmp_header_data[54];

	//Move the file pointer of image file to the starting position.
	rewind(fptr_src_image);
	//Read 54 bytes from source image bmp file and store it in a bmp header data.
	fread(bmp_header_data, 54, 1, fptr_src_image);
	//Write 54 bytes from bmp header data to stego image file.
	fwrite(bmp_header_data, 54, 1, fptr_dest_image);
	//Validating the bmp header is copied in stego image file or not.
	if(ftell(fptr_dest_image) == 54)
	{
		//If yes return e_success.
		return e_success;
	}
	else
	{
		//If not return e_failure.
		return e_failure;
	}
}

/* Encoding Magic string in stego image file
 * Input: magic string and source and destination file information
 * Output: Encode magic string in stego image first size*8 bytes from image data
 * Return: e_success or e_failure
 */
Status encode_magic_string (char *magic_string, EncodeInfo *encInfo)
{
	//Encode the data to output image.
	if(encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Encoding the secret file extenstion size to stego image file.
 * Input: secret file extension size, file information.
 * Output: Encode file extenstion size to stego image.
 * Return: e_success or e_failure
 */
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
	//Encode secret file extension size to lsb of bytes in stego image.
	if(encode_size_to_lsb(extn_size, encInfo -> fptr_src_image ,encInfo -> fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Encoding file extenstion to stego image file.
 * Input: Secret file extension and file information.
 * Output: Encode secret file extension to stego image file.
 * Return: e_success or e_failure
 */
Status encode_secret_file_extn(const char *extn_secret_file, EncodeInfo *encInfo)
{
	//Encode secret file extension(data) to image.
	if(encode_data_to_image(extn_secret_file, strlen(extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Encoding secret file size data to stego image file.
 * Input: secret file size and file information.
 * Output: Encode secret file size to stego image file.
 * Return: e_success or e_failure
 */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
	//Encode secret file size to stego image file.
	if(encode_size_to_lsb(file_size, encInfo -> fptr_src_image ,encInfo -> fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Encoding secret file data to stego image file.
 * Input: Source and destination file information.
 * Output: Encode secret data to stego image file.
 * Return: e_success or e_failure
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char secret_data[encInfo->size_secret_file];

	//Rewind fptr_secret to start.
	rewind(encInfo->fptr_secret);
	//Read secret_file_size data from fptr_secret, store into secret_data(arr)
	fread(secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);
	//Encode secret file data to stego image file.
	if(encode_data_to_image(secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Encode data to image data
 * Description: Encoding characters to image file.
 * Input: data, data size, File pointer of source and stego image files
 * Output: Encode data to stego image file.
 * Return: e_success or e_failure
 */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char image_buffer[8];
	//Loop until the size of data.
	for(int i = 0; i < size; i++)
	{
		//Read 8byte of data, Store into buffer.
		fread(image_buffer, 8, 1, fptr_src_image);
		//Call the encode_byte_to_lsb(data[0],image_buffer).
		encode_byte_to_lsb(data[i], image_buffer);
		//Write 8byte of encoded buffer data into stego_image.
		fwrite(image_buffer, 8, 1, fptr_stego_image);
	}
	return e_success;
}

/* Encoding byte to lsb.
 * Description: Encode the data to the lsb of each byte in stego image file.
 * Input: Data to be encoded and image buffer data.
 * Output: Encode data to image_buffer
 * Return: e_success or e_failure
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i = 0; i < 8; i++)
	{
		/*Get a bit from the data (start from MSB bit)
		  Clear the LSB from buffer.
		  Replace the LSB to bit from data.*/
		image_buffer[i] = (image_buffer[i] & (~1)) | ((data >> (7 - i)) & 1);
	}
	return e_success;
}

/* Encoding size to lsb.
 * Description: Encode the size to the lsb of each byte of stego image file.
 * Input: Size, image buffer data.
 * Output: Encoding the size to lsb of image data.
 * Return Value: e_success on file errors
 */
Status encode_size_to_lsb(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[32];
	//Read 32 bytes of data from src image to buffer.
	fread(buffer, 32, 1, fptr_src_image);
	//encode_size_to_lsb(extn_size,buffer);
	for(int i = 0; i < 32; i++)
	{
		/*Get a bit from the size (start from MSB bit).
		  Clear the LSB from buffer.
		  Replace the LSB to bit from size.*/
		buffer[i] = (buffer[i] & (~1)) | ((size >> (31 - i)) & 1);
	}
	//Write 32 bytes in stego_image
	fwrite(buffer, 32, 1, fptr_stego_image);

	return e_success;
}

/* Copy remaining data from source image to stego image
 * Input: File pointer of source image and stego image
 * Output: Remaining image data copied from source image file to destination image file.
 * Return: e_success or e_failure
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;

	//Read 1 byte from source image file to ch until there is no character to read.
	while(fread(&ch, 1, 1, fptr_src) > 0)
	{
		//Write the 1 byte from ch to stego image or destination.
		fwrite(&ch, 1, 1, fptr_dest);
	}
	return e_success;
}
