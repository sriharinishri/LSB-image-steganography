/*-------------------------------TEST_ENCODE.C-----------------*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
// files are passed through Command line arguments
int main( int argc, char *argv[] )
{
	//condition to check if atleast one file is passed or not
	if ( argc < 3 )
	{
		printf( "Error : Please pass files through arguments\n" );
		return -1;
	}
	//checking second argument to see if its enoding / decoding
	if ( check_operation_type(argv) == e_encode )
	{
		printf( "Selected encoding \n" );
		//structure pointer
		EncodeInfo encInfo;
		//validating input files
		if ( read_and_validate_encode_args( argv, &encInfo ) == e_success )
		{
			printf( "Read and validate function is success \n" );
			printf( "Started encoding...........\n" );
			//function call for encoding
			if ( do_encoding( &encInfo ) == e_success )
			{
				printf( "-------------Completed encoding-----------\n" );
			}
			else
			{
				printf( "-------------Failed to encode------------\n" );
				return -1;
			}
		}
		else 
		{
			printf( "Read and validate  is a failure \n" );
			return -1;
		}
	}
	else if ( check_operation_type(argv) == e_decode )
	{
		printf( "Selected Decoding \n" );
		//structure pointer
		DecodeInfo decInfo;
		//validating input files
		if ( read_and_validate_decode_args( argv, &decInfo ) == e_success )
		{
			printf( "Read and validate function is success \n" );
			printf( "Started decoding...........\n" );
			//function call for decoding
			if ( do_decoding( &decInfo ) == e_success )
			{
				printf( "-------------Completed decoding-----------\n" );
			}
			else
			{
				printf( "-------------Failed to decode------------\n" );
				return -1;
			}
		}
		else
		{
			printf( "Read and validate  is a failure \nFor decoding pass second arg as .bmp file and pass third arg as .txt file \n" );
			return -1;
		} 
	}
	else 
	{
		printf( "Invalid Input \nFor encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp \nFor decoding: ./a.out -d stego.bmp decoded.txt \n" );

	}
	return 0;
}

//function to check which operation to be done
OperationType check_operation_type( char *argv[] )
{
	if( strcmp(argv[1], "-e" ) == 0 )
		return e_encode;
	else if( strcmp(argv[1], "-d" ) == 0 )
		return e_decode;
	else 
		return e_unsupported;
}
