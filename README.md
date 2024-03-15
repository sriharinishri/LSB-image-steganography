# LSB-image-steganography
LSB (Least Significant Bit) image steganography is a technique used to hide information within digital images by modifying the least significant bit of pixel values.  By replacing the least significant bit of pixel values with secret data, LSB steganography enables covert communication and data hiding.

// TEST_ENCODE
Code Overview:
The program takes command-line arguments for file operations (encoding or decoding).
It includes header files like encode.h, decode.h, types.h, and common.h for necessary declarations and definitions.
The main function checks the number of arguments passed and determines whether to encode or decode based on the second argument.
It calls functions for encoding or decoding based on the operation type.
Main Function:
Argument Checking:
Verifies if the correct number of arguments is passed.
Determines the operation type (encoding or decoding).
Encoding:
If the operation type is encoding, it initializes an EncodeInfo structure.
Validates input files and proceeds with encoding if successful.
Calls the encoding function and displays appropriate messages based on success or failure.
Decoding:
If the operation type is decoding, it initializes a DecodeInfo structure.
Validates input files and proceeds with decoding if successful.
Calls the decoding function and displays messages accordingly.
Operation Type Check:
Determines the operation type based on the second argument passed.
Usage Instructions:
For encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp
For decoding: ./a.out -d stego.bmp decoded.txt
Recommendations:
Ensure that the necessary header files (encode.h, decode.h, types.h, common.h) contain the required function prototypes and definitions.
Implement the functions read_and_validate_encode_args, do_encoding, read_and_validate_decode_args, and do_decoding for encoding and decoding functionalities.
Handle edge cases and error scenarios to enhance the robustness of the program.
This code structure provides a foundation for implementing steganography operations in C, allowing users to encode and decode data within image files effectively.

