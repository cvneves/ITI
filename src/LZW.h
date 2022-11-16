#ifndef LZW_H
#define LZW_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_set>

using namespace std;

struct LZW {
	vector<string> alphabet;
	map<string, uint16_t> dict;
	map<uint16_t, string> inv_dict;
	int max_word_size;

	void FillDictWithAlphabet()
	{
		for (int i = 0; i < 256; i++)
		{
			string tmp;
			tmp += (char) i;

			dict[tmp] = i;
			inv_dict[i] = tmp;
		}
	}

	LZW() : max_word_size(8) { FillDictWithAlphabet(); }
	LZW(int k) :  max_word_size(k) { FillDictWithAlphabet(); }

	void Encode(string input_filename, string output_filename);
	void Decode(string input_filename, string output_filename);
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

void LZW::Encode(string input_filename, string output_filename)
{
	/* Populate dictionary with alphabet */
	for (int i = 0; i < 256; i++){
		string tmp;
		tmp += (char) i;
		dict[tmp] = i;
	}

	string line;
	ifstream input_file(input_filename, ios::binary);
	ofstream output_file(output_filename, ios::binary);

	if (input_file.is_open() && output_file.is_open())
	{
		output_file.write((const char*) &max_word_size, sizeof(char));

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

			/* Output bits */
			bool byte_written = false;
			for (int bit = max_word_size - 1; bit >= 0; bit--) 
			{
				int out_bit = ((curr_keyword >> bit) & 1);

				if (bits_written == 8)
				{
					// cout << ToBitString((int) byte_to_write, 8);
					output_file.write((const char*) &byte_to_write, sizeof(char));

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
				// cout << ToBitString((int) byte_to_write, 8);
				output_file.write((const char*) &byte_to_write, sizeof(char));

				byte_to_write = 0;
				bits_written = 0;
			}

			if(!dict.count(curr_str) && dict.size() < (1 << max_word_size))
				dict[curr_str] = dict.size();

		}

		if (bits_written)
		{
			byte_to_write <<= (8 - bits_written);

			// cout << ToBitString((int) byte_to_write, 8);
			output_file.write((const char*) &byte_to_write, sizeof(char));
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
	output_file.close();

}

void LZW::Decode(string input_filename, string output_filename)
{
	string line;
	ifstream input_file(input_filename, ios::binary);
	ofstream output_file(output_filename, ios::binary);

	if (input_file.is_open() && output_file.is_open())
	{
		input_file.read((char*) &max_word_size, sizeof(char));
		// cout << ToBitString(max_word_size, 8) << "\n";

		uint16_t curr_keyword = 0;
		int bits_read = 0;

		int bits_written = 0;

		char curr_byte;

		string curr_str;
		string prev_str;
		uint16_t prev_keyword;


		int keywords_read = 0;
		while (!input_file.eof())
		{
			input_file.read(&curr_byte, sizeof(char));

			if (input_file.eof())
				break;

			// cout << bits_read << "\n";
			for (int bit = 8 - 1; bit >= 0; bit--)
			{
				if (bits_read == max_word_size)
				{
					/***************** KEYWORD SUCESSFULLY READ *******************/
					// cout << (char) curr_keyword << endl;

					keywords_read++;

					if (keywords_read == 1)
					{
						// prev_str = inv_dict[curr_keyword];
						prev_keyword = curr_keyword;
						// cout << curr_keyword << " - " << inv_dict[curr_keyword][0] << endl;
						output_file.write((const char*) &inv_dict[curr_keyword][0], sizeof(char));
					}
					else
					{
						string prev_str = inv_dict[prev_keyword];

						if (!inv_dict.count(curr_keyword))
						{
							string temp = prev_str + prev_str[0];
							inv_dict[(uint16_t) inv_dict.size()] = temp;
							// cout << curr_keyword << " - " << temp << endl;
							output_file.write((const char*) temp.data(), temp.size());
						}
						else
						{
							string temp = prev_str + inv_dict[curr_keyword][0];
							inv_dict[(uint16_t) inv_dict.size()] = temp;
							// cout << curr_keyword << " - " << inv_dict[curr_keyword] << endl;
							output_file.write((const char*) inv_dict[curr_keyword].data(), inv_dict[curr_keyword].size());
						}

						prev_keyword = curr_keyword;
					}

						curr_keyword = 0;
						bits_read = 0;
				}

				curr_keyword <<= 1;
				curr_keyword |= ((curr_byte >> bit) & 1);

				bits_read++;
			}

		}

		if (bits_read == max_word_size)
		{
			/***************** KEYWORD SUCESSFULLY READ *******************/
			// cout << (char) curr_keyword << endl;
			keywords_read++;

			string prev_str = inv_dict[prev_keyword];

			curr_keyword <<= (max_word_size - bits_read);
			// cout << ToBitString(curr_keyword, max_word_size) << endl;

			if (!inv_dict.count(curr_keyword))
			{
				string temp = prev_str + prev_str[0];
				inv_dict[(uint16_t) inv_dict.size()] = temp;
				// cout << curr_keyword << " - " << temp << endl;
				output_file.write((const char*) temp.data(), temp.size());
			}
			else
			{
				string temp = prev_str + inv_dict[curr_keyword][0];
				inv_dict[(uint16_t) inv_dict.size()] = temp;
				// cout << curr_keyword << " - " << inv_dict[curr_keyword] << endl;
				output_file.write((const char*) inv_dict[curr_keyword].data(), inv_dict[curr_keyword].size());
			}
		}
	}
	else
	{
		cout << "Error loading the file" << endl;
	}

	input_file.close();
	output_file.close();
}

#endif
