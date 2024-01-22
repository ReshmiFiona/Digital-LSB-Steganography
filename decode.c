
#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

//Function Definitions. 

/* Read File names and validate
 * Input: Command line arguments.
 * Output: Stores the files names.
 * Return: e-success or e_failure.
 */

Status read_and_validate_decode(char *argv[], DecodeInfo *decInfo)
{
	//Check the source(stego image) file (argv[2] is a .bmp file or not.
	if (strcmp(strstr(argv[2], "."), ".bmp") == 0)		
	{
		//If yes, Store the address of the source(stego image) file name.
		decInfo -> stego_image_fname = argv[2];
	}
	else
	{
		//ERROR
		printf("ERROR: Source file %s format should be .bmp\n", argv[2]);
		printf("%s : Decoding : %s -e <.bmp file> [output file]\n", argv[0],argv[0]);
		return e_failure;
	}
	//Checking whether the file name(argv[3]) is passed or not.
	if (argv[3] != NULL)		
	{	
		//If passed, store the output file name.
		strcpy(decInfo -> output_file_fname, argv[3]);
		//decInfo -> output_file_fname = argv[3];					
	}
	else
	{
		//If not, store the default file name.
		strcpy(decInfo -> output_file_fname, "output");
		//decInfo -> output_file_fname = "output";	
	}
	return e_success;
}

/* Open Stego bmp image file
 * Inputs: Stego Image file
 * Output: Open the stego file and read the address and store it in a file pointer.
 * Return: e_success or e_failure.
 */
Status open_bmp_file(DecodeInfo *decInfo)
{
	//Opening source (stego image file) and storing the address of the file in a file pointer.
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");			 

	//Error Handling. If file pointer is NULL.
	if (decInfo->fptr_stego_image == NULL)                                                    
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return e_failure;
	}
	return e_success;						
}

/* Decoding secret data from stego image file to output file.
 * Input: File information of stego image file and output file
 * Output: Decoded secret message copied to output file
 * Return: e_success or e_failure.
 */

Status do_decoding(DecodeInfo *decInfo)
{
	printf("INFO: ## Decoding Procedure Started ##\n");

	//Opening bmp file.
	printf("INFO: Opening required files\n");
	if (open_bmp_file(decInfo) == e_success)
	{
		printf("INFO: Opened %s\n", decInfo -> stego_image_fname);
		//Skip the 54 bytes header.

		//Move the FILE pointer to start.
		rewind(decInfo->fptr_stego_image);
		//Move the FILE pointer to 54th byte from beginning.
		fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
		//Decoding Magic String Signature.
		printf("INFO: Decoding Magic String Signature\n");
		if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
		{
			printf("INFO: Done\n");

			//Decoding Output File Extension Size.
			printf("INFO: Decoding Output File Extension Size\n");
			if(decode_extn_size(decInfo) == e_success)
			{
				printf("INFO: Done\n");

				//Decoding Output File Extension.
				printf("INFO: Decoding Output File Extension\n");
				if(decode_secret_file_extn(decInfo -> file_extn_size, decInfo) == e_success)
				{
					printf("INFO: Done\n");

					//Concatenate output file name and extension.
					strcat(decInfo -> output_file_fname, decInfo->output_file_extn);

					//Decoding Output File Size.
					printf("INFO: Decoding Output File Size\n");
					if(decode_secret_file_size(decInfo) == e_success)
					{
						printf("INFO: Done\n");

						//Decoding Output File Data.
						printf("INFO: Decoding Output File Data\n");
						if(decode_secret_file_data(decInfo -> output_file_size, decInfo) == e_success)
						{
							printf("INFO: Done\n");
							fclose(decInfo->fptr_stego_image);
							fclose(decInfo->fptr_output_file);
							return e_success;
						}
						else
						{
							printf("INFO: Decoding output file data failed\n");
							return e_failure;
						}
					}
					else
					{
						printf("INFO: Decoding output file size failed\n");
						return e_failure;
					}
				}
				else
				{
					printf("INFO: Decoding output file extension failed\n");
					return e_failure;
				}
			}
			else
			{
				printf("INFO: Decoding output file extension size failed\n");
				return e_failure;
			}
		}
		else
		{
			printf("INFO: Decoding magic string failed\n");
			return e_failure;
		}
	}
	else
	{
		printf("INFO: Failed to open bmp file.\n");
		return e_failure;
	}
}

/* Decoding the magic string from stego image file.
 * Input: Magic string and File information of stego image and output file.
 * Output: Decode the magic string and compare. If magic string is found proceed or else stop decoding.
 * Return: e_success or e_failure.
 */

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
	int size = strlen(magic_string);
	//Character array to get the decoded magic string.
	char decoded_magic_str[size + 1];
	//Decode data from inage.
	if(decode_data_from_image(size, decoded_magic_str, decInfo) == e_success)
	{
		decoded_magic_str[size] = '\0';
		//Validating magic string by comparing the decoded magic string with the original magic string.
		if(strcmp(decoded_magic_str, magic_string) == 0)					
		{
			return e_success;
		}
		else
		{
			return e_failure;
		}
	}
}

/* Decoding file extenstion size from stego image file
 * Input: File information of stego image file and output file
 * Output: Decode the output file extenstion size from stego image.
 * Return: e_success or e_failure
 */

Status decode_extn_size(DecodeInfo *decInfo)
{
	//Decode the extension size from lsb of each byte of image data.
	decode_size_from_lsb(&decInfo->file_extn_size, decInfo);			

	return e_success;
}

/* Decoding file extenstion from stego image file
 * Input: Extenstion Size and File info of stego image
 * Output: Decode the output file extenstion from stego image file
 * Return: e_success or e_failure
 */

Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
	//Decode secret file extension from the image.
	if(decode_data_from_image(size, decInfo->output_file_extn, decInfo) == e_success)
	{
		decInfo->output_file_extn[size] = '\0';

		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/* Decode file size from stego image
 * Input: FILE info of stego image and output file
 * Output: Decodes the file image and store in image_data_size
 * Return: e_success or e_failure
 */

Status decode_secret_file_size(DecodeInfo *decInfo)
{
	//Decode output file size from the lsb of each byte in image.
	decode_size_from_lsb(&decInfo -> output_file_size, decInfo);		

	return e_success;
}

/* Decode file data from stego image
 * Input: FILE info of stego image and output decode file
 * Output: Write decode data in the output file
 * Return: e_success or e_failure
 */

Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
	char secret_file_data[size + 1];
	//Open secret file.
	open_secret_file (decInfo);
	//Decode secret data from the image.
	if(decode_data_from_image(size, secret_file_data, decInfo) == e_success)
	{
		secret_file_data[size] = '\0';
		//Write the secret data in a file.
		fwrite(secret_file_data, size, 1, decInfo -> fptr_output_file);	
	}

	return e_success;
}

/* Decode data from image.
 * Input: no of characters and character data array, stego image file pointer.
 * Output: Decode the data from the image_data 
 * Return: e_success or e_failure
 */

Status decode_data_from_image(int size, char *char_data, DecodeInfo *decInfo)
{
	char image_buffer[8];
	//Loop until no of character times.
	for(int i = 0; i < size; i++)							
	{
		//Reading 8 bytes from stego image and storing it into image buffer.
		fread(image_buffer, 8, 1, decInfo -> fptr_stego_image);
		//decode secret data (byte) from lsb.	
		decode_byte_from_lsb(&char_data[i] ,image_buffer);
	}

	return e_success;
}

/* Decode the byte from lsb. 
 * Input: Image_data array and data character
 * Output: Decode the image_data and stores the 1 byte data in decode_data
 * Return: e_success or e_failure
 */

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
	*data = 0;
	//loop till 8 (for 8 bytes)
	for( int i = 0; i < 8; i++)
	{
		//Leftshift the data and store the lsb of image in data.
		//Get the lsb bit from image buffer.
		*data = *data << 1 | image_buffer[i] & 1;		
	}

	return e_success;
}

/* Decode size from stego image byte
 * Input: Image_data array and decode_data character
 * Output: Decode 1 byte from image_data and store in data.
 * Return: e_success or e_failure
 */

Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
	char buffer[32];
	//Read 32 bytes from stego image and store it in buffer.
	fread(buffer, 32, 1, decInfo -> fptr_stego_image);					

	//loop till 32 (for 32 bytes)
	for(int i = 0; i < 32; i++)
	{
		//Leftshift the size and store the lsb of image in size.
		//Get the lsb bit from image buffer.
		*size = *size << 1 | buffer[i] & 1;						
	}
	return e_success;
}

/* Open secret file
 * Inputs: Secret file pointer and file name
 * Output: Open the secret file and read the address and store it in a file pointer.
 * Return: e_success or e_failure.
 */

Status open_secret_file (DecodeInfo *decInfo)
{
	decInfo->fptr_output_file = fopen(decInfo->output_file_fname, "w");			
	//Error handling
	if (decInfo->fptr_output_file == NULL)							
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_file_fname);

		return e_failure;
	}
	return e_success;
}


