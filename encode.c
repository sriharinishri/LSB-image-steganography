//encode.c
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
//function to validate whether proper files are passed or not
Status read_and_validate_encode_args( char *argv[], EncodeInfo *encInfo )
{
	//to check whether the source file is a bmp file
	if ( strcmp( strstr( argv[2], "." ), ".bmp" ) == 0 )
		encInfo -> src_image_fname = argv[2];
	else 
		return e_failure;
	//to check whether secret file is txt file
	if ( strcmp( strstr( argv[3], "." ), ".txt" ) == 0 )
		encInfo -> secret_fname	= argv[3];
	else 
		return e_failure;
	//if no file to store encoded data is passed, create one
	if ( argv[4] != NULL )
		encInfo -> stego_image_fname = argv[4];
	else 
		encInfo ->stego_image_fname = "stego.bmp";
	return e_success;
}
/* Function Definitions */

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
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
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
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open source file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
   	fprintf(stderr, "ERROR: Unable to open secret file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open stego file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//function to check whether source file is capable of storing secret file
Status check_capacity( EncodeInfo *encInfo )
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image );
	encInfo -> size_secret_file = get_file_size( encInfo -> fptr_secret );

	if ( encInfo -> image_capacity > ( 16 + 32 +32 +32 + ( encInfo -> size_secret_file * 8 )))
		return e_success;
	else 
		return e_failure;
}

//function to get file size
uint get_file_size ( FILE *fptr )
{
	fseek( fptr, 0, SEEK_END );
	return ftell( fptr );
}

//header file of source is copied to the encoded file
Status copy_bmp_header ( FILE *fptr_src_image,  FILE *fptr_dest_image )
{
	char str[54];
	fseek( fptr_src_image, 0, SEEK_SET );
	fread( str, 54, 1, fptr_src_image );
	fwrite( str, 54, 1, fptr_dest_image );
	return e_success;
}

//encoding magic string to the destination file
Status encode_magic_string( char *magic_string, EncodeInfo *encInfo )
{
	encode_data_to_image( magic_string, 2, encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo );
	return e_success;
}

//secret data is encoded with source file and stored in destination file
Status encode_data_to_image( char *data, int size, FILE *fptr_src_image, FILE *fptr_dest_image, EncodeInfo *encInfo )
{
	for ( int i = 0 ; i < size ; i++ )
	{
		fread( encInfo -> image_data, 8, 1, fptr_src_image );
		encode_byte_to_lsb( data[i], encInfo -> image_data );
		fwrite( encInfo -> image_data, 8, 1, fptr_dest_image );
	}
	return e_success;
}

//one byte of secret file is hidden under 8 bytes of source file
Status encode_byte_to_lsb( char data, char *image_buffer )
{
	unsigned int mask = 1 << 7;
	for ( int i = 0 ; i < 8 ; i++ )
	{
		image_buffer[i] =  ( image_buffer[i] & 0XFE ) | (( data & mask ) >> ( 7 - i ));
		mask = mask >> 1;
	}
	return e_success;
} 

//function to encode extn size in destination file
Status encode_extn_size( int size, FILE *fptr_src_image, FILE *fptr_stego_image )
{
	char str[32];
	fread( str, 32, 1, fptr_src_image );
	encode_size_to_lsb( size, str );
	fwrite( str, 32, 1, fptr_stego_image );
	return e_success;
}

//function to encode extn size in bits in destination file
Status encode_size_to_lsb( int size, char *buffer )
{
	unsigned int mask = 1 << 31;
	for ( int i = 0 ; i < 32 ; i++ )
	{
		buffer[i] =  ( buffer[i] & 0XFE ) | (( size & mask ) >> ( 31 - i ));
		mask = mask >> 1;
	}
	return e_success;
}

//function to encode file extn
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo )
{
	encode_data_to_image( file_extn, 4, encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo );
	return e_success;
}

//function to encode secret file size
Status encode_secret_file_size( int size, EncodeInfo *encInfo )
{
	char str[32];	
	fread( str, 32, 1, encInfo -> fptr_src_image );
	encode_size_to_lsb( size, str );
	fwrite( str, 32, 1, encInfo -> fptr_stego_image );
	return e_success;
}

//function to encode secret file data into the destination file
Status encode_secret_file_data ( EncodeInfo *encInfo )
{
	char str[encInfo -> size_secret_file];
	fseek( encInfo -> fptr_secret, 0, SEEK_SET );
	fread( str, encInfo -> size_secret_file, 1,encInfo ->fptr_secret );
	encode_data_to_image( str, encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo );
	return e_success;
}

//remaining bytes of source is copied to destination file
Status copy_remaining_img_data( FILE *fptr_src, FILE *fptr_dest )
{
	char ch;
	while ( fread( &ch, 1, 1, fptr_src ) > 0 )
	{
		fwrite( &ch, 1, 1, fptr_dest );
	}
	return e_success;
}

//function for encoding
Status do_encoding( EncodeInfo *encInfo )
{
	//function call to open files
	if ( open_files( encInfo ) == e_success )
	{
		printf( "Open files function was successful\n" );
		//function call to check if bmp file is enough to store secret data
		if ( check_capacity( encInfo ) == e_success )
		{
			printf( "Check capacity is a success \n" );
			//function call to copy bmp header
			if ( copy_bmp_header( encInfo -> fptr_src_image , encInfo -> fptr_stego_image ) == e_success )
			{
				printf( "Copied bmp header successfully \n" );
				//function call to encode magic string
				if ( encode_magic_string( MAGIC_STRING, encInfo ) == e_success )
				{
					printf( "Encoded magic string \n" );
					strcpy( encInfo -> extn_secret_file, strstr( encInfo ->secret_fname, "." ));
					//function call to encode file extn size
					if ( encode_extn_size( strlen( encInfo -> extn_secret_file ), encInfo -> fptr_src_image, encInfo -> fptr_stego_image ) == e_success)
					{
						printf( "Encoded Extn size \n" );
						//fuction call to enocde file extn
						if ( encode_secret_file_extn( encInfo -> extn_secret_file, encInfo ) == e_success )
						{
							printf( "Encoded file extn  \n" );
							//function to encode secret file size
							if ( encode_secret_file_size( encInfo -> size_secret_file, encInfo) == e_success )
							{
								printf( "Encoded secret file size \n" );
								//function to encode secret file data
								if ( encode_secret_file_data( encInfo ) == e_success )
								{
									printf( "Encoded secret file data \n" );
									//function call to copy remaining data
									if ( copy_remaining_img_data( encInfo -> fptr_src_image, encInfo -> fptr_stego_image ) == e_success)
									{
										printf( "Copied remaining data \n" );

									}
									else
									{
										printf(" Failed to copy remaining data \n" );
										return e_failure;
									}	
								}
								else
								{
									printf( "Failed to encode secret file data \n" );
									return e_failure;
								}
							}
							else
							{
								printf( "Failed to encode secret file size \n" );
								return e_failure;
							}
						}
						else
						{
							printf( "Failed to encode file extn \n" );
							return e_failure;
						}

					}
					else
					{
						printf( "Failed to encode extn size \n" );
						return e_failure;
					}
				}
				else 
				{
					printf( "Failed to encode magic string \n" );
					return e_failure;
				}
			}
			else
			{
				printf( "Failed to copy bmp header \n" );
				return e_failure;
			}
		}
		else
		{
			printf( "Check capacity is a failure \n" );
			return e_failure;
		}
	}
	else 
	{
		printf( "Open files is a failure\n" );
		return e_failure;
	}
	return e_success;
}
