#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h" 

int main(int argc, char *argv[])
{
    //structure variable for encoding
    EncodeInfo encInfo;
    //structure variable for decoding 
    DecodeInfo decInfo;
    //to check if command-line arguments are provided
    if(argc == 1) 
    {
	printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
    }
    else
    {
	//to determine the operation type (encode or decode)
	int ret = check_operation_type(argv); 
	//to perform actions based on the operation type
	if(ret == e_encode) 
	{
	    //to validate and read encoding arguments
	    if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
	    {
		if( do_encoding(&encInfo) == e_success )
		{
		    printf("INFO: ## Encoding Done Successfully ##\n");	 
		}
		else
		{
		    printf("INFO: ## Encoding Failed ##\n");	 
		}
	    }
	    else
	    {
		//to validate or reading of encoding arguments failed
		printf("INFO: Read and validate failed\n");
		printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
	    }
	}
 
	if(ret == e_decode)
	{
	    if(read_and_validate_decode_args(argc, argv, &decInfo) == e_success) 
	    {
		if( do_decoding(argv,&decInfo) == e_success)
		{
		    printf("INFO: ## Decoding Done Successfully ##\n");	 
		}
		else
		{
		    printf("INFO: ## Decoding Failed ##\n");	 
		}
	    }
	    else
	    {
		//to validate or reading of decoding arguments failed
		printf("INFO: Read and validate failed\n");
		printf("ERROR: Pass args for \nEncoding : ./a.out -e beautiful.bmp secret.txt\nDecoding : ./a.out -d output.bmp\n");
	    }
	}
    }
}

//function to check the operation type based on command-line arguments
OperationType check_operation_type(char *argv[]) 
{
    if(strcmp(argv[1],"-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}

