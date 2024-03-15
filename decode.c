/* -------------------------------DECODE.C---------------------*/
/* Name : Sriharini K
 * Date : 12th Sept 2023
 * Description : AdvC Project -Steganography
 */
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//validating files passed for decoding
Status read_and_validate_decode_args( char *argv[], DecodeInfo *decInfo )
{
	//checking if received decoded file is a bmp file
	if ( strcmp( strstr( argv[2], "." ), ".bmp" ) == 0 )
		decInfo -> rec_image_fname = argv[2];
	else 
		return e_failure;
	//if decoded destination is passed, checking it for txt file else creating a txt file
	if ( argv[3] != NULL )
	{
		if ( strcmp(strstr( argv[3], "." ), ".txt" ) == 0 )
			decInfo -> dec_fname = argv[3];
		else 
			return e_failure;
	}
	else 
		decInfo -> dec_fname = "output.txt";
	return e_success;
}

Status open_de_files( DecodeInfo *decInfo )
{
	//received decoded file
	decInfo -> fptr_rec_image = fopen(decInfo -> rec_image_fname, "r" );
	//Do Error handling
	if ( decInfo -> fptr_rec_image == NULL )
	{
		perror("fopen");
		fprintf(stderr, "ERROR: unable to open received (encoded) file %s \n", decInfo->rec_image_fname );
		return e_failure;
	}

	//Decoded secret file
	decInfo->fptr_dec = fopen(decInfo -> dec_fname , "w" );
	//Do Error handling
	if ( decInfo -> fptr_dec == NULL )
	{
		perror("fopen");
		fprintf(stderr, "ERROR: unable to open deccoded secret file %s \n", decInfo->dec_fname );
	}

	//No failure return e_success
	return e_success;
} 

//decoding magic string from received encoded file
Status decode_magic_string( char *magic_string,  DecodeInfo *decInfo )
{
	//length of magic string
	int size = (int) strlen(magic_string);
	fseek( decInfo -> fptr_rec_image, 54, SEEK_SET );
	if( decode_data_from_image( magic_string, size, decInfo -> fptr_rec_image , decInfo ) != e_success )
		return e_failure;
	return e_success;
}

//taking  bytes of encoded file to get a byte of magic string
Status decode_data_from_image ( char *data, int size, FILE *fptr_rec_image, DecodeInfo *decInfo )
{
	for ( int i = 0 ; i < size ; i++ )
	{
		fread( decInfo -> image_data , 8, 1, fptr_rec_image );
		if ( decode_byte_from_lsb( data[i], decInfo-> image_data ) != e_success ) 
			return e_failure;
	}
	return e_success;
}

//taking bits from 8 lsbs to create one byte
Status decode_byte_from_lsb( char data, char *image_buffer )
{
	char ch = 0;
	for ( int i = 0 ; i < 8 ; i++ )
	{
		ch = ( image_buffer[i] & 1 ) | ch ;
		if( i != 7 ) ch = ch << 1;
	} 
	if ( ch == data ) return e_success;
	else return e_failure;
}

//extension of file is decoded in this function
Status decode_secret_file_extn ( char *file_extn , DecodeInfo *decInfo )
{
	int size = (int) strlen(file_extn);
	fseek( decInfo -> fptr_rec_image, 32, SEEK_CUR );
	if( decode_data_from_image( file_extn, size, decInfo -> fptr_rec_image , decInfo ) != e_success )
		return e_failure;
	return e_success;
}	

//function to decode secret file size 
Status decode_secret_file_size( DecodeInfo *decInfo )
{
	char str[32];
	int size = 0;
	fread( str, 32, 1, decInfo -> fptr_rec_image );
	for ( int i = 0 ; i < 32 ; i++ )
	{
		size = ( str[i] & 1 ) | size;
		if( i != 31 ) size = size << 1; 
	}	
	decInfo -> file_size = size;
	return e_success;
}

//function to decode secret file data
Status decode_secret_file_data( int size, DecodeInfo *decInfo )
{
	char str[8];
	for ( int i = 0 ; i < size ; i++ )
	{
		fread( str, 8, 1, decInfo -> fptr_rec_image );
		decode_file( str, decInfo );
		fwrite( str, 1, 1, decInfo -> fptr_dec );
	}
	return e_success;
}

//function to decode a single byte of secret file from 8 bytes of encoded file
Status decode_file( char *str, DecodeInfo *decInfo )
{
	char ch = 0;
	for ( int i = 0 ; i < 8 ; i++ )
	{
		ch = ( str[i] & 1 ) | ch ;
		if( i != 7 ) ch = ch << 1;
	} 
	str[0] = ch;
	return e_success;
}

//function for encoding which contains all function calls
Status do_decoding( DecodeInfo *decInfo )
{
	//function call to check status of files
	if ( open_de_files( decInfo ) == e_success )
	{
		printf( "Open files function was successful \n" );
		//function call to decode magic string
		if ( decode_magic_string ( MAGIC_STRING, decInfo ) == e_success )
		{
			printf( "Magic String Decoded and this file contains secret text \n" );
			//function call to decode secret file extn
			if ( decode_secret_file_extn ( EXTN , decInfo ) == e_success ) 
			{
				printf( "Decoding secret file extension is successful \n");
				//function call to decode secret file size
				if ( decode_secret_file_size ( decInfo ) == e_success )
				{
					printf( "Decoding Secret file size is successful \nSecret file Size is %d \n",  decInfo -> file_size  );
					//function to decode secret file data
					if ( decode_secret_file_data ( decInfo -> file_size, decInfo ) == e_success )
					{
						printf( "Decoding secret file data is successful \n" );
						return e_success;
					}
					else
					{
						printf( "Decoding secret file data is failed \n" );
						return e_failure;
					}	
				}
				else
				{
					printf( "Decoding secret file size is failed \n" );
					return e_failure;
				}	
			}
			else
			{
				printf( "Secret File extension is not a valid .txt file \n" );
				return e_failure;
			}
		}
		else
		{
			printf( "Magic string Mismatch and this file has no secret text \n" );
			return e_failure;
		}
	}
	else
	{
		printf( "Open files function was not successful \n" );
		return e_failure;
	}
	return e_success;
}
