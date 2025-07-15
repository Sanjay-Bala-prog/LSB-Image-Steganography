                                           LSB Image Steganography in C

A mini project in C that implements **LSB (Least Significant Bit) image steganography** to hide and extract secret messages inside BMP image files.  
This project demonstrates how digital data can be concealed within image pixels using bit-level manipulation.

What is LSB Steganography?
               LSB steganography hides data by replacing the least significant bit of image pixels with bits from a secret message or file.  
This technique is simple yet effective and results in minimal visual distortion of the image.

Features:

- Encode secret data (text or files) into a 24-bit BMP image
- Decode and retrieve hidden data from the stego image
- Modular design with separate encode/decode functions
- Command-line interface with validation and error handling

Technologies Used:

- C programming
- Bitwise operations
- File I/O (`fopen`, `fread`, `fwrite`,`fseek`,`ftell`)
- BMP image file structure

How to Compile & Run:
Compile:

gcc *c

For Encoding :
  ./a.out -e source.bmp secret.txt stego.bmp

For Decoding:
   ./a.out -e source.bmp secret.txt stego.bmp
