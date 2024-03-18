# Password Cracker

Based on the challenge of the same name on CodingChallenges.fyi. My (current) attempt to create a cracker for MD5-hashed passwords using C++.

## Compiling

I haven't made a makefile for this yet, but you can compile this using the following command (after unzipping/cloning this repo): 
`g++ -std=c++17 test.cpp mypasscrack.cpp -o [OUTPUT_FILE_NAME].o`

Alternatively, you can use the provided binaries, `test.o` for Windows 10, or `testOSX_x86-64.o` for x64 (Intel-based) MacOS.