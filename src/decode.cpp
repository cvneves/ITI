#include <iostream>

#include "LZW.h"

int main(int argc, char**argv)
{
	LZW lzw;

	lzw.Decode(string(argv[1]), string(argv[2]));

// 	string message = "abracadabra";
// 	vector<string> alphabet;
// 	for (int i = 0; i < 256; i++)
// 	{
// 		string tmp;
// 		tmp += i;
// 		alphabet.push_back(tmp);
// 	}
// 
// 	map<string, unsigned int> dict;
// 
// 	for (int i = 0; i < alphabet.size(); i++){
// 		dict[alphabet[i]] = i;
// 	}
// 
// 	for (int i = 0; i < message.size(); i++){
// 		string curr_str = string(1, message[i]);
// 		//cout << curr_str << endl;
// 
// 		int k = i;
// 		while (dict.count(curr_str) && k < message.size())
// 		{
// 			curr_str += message[k+1];
// 			k++;
// 		}
// 		i = k-1;
// 
// 		if(!dict.count(curr_str))
// 			dict[curr_str] = dict.size();
// 
// 	}
// 
// 
// 	map<string, unsigned int>::iterator it;
// 	for (it=dict.begin(); it!=dict.end(); ++it){
// 		cout << (it->first) << " => " << it->second << endl;
// 	}
// 
// 
// 	//cout << dict[3] << endl;
// 
// 
	return 0;
}
