#include <iostream>

#include "LZW.h"

int main(int argc, char**argv)
{
	LZW lzw(stoi(argv[3]));

	lzw.Encode(string(argv[1]), string(argv[2]));

	return 0;
}
