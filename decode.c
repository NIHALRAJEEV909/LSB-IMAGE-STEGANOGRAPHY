#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//function to read and validate command-line arguments
Status read_and_validate_decode_args(int argc, char *argv[],DecodeInfo *decInfo) 
{
    //to check if the correct no. of arguments is provided
    if(argc >= 3 && argc <= 4)
    {
	//to check argv[2] is .bmp file or not
	if( (strstr(argv[2],".") != NULL) && (strcmp(strstr(argv[2],"."),".bmp") == 0) ) 
	{
	    //to set the source image file name
	    decInfo -> src_image_fname = argv[2];
	    //to check if the optional output file name is provided
	    if(argv[3] != NULL) 
	    {
		strcpy(decInfo -> secret_fname,argv[3]); 
	    }
	    else
	    {
		//to set default output file name if not provided
		strcpy(decInfo -> secret_fname,"decoded");
	    }
	}
	else
	{
	    //to return failure if the second argument does not have a ".bmp" extension
	    return e_failure;  
	}
    }
    else
    {
	//to return failure if the number of arguments is not within the expected range
	return e_failure;
    }
    return e_success;
}
//function to perform decoding steps
Status do_decoding(char *argv[],DecodeInfo *decInfo) 
{
    printf("INFO: ## Decoding Procedure Started ##\n"); 
    //Function calls
    printf("INFO: Opening required files\n");
    //to open required files for decoding
    if(open_files_decode(decInfo) == e_success)
    {
	printf("INFO: Decoding Magic String Size\n");
	//to decode magic string size
	if(decode_magic_string_size(decInfo) == e_success) 
	{
	    printf("INFO: Done\n");
	    printf("INFO: Decoding Magic String Signature\n");
	    //to decode magic string
	    if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	    {
		printf("INFO: Done\n");
		printf("INFO: Decoding Output File Extension Size\n");
		//to decode output file extension size
		if(decode_secret_file_extn_size(decInfo) == e_success) 
		{
		    printf("INFO: Done\n");
		    printf("INFO: Decoding Output File Extension \n");
		    //to decode output file extension
		    if(decode_secret_file_extn(argv,decInfo) == e_success)
		    {
			printf("INFO: Done\n");
			//to open the secret file for writing
			if(open_secret_fname(argv,decInfo) == e_success)
			{
			    printf("INFO: Opened %s\n",decInfo -> secret_fname);
			    printf("INFO: Done. Opened all required files\n");
			    //to decode secret file size
			    if(decode_secret_file_size(decInfo) == e_success) 
			    {
				printf("INFO: Decoding %s File Size\n",decInfo -> secret_fname);
				printf("INFO: Done\n");
				//to decode secret file data
				if(decode_secret_file_data(decInfo) == e_success)
				{
				    printf("INFO: Decoding %s File Data\n",decInfo -> secret_fname);
				    printf("INFO: Done\n");
				}
				else
				{
				    //to print error message and return failure
				    printf("decode_secret_file_data is fail\n"); 
				    return e_failure;
				}
			    }
			    else
			    {
				//to print error message and return failure
				printf("decode_secret_file_size failed\n");
				return e_failure;
			    }
			}
			else
			{
			    //to print error message and return failure
			    printf("open_secret_fname failed\n");
			    return e_failure;
			}
		    }
		    else
		    {
			//to print error message and return failure
			printf("decode_secret_file_extn failed\n");
			return e_failure;
		    }
		}
		else
		{
		    //to print error message and return failure
		    printf("decode_secret_file_extn_size failed\n");
		    return e_failure;
		}
	    }
	    else
	    {
		//to print error message and return failure
		printf("decode_magic_string failed\n");
		return e_failure;
	    }
	}
	else
	{
	    //to print error message and return failure
	    printf("decode_magic_string_size failed\n");
	    return e_failure;
	}		      
    }
    else
    {
	//to print error message and return failure
	printf("File failed to open\n");
	return e_failure;
    }
    return e_success;
} 

//function to decode a byte to LSB
char decode_byte_to_lsb(char *image_buffer) 
{
    char ch = 0;
    //loop through each bit in the byte and extract LSB
    for(int i = 7 ; i >= 0 ; i--) 
    {
	ch = ch | (image_buffer[7 - i] & 1) << i;
    }
    return ch;
}

//function to decode a 32-bit size value from LSB-encoded bytes
int decode_size_to_lsb(char *image_buffer) 
{
    int size = 0;
    //loop through each bit in the 32-bit size value and extract LSB
    for(int i = 31 ; i >= 0 ; i--)  
    {
	size = size | ((image_buffer[31 - i] & 1) << i);  
    }
    return size;
}

//function to decode magic string size from the source image
Status decode_magic_string_size(DecodeInfo *decInfo)  
{
    //to move file pointer to the position where magic string size is stored
    fseek(decInfo -> fptr_src_image,54,SEEK_SET);
    char str[32];
    
    fread(str,1,32,decInfo -> fptr_src_image);
    decInfo -> magic_string_size = decode_size_to_lsb(str);
    return e_success;
}

//function to decode magic string from the source image
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo) 
{
    int i;
    char str[8];
    char check_magic[(decInfo -> magic_string_size) + 1];
    //loop through each character in the magic string
    for(i = 0;i < decInfo -> magic_string_size;i++) 
    {
	//to read 8 bytes from the source image and decode each byte
	fread(str,8,1,decInfo -> fptr_src_image); 
	check_magic[i] = decode_byte_to_lsb(str);
    } 
    check_magic[i] = '\0';
    //to compare the decoded magic string with the expected magic string
    if(strcmp(check_magic,magic_string) == 0) 
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//function to decode secret file extension size from the source image
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char str[32];
    //to read 32 bytes from the source image and decode size value
    fread(str,32,1,decInfo -> fptr_src_image); 
    decInfo -> size_secret_file_extn = decode_size_to_lsb(str);
    return e_success;
}

//function to decode secret file extension from the source image
Status decode_secret_file_extn(char *argv[],DecodeInfo *decInfo) 
{
    int i;
    char str[8];
    char check_extn[decInfo -> size_secret_file_extn + 1];
    // Loop through each character in the file extension
    for(i = 0 ; i < decInfo -> size_secret_file_extn ; i++)
    {
	//to read 8 bytes from the source image and decode each byte
	fread(str,8,1,decInfo -> fptr_src_image);
	check_extn[i] = decode_byte_to_lsb(str);
    }
    check_extn[i] = '\0';
   
    //to concatenate the decoded extension with the secret file name
    if( strcmp(check_extn,".c") == 0 || strcmp(check_extn,".txt") == 0 || strcmp(check_extn,".sh") == 0)
    {
	if(strstr(decInfo -> secret_fname,".") != NULL)
	{
	    if(strcmp(strstr(decInfo -> secret_fname,"."),".txt") != 0)
	    {
		strcat(decInfo -> secret_fname,check_extn);
	    }
	}
	else
	{
	    strcat(decInfo -> secret_fname,check_extn);
	}
    }
    else
    {
	return e_failure;
    }
    return e_success;
}

//function to decode secret file size from the source image
Status decode_secret_file_size(DecodeInfo *decInfo)  
{
    char str[32];
    //to read 32 bytes from the source image and decode size value
    fread(str,32,1,decInfo -> fptr_src_image); 
    decInfo -> size_secret_file = decode_size_to_lsb(str);
    return e_success;
}

//function to decode secret file data from the source image and write to the secret file
Status decode_secret_file_data(DecodeInfo *decInfo)  
{
    //array to store secret file data
    char secret_data[decInfo -> size_secret_file];
    //to move the file pointer to 0th pointer -> fptr_secret

    char str[8];
    //loop through each byte in the secret file data
    for(int i = 0;i < decInfo -> size_secret_file;i++) 
    {
	//to read 8 bytes from the source image and decode each byte
	fread(str,8,1,decInfo -> fptr_src_image); 
	secret_data[i] = decode_byte_to_lsb(str);
    }
    //to write the decoded data to the secret file
    fwrite(secret_data,decInfo -> size_secret_file,1,decInfo -> fptr_secret); 
    return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_decode(DecodeInfo *decInfo)
{
    //Src Image file
    //to open the source image file for reading
    decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r");
    //Do Error handling
    //to handle errors if the file fails to open
    if (decInfo -> fptr_src_image == NULL) 
    {
	perror("fopen");
	fprintf(stderr, "EvRROR: Unable to open file %s\n", decInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n",decInfo -> src_image_fname);
    return e_success;
}

Status open_secret_fname(char *argv[],DecodeInfo *decInfo)
{
    //secret file
    //to open the secret file for writing
    decInfo -> fptr_secret = fopen(decInfo->secret_fname, "w");
    //to do error handling
    if (decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
	return e_failure;
    }
    return e_success;
}
