#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

struct LZW {
	vector<string> alphabet;
	map<string, uint16_t> dict;
	const int max_word_size;

	LZW(int k) :  max_word_size(k) {}

	void Compress(string input_filename, string output_filename);
};

string ToBitString(int num, int len)
{
	string temp;
	for (int i = 0; i < len; i++)
	{
		temp += (char) ((num >> i) & 1) + 48;
	}

	reverse(temp.begin(), temp.end());

	return temp;
}

void LZW::Compress(string input_filename, string output_filename)
{
	/* Populate dictionary with alphabet */
	for (int i = 0; i < 256; i++){
		string tmp;
		tmp += (char) i;
		dict[tmp] = i;
	}

	string line;
	ifstream input_file(input_filename);

	if (input_file.is_open())
	{
		char curr_byte;
		input_file.read(&curr_byte, sizeof(char));

		unsigned char byte_to_write = 0;
		int bits_written = 0;

		while (!input_file.eof())
		{
			string curr_str;
			curr_str += curr_byte;

			uint16_t curr_keyword = (uint16_t) curr_byte;

			while (dict.count(curr_str) && !input_file.eof())
			{
				input_file.read(&curr_byte, sizeof(char));

				curr_keyword = dict[curr_str];

				if (input_file.eof())
					break;

				curr_str += curr_byte;
			}

			// cout << curr_keyword << ' ';

			/* Output bits */
			bool byte_written = false;
			for (int bit = max_word_size - 1; bit >= 0; bit--) 
			{
				int out_bit = ((curr_keyword >> bit) & 1);

				if (bits_written == 8)
				{
					// cout << (int) byte_to_write << endl;
					cout << ToBitString((int) byte_to_write, 8);
					byte_written = true;

					byte_to_write = 0;
					bits_written = 0;
				}

				byte_to_write <<= 1;
				byte_to_write |= out_bit;
				bits_written++;
			}

			if (!byte_written && !bits_written)
			{
				// cout << (int) byte_to_write << endl;
				cout << ToBitString((int) byte_to_write, 8);
				byte_to_write = 0;
				bits_written = 0;
			}

			if(!dict.count(curr_str) && dict.size() < (1 << max_word_size))
				dict[curr_str] = dict.size();

		}

		if (bits_written)
		{
			byte_to_write <<= (8 - bits_written);
			cout << ToBitString((int) byte_to_write, 8);
		}

		// vector<pair<int, string>> temp_v;

		// for (auto it=dict.begin(); it!=dict.end(); ++it)
		// {
		// 	temp_v.push_back(make_pair(it->second, it->first));
		// 	// cout << (it->first) << " => " << it->second - 97 << endl;
		// }
		// sort(temp_v.begin(), temp_v.end());
		// cout << endl;
		// for (auto &v : temp_v)
		// 	cout << v.first << " => " << v.second << endl;
	}
	else
	{
		cout << "Error loading the file" << endl;
	}

	input_file.close();
}

int main(int argc, char**argv)
{
	LZW lzw(stoi(argv[2]));

	lzw.Compress(string(argv[1]), "output/out.txt");

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
