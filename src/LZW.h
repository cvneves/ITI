#ifndef LZW_H
#define LZW_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <set>

using namespace std;

struct LZW {
	vector<string> alphabet;
	map<string, uint16_t> dict;
	map<uint16_t, string> inv_dict;
	int max_word_size;
	bool use_model = false;

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

	void PrintDict()
	{
		vector<pair<int, string>> temp_v;

		for (auto it=dict.begin(); it!=dict.end(); ++it)
		{
			temp_v.push_back(make_pair(it->second, it->first));
			// cout << (it->first) << " => " << it->second - 97 << endl;
		}
		sort(temp_v.begin(), temp_v.end());
		cout << endl;
		for (auto &v : temp_v)
			cout << v.first << " => " << v.second << endl;
	}

	void Encode(string input_filename, string output_filename);
	void Decode(string input_filename, string output_filename);

	void SaveModel(string model_filename);
	void LoadModel(string model_filename);
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
	int bytes_written = 0;

	set<int> used_codewords;

	if (input_file.is_open() && output_file.is_open())
	{
#ifdef WRITE_COMPRESSED_FILE_TO_DISK
		output_file.write((const char*) &max_word_size, sizeof(char));
#endif
		bytes_written++;

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

			used_codewords.insert(curr_keyword);

			/* Output bits */
			bool byte_written = false;
			for (int bit = max_word_size - 1; bit >= 0; bit--) 
			{
				int out_bit = ((curr_keyword >> bit) & 1);

				if (bits_written == 8)
				{
					// cout << ToBitString((int) byte_to_write, 8);
#ifdef WRITE_COMPRESSED_FILE_TO_DISK
					output_file.write((const char*) &byte_to_write, sizeof(char));
#endif
					bytes_written++;

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
#ifdef WRITE_COMPRESSED_FILE_TO_DISK
				output_file.write((const char*) &byte_to_write, sizeof(char));
#endif
				bytes_written++;

				byte_to_write = 0;
				bits_written = 0;
			}

			if(!use_model && !dict.count(curr_str) && dict.size() < (1 << max_word_size))
				dict[curr_str] = dict.size();

		}

		if (bits_written)
		{
			byte_to_write <<= (8 - bits_written);

			// cout << ToBitString((int) byte_to_write, 8);
#ifdef WRITE_COMPRESSED_FILE_TO_DISK
			output_file.write((const char*) &byte_to_write, sizeof(char));
#endif
			bytes_written++;
		}

		// PrintDict();
	}
	else
	{
		cout << "Error loading the file" << endl;
	}

	cout << "Compressed file size: " << bytes_written << endl;
	cout << "Used codewords: " << used_codewords.size() << endl;
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
							if ( inv_dict.size() < (1 << max_word_size) )
								inv_dict[(uint16_t) inv_dict.size()] = temp;
							// cout << curr_keyword << " - " << temp << endl;
							output_file.write((const char*) temp.data(), temp.size());
						}
						else
						{
							string temp = prev_str + inv_dict[curr_keyword][0];
							if ( inv_dict.size() < (1 << max_word_size) )
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
				if ( inv_dict.size() < (1 << max_word_size) )
					inv_dict[(uint16_t) inv_dict.size()] = temp;
				// cout << curr_keyword << " - " << temp << endl;
				output_file.write((const char*) temp.data(), temp.size());
			}
			else
			{
				string temp = prev_str + inv_dict[curr_keyword][0];
				if ( inv_dict.size() < (1 << max_word_size) )
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

void LZW::SaveModel(string model_filename)
{
	ofstream model_file(model_filename, ios::binary);

	if (model_file.is_open())
	{
		/* File structure:
		 * <K> <total number of patterns>
		 * <code> <code string length> <code string> ... (dict.size() times) */
		model_file.write((char*) &max_word_size, sizeof(int));
		int tmp = dict.size();
		model_file.write((char*) &tmp, sizeof(int));

		for (auto it = dict.begin(); it != dict.end(); ++it)
		{
			model_file.write((char*) &(it->second), sizeof(int));

			int str_length = it->first.size();
			model_file.write((char*) &(str_length), sizeof(int));
			model_file.write((char*) it->first.data(), it->first.size());
		}
	}

	model_file.close();
}

void LZW::LoadModel(string model_filename)
{
	ifstream model_file(model_filename, ios::binary);

	if (model_file.is_open())
	{
		use_model = true;
		dict.clear();

		model_file.read((char*) &max_word_size, sizeof(int));

		int dict_size;
		model_file.read((char*) &dict_size, sizeof(int));


		for (int i = 0; i < dict_size; i++)
		{
			int curr_code;
			model_file.read((char*) &curr_code, sizeof(int));

			int str_length;
			model_file.read((char*) &str_length, sizeof(int));

			char str_buffer[str_length];
			model_file.read((char*) str_buffer, str_length);

			string tmp_str;
			for (int c = 0; c < str_length; c++)
			{
				// cout << (int) tmp_str[c];
				tmp_str += str_buffer[c];
			}
			// cout << endl;

			dict[tmp_str] = curr_code;
		}

		// PrintDict();
	}

	model_file.close();
}

#endif
