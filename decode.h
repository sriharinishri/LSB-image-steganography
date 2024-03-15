/*--------------------------------------------DECODE.H---------------------------------*/
/* Name : Sriharini K
 * Date : 12th Sept 2023
 * Description : AdvC Project -Steganography
 */


#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding encoded file to secret message
 */

#define EXTN ".txt"
#define MAX_BUF_SIZE 8

typedef struct _DecodeInfo
{
	//Stego Image Info
	char *rec_image_fname;
	FILE *fptr_rec_image;
	char image_data[MAX_BUF_SIZE];
	
	//Decoded File Info
	char *dec_fname;
	FILE *fptr_dec;
	int file_size;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_de_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data( int size, DecodeInfo *decInfo);

/*Decode secret file data and copy it in the destination file*/
Status decode_file( char *str, DecodeInfo *decInfo );

/* Decode data from image*/
Status decode_data_from_image(char *data, int size, FILE *fptr_rec_image, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char data, char *image_buffer);

#endif
