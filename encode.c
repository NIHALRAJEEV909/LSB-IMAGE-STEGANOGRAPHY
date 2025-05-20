#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
//function to read and validate command-line arguments for encoding
Status read_and_validate_encode_args(int argc,char *argv[],EncodeInfo *encInfo) 
{
    //check if the correct number of command-line arguments is provided
    if(argc >= 4 && argc <= 5)
    {
	//to check argv[2] is .bmp file or not
	if(strstr(argv[2],".") != NULL && strcmp(strstr(argv[2],"."),".bmp") == 0) 
	{
	    encInfo -> src_image_fname = argv[2];
	}
	else
	{
	    return e_failure;
	}
	//to check if the secret file has a valid extension
	if(strstr(argv[3],".") != NULL)
	{
	    if(strcmp(strstr(argv[3],"."),".txt") == 0 || strcmp(strstr(argv[3],"."),".c") == 0 || strcmp(strstr(argv[3],"."),".sh") == 0 )
	    {
		encInfo -> secret_fname = argv[3];
	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    return e_failure;
	}
	//to check if the stego image file is provided and has a ".bmp" extension
	if(argv[4] != NULL)
	{
	    if(strstr(argv[4],".") != NULL)
	    {
		if(strcmp(strstr(argv[4],"."),".bmp") == 0 )
		{
		    encInfo -> stego_image_fname = argv[4];
		}
		else
		{
		    return e_failure;
		}
	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    //to set default stego image file name if not provided
	    printf("INFO: Output file not mentioned. Creating steged_img.bmp as default\n");
	    encInfo -> stego_image_fname = "steged_img.bmp";
	}
    }
    else
    {
	return e_failure;
    }
    return e_success;
}

//function to perform encoding steps
Status do_encoding(EncodeInfo *encInfo) 
{
    //function call
    if(open_files(encInfo) == e_success)
    {
	//to print the message
	printf("INFO: Done \n");
	printf("INFO: ## Encoding Procedure Started ##\n");
	//to check if the capacity of the source image is sufficient
	if(check_capacity(encInfo) == e_success) 
	{
	    //to print the message
	    printf("INFO: Done. Found Ok\n");
	    printf("INFO: Copying Image Header\n");
	    //to copy BMP header from source image to stego image
	    if(copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success) 
	    {
		//to print the message
		printf("INFO: Done\n");
		printf("INFO: Encoding Magic String Size\n");
		//to encode the size of the magic string into the stego image
		if(encode_magic_string_size(strlen(MAGIC_STRING), encInfo) == e_success) 
		{
		    //to print the message
		    printf("INFO: Done\n");
		    printf("INFO: Encoding Magic String Signature\n");
		    //to encode the magic string into the stego image
		    if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)  
		    {
			//to print the message
			printf("INFO: Done\n");
			printf("INFO: Encoding %s File Extension Size\n",encInfo -> secret_fname);
			//to encode the size of the secret file extension into the stego image
			if(encode_secret_file_extn_size(strlen(strstr(encInfo -> secret_fname,".")), encInfo) == e_success)
			{
			    //to print the message
			    printf("INFO: Done\n");
			    printf("INFO: Encoding %s File Extension\n",encInfo -> secret_fname);
			    //to encode the secret file extension into the stego image
			    if(encode_secret_file_extn(strstr(encInfo -> secret_fname,"."),encInfo) == e_success)
			    {
				//to print the message
				printf("INFO: Done\n");
				printf("INFO: Encoding %s File Size\n",encInfo -> secret_fname);
				//to encode the size of the secret file into the stego image
				if(encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
				{
				    //to print the message
				    printf("INFO: Done\n");
				    printf("INFO: Encoding %s File Data\n",encInfo -> secret_fname);
				    //to encode the size of the secret file into the stego image
				    if(encode_secret_file_data(encInfo) == e_success)
				    {
					//to print the message
					printf("INFO: Done\n");
					printf("INFO: Copying Left Over Data\n");
					//to copy the remaining data from source image to stego image
					if(copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
					{
					    //to print the message
					    printf("INFO: Done\n");
					}
					else
					{
					    //to print the message
					    printf("copy_remaining_img_data failed");
					    return e_failure;
					}
				    }
				    else
				    {
					//to print the message
					printf("encode_secret_file_data is fail");
					return e_failure;
				    }
				}
				else
				{
				    //to print the message
				    printf("encode_secret_file_size failed");
				    return e_failure;
				}
			    }
			    else
			    {
				//to print the message
				printf("encode_secret_file_extn failed\n");
				return e_failure;
			    }
			}
			else
			{
			    //to print the message
			    printf("encode_secret_file_extn_size failed\n");
			    return e_failure;
			}
		    }
		    else
		    {
			//to print the message
			printf("encode_magic_string failed\n");
			return e_failure;
		    }
		}
		else
		{
		    //to print the message
		    printf("encode_magic_string_size failed\n");
		    return e_failure;
		}
	    }
	    else
	    {
		//to print the message
		printf("Copy bmp header failed");
		return e_failure;
	    }

	}
	else
	{
	    //to print the message
	    printf("File capacity exceeded\n");
	    return e_failure;
	}		      
    }
    else
    {
	//to print the message
	printf("File failed to open\n");
	return e_failure;
    }
    return e_success;
} 

//function to check the capacity of the source image to handle the secret file
Status check_capacity(EncodeInfo *encInfo)
{
    //to find the bmp file size
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    //to find the magic string length
    int magic_string_length = strlen( MAGIC_STRING );
    //to find the extn size
    int length_of_extn = strlen(strstr(encInfo -> secret_fname,"."));
    //to find secret file size
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    printf("INFO: Checking for %s size\n",encInfo -> secret_fname);
    //to check if the secret file is not empty
    if((encInfo -> size_secret_file) != 0)  
    {
	printf("INFO: Done. Not Empty\n");
	printf("INFO: Checking for %s capacity to handle %s\n",encInfo -> src_image_fname,encInfo -> secret_fname);
	//to check if the source image capacity is sufficient to handle the secret file
	if(encInfo -> image_capacity > (54 + 32 + (magic_string_length * 8) + 32 + (length_of_extn * 8) + 32 + (encInfo -> size_secret_file * 8)))
	{
	    return e_success;
	}	
	else
	{
	    return e_failure;
	}
    }
}

//function to get the size of a file
uint get_file_size(FILE *fptr) 
{
    //to move the fptr to end
    fseek(fptr,0,SEEK_END);
    //to return the current position of the file pointer, which is the size of the file
    return ftell(fptr);
}

//function to copy BMP header from source image to stego image
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char str[54];

    fseek(fptr_src_image,0,SEEK_SET); //Move file pointer to 0th position
    fread(str,1,54,fptr_src_image);  // Read 54 bytes from the source image file
    fwrite(str,1,54,fptr_stego_image); // Write 54 bytes to the stego image file
    return e_success;
}

//function to encode a byte into the least significant bit of each byte in a buffer
Status encode_byte_to_lsb(char data,char *image_buffer) 
{
    for(int i = 7;i >= 0;i--)
    {
	image_buffer[7 - i] = (image_buffer[7 - i] & (~1)) | ((unsigned)(data & (1 << i)) >> i);
    }
}

//function to encode an integer size into the least significant bit of each byte in a buffer
Status encode_size_to_lsb(int data,char *image_buffer) 
{
    for(int i = 31 ; i >= 0 ; i--)
    {
	image_buffer[31 - i] = (image_buffer[31 - i] & (~1)) | ((unsigned)(data & (1 << i)) >> i);
    }
}

//function to encode the size of a magic string into the stego image
Status encode_magic_string_size(int size,EncodeInfo *encInfo) 
{
    char str[32];
    //to read 32 bytes from the source image file
    fread(str,1,32,encInfo -> fptr_src_image);
    encode_size_to_lsb(size,str);
    //to encode the size into the least significant bits of the read buffe
    //to write 32 bytes to the stego image file
    fwrite(str,1,32,encInfo -> fptr_stego_image);
    return e_success;
}

//function to encode a magic string into the stego image
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo) 
{
    char str[8];
    
    //to iterate over the characters of the magic string
    for(int i = 0 ; i < strlen(magic_string) ; i++) 
    {
	//to read 8 bytes from the source image file
	fread(str,8,1,encInfo -> fptr_src_image);
        //to encode each character into the least significant bits of the read buffer	
	encode_byte_to_lsb(magic_string[i],str);
	//to write 8 bytes to the stego image file
	fwrite(str,8,1,encInfo -> fptr_stego_image); 
    } 
    return e_success;
}

//function to encode the size of a secret file extension into the stego image
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo) 
{
    char str[32];

    //to read 32 bytes from the source image file
    fread(str,32,1,encInfo -> fptr_src_image);
    //to encode the size into the least significant bits of the read buffer
    encode_size_to_lsb(size,str); 
    //to write 32 bytes to the stego image file
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    char str[8];
    //to run the loop -> strlen(file_extn)
    for(int i = 0 ; i < strlen(file_extn) ; i++)
    {
	//fread -> 8bytes -> encInfo -> fptr_src_image
	fread(str,8,1,encInfo -> fptr_src_image);
	encode_byte_to_lsb(file_extn[i], str);
	//fwrite -> 8bytes -> encInfo -> fptr_stego_image
	fwrite(str,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    char str[32];

    //fread -> 32bytes -> encInfo -> fptr_src_image
    fread(str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size,str);
    //fwrite -> write 32 bytes -> encInfo -> fptr_stego_image
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //to take data from secret.txt file
    char secret_data[encInfo -> size_secret_file];
    //move the file pointer to 0th pointer -> fpte_secret
    fseek(encInfo -> fptr_secret,0,SEEK_SET);
    fread(secret_data,encInfo -> size_secret_file,1,encInfo -> fptr_secret);

    //to encode chars
    char str[8];
    //to run the loop -> size_secret_file times
    for(int i =0;i < strlen(secret_data);i++)
    {
	//fread -> 8bytes -> fptr_src_image
	fread(str,1,8,encInfo -> fptr_src_image);
	encode_byte_to_lsb(secret_data[i], str);
	//fwrite -> 8bytes -> fptr_stego_image
	fwrite(str,1,8,encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[1];
    while(feof(fptr_src_image) != 1)
    {
	//fread -> 1 byte -> fptr_src_image
	fread(str,1,1,fptr_src_image);
	//fwrie -> 1byte -> fptr_stego_image
	fwrite(str,1,1,fptr_stego_image);
    }

    return e_success;
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
    //seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    //to read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    //to return image capacity
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
    printf("INFO: Opening required files\n");
    //Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //to do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n",encInfo -> src_image_fname);
    }

    //secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //to do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n",encInfo -> secret_fname);
    }

    //stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //to do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n",encInfo -> stego_image_fname);
    }
    //no failure return e_success
    return e_success;
}
