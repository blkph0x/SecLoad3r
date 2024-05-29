# SecLoad3r
Loads things

SecLoad3r.cpp info
How it Works:

Base64 Decoding: The program starts by decoding the Base64-encoded files into their binary representation. It utilizes a function base64_decode to perform the decoding.

Finding Entry Point RVA: It then locates the entry point RVA (Relative Virtual Address) from the Portable Executable (PE) header of the executable. This is done by parsing the DOS and NT headers of the PE file.

Concatenating Files: Files are concatenated into a single executable buffer. The filenames are expected to follow the pattern "part_#.b64", where # represents a sequential number.

Execution: After locating the entry point RVA, the program calculates the entry point address and calls the entry point function, effectively executing the concatenated executable.

How to Use:

Compile the source code using a C++ compiler compatible with Windows.
Ensure that all parts of the split executable are named following the pattern "part_#.b64", where # is the sequential number of the file.
Run the compiled executable.
Enter the number of parts when prompted.
The program will concatenate the files, locate the entry point, and execute the executable.
Dependencies:

Windows operating system
C++ compiler compatible with Windows
Standard C++ libraries
Windows API (for PE header parsing)

SecSplit3r.cpp info
How it Works:

Binary File Splitting: The program starts by reading the binary executable file specified as input. It then divides the file into a specified number of even parts.

Base64 Encoding: Each part of the binary file is encoded into Base64 format using a custom function base64_encode. Base64 encoding is a common method used to represent binary data as ASCII text, making it easier to handle and transmit.

Output Files: The encoded data for each part is written to separate files named according to the pattern "part_#.b64", where # represents a sequential number.

How to Use:

Compile the source code using a C++ compiler.
Run the compiled executable from the command line, specifying the binary executable file as input.
The program will split the binary file into parts and encode each part into Base64 format.
The encoded parts will be saved as separate files in the current directory.
Adjusting Number of Parts:

To adjust the number of parts the binary file is split into, modify the num_parts constant in the code. By default, it is set to 5.

Dependencies:

C++ compiler
Standard C++ libraries
Windows operating system (for the provided code)
