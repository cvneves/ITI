#include <iostream>

#include "LZW.h"

int main(int argc, char**argv)
{
	LZW lzw;

	if (argc > 3 && string(argv[3]) == string("r"))
	{
		lzw.LoadModel(string(argv[4]));
	}

	lzw.Decode(string(argv[1]), string(argv[2]));

	return 0;
}
