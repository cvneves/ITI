#include <iostream>

#include "LZW.h"

int main(int argc, char**argv)
{
	LZW lzw(stoi(argv[3]));

	if (argc > 4 && string(argv[4]) == string("r"))
	{
		lzw.LoadModel(string(argv[5]));
	}

	lzw.Encode(string(argv[1]), string(argv[2]));

	if (argc > 4 && string(argv[4]) == string("w"))
	{
		lzw.SaveModel(string(argv[5]));
	}

	return 0;
}
