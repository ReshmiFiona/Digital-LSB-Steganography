/*
Title	: DIGITAL LSB STEGANOGRAPHY
Date	: 13-01-2024
Name	: Reshmi Fiona T

Description: 

	Steganography is the art of hiding information that communication  is  taking  place, by  hiding  information  in  other  information. Many different carrier file formats can be used, but digital images are the most popular because of their frequency on the internet. For hiding secret  information in images, there exists a large variety of steganography techniques some are more complex than others and all of them  have respective  strong  and weak points. Different applications may require  absolute  invisibility  of the secret information,while others require a large secret message to be hidden. This project is called LSB Steganography because we are hiding the secret data in the Least Significant Bit (LSB) of the innocent source image.


Input CLAs:
			1. -e (for Encoding)
			2. Source image file(.bmp file) 
			3. Secret file (.txt file)
			4. Stego image filename [Optional]
		
			1. -d (for Decoding)
			2. Stego image file (.bmp file)
			3. Output file name [Optional]

Sample execution: -

Test Case 1:

./a.out -e beautiful.bmp secret.txt stego_image.bmp

INFO: Selected Encoding
INFO: Read and validation is done successfully
INFO: Opening required files
INFO: Opened beautiful.bmp
INFO: Opened secret.txt
INFO: Opened encoded_img.bmp
INFO: Done
INFO: ## Encoding Procedure Started ##
INFO: Checking for secret.txt size
INFO: Done. Not Empty
INFO: Checking for beautiful.bmp capacity to handle secret.txt
INFO: Done. Found OK
INFO: Copying Image Header
INFO: Done
INFO: Encoding Magic String Signature
INFO: Done
INFO: Encoding secret.txt File Extension Size
INFO: Done
INFO: Encoding secret.txt File Extension
INFO: Done
INFO: Encoding secret.txt File Size
INFO: Done
INFO: Encoding secret.txt File Data
INFO: Done
INFO: Copying Left Over Data
INFO: ## Encoding Done Successfully ##

Test Case 2:

./a.out -d stego_image.bmp output

INFO: Selected Decoding
INFO: Read and validation is done successfully
INFO: ## Decoding Procedure Started ##
INFO: Opening required files
INFO: Opened stego_image.bmp
INFO: Decoding Magic String Signature
INFO: Done
INFO: Decoding Output File Extension Size
INFO: Done
INFO: Decoding Output File Extension
INFO: Done
INFO: Decoding Output File Size
INFO: Done
INFO: Decoding Output File Data
INFO: Done
INFO: ## Decoding Done Successfully ##
*/




#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
	//Declare the variables.
	int operation_type;
	//Check the number of arguments minimum required argument is greater than or equal to 3.
	if(argc >= 3)
	{
		//Call check operation and store the operation in a variable.
		operation_type = check_operation_type(argv);

		//Check the operation type.
		//Error handling, If e unsupported print invalid with usage.
		if(operation_type == e_unsupported)
		{
			printf("ERROR: Invalid! Please pass the correct option.\nUsage: Pass -e for encoding and -d for decoding.\n");
			printf("%s : Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
			printf("%s : Decoding: %s -d <.bmp file> [output file]\n", argv[0],argv[0]);
			return e_failure;
		}

		//Encoding, If e_encode print selected encoding.
		else if(operation_type == e_encode)
		{
			EncodeInfo encInfo;
			printf("INFO: Selected Encoding\n");
			//Check whether the arguments are greater than or equal to 4.
			if(argc >= 4)
			{
				//File validation.
				if(read_and_validate_encode_args(argv, &encInfo) == e_success)
				{
					printf("INFO: Read and validation is done successfully\n");

					//Encoding the secret data.
					if(do_encoding(&encInfo) == e_success)
					{
						printf("INFO: ## Encoding Done Successfully ##\n");
					}
					else
					{
						printf("INFO: Encoding Failed\n");
						return e_failure;
					}
				}
				else
				{
					printf("INFO: Read and validation failed\n");
					return e_failure;
				}
			}
			else
			{
				//If the arguments are less than 4 then print the error message.
				printf("ERROR: Arguments are missing\n");
				printf("%s : Encoding: %s -e <.bmp file> <.txt file> [output file]\n", argv[0],argv[0]);
				return e_failure;
			}
		}

		//Decoding, If e_decode print selected decoding.
		else if(operation_type == e_decode)
		{
			printf("INFO: Selected Decoding\n");
			DecodeInfo decInfo;
			//Check whether the arguments are greater than or equal to 3
			if(argc >= 3)
			{
				//File validation
				if(read_and_validate_decode(argv, &decInfo) == e_success)
				{
					printf("INFO: Read and validation is done successfully\n");

					//Decoding the secret data from stego image.
					if(do_decoding(&decInfo) == e_success)
					{
						printf("INFO: ## Decoding Done Successfully ##\n");
					}
					else
					{
						fprintf(stderr,"ERROR: Decoding Failed\n");
						return e_failure;
					}
				}
				else
				{
					fprintf(stderr, "ERROR: Read and validation failed\n");
					return e_failure;
				}
			}
			else
			{
				//If the arguments are less than 3 then print the error message.
				fprintf(stderr,"ERROR: Arguments are missing\n");
				printf("%s : Decoding: %s -d <.bmp file> [output file]\n", argv[0],argv[0]);
				return e_failure;
			}
		}
	}
	else
	{
		//If arguments are less than 3 print the error message.
		printf("ERROR: Arguments are missing. Please pass the required arguments.\n");
		printf("%s : Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
		printf("%s : Decoding: %s -d <.bmp file> [output file]\n", argv[0],argv[0]);
		return e_failure;
	}
	return e_success;
}

//Common function for both encoding and decoding.
OperationType check_operation_type(char *argv[])
{
	//Check if the argv[1] is present ie) string address not equal to NULL.
	if(argv[1] != NULL)
	{
		//Check the option.
		//Check argv[1] is -e or not.
		if(strcmp(argv[1],"-e") == 0)
		{
			//If "-e", return e_encode.
			return e_encode;
		}
		//Check argv[1] is -d or not.	
		else if(strcmp(argv[1],"-d") == 0)
		{
			//If "-d", return e_decode.
			return e_decode;
		}	
		else
		{
			//Else return e_unsupported.
			return e_unsupported; 
		}
	}
	//If argv[1] is NULL, return e_unsupported.
	return e_unsupported;
}
