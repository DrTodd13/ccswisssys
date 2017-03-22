#include "stdafx.h"
#include "tokenize_csv.h"

std::vector< std::vector<std::wstring> > load_csvw_file(const std::wstring &filename, bool skip_header) {
	std::wifstream infile(filename);
	std::vector< std::vector<std::wstring> > ret;

	if (!infile) {
		return ret;
	}

	unsigned cur_line = 1;

	while (!infile.eof()) {
		std::wstring whole_line;
		getline(infile, whole_line);
		if (infile.eof()) break;

		++cur_line;

		//std::cout << "=========================================================================" << std::endl;
		//std::cout << whole_line << std::endl;
		if (skip_header) {
			//std::cout << "Skipping header" << std::endl;
			skip_header = false;
			continue;
		}

		std::vector<std::wstring> elems;

		unsigned i;
		bool in_quote = false;
		std::wstring cur_str = L"";
		bool line_done = false;
		while (!line_done) {
			for (i = 0; i < whole_line.size(); ++i) {
				wchar_t cur = whole_line[i];

				if (cur == '"') {
					in_quote = !in_quote;
				}

				if (in_quote) {
					cur_str += cur;
				}
				else {
					if (cur == ',') {
						elems.push_back(cur_str);
						cur_str = L"";
					}
					else {
						cur_str += cur;
					}
				}
			}
			if (in_quote) {
				getline(infile, whole_line);
				if (infile.eof()) {
					//std::cerr << "File eof while inside a quote." << std::endl;
					exit(-1);
				}
				++cur_line;
			}
			else {
				line_done = true;
			}
		}
		elems.push_back(cur_str);

		ret.push_back(elems);
	}

	return ret;
}


std::vector<std::wstring> tokenize_csv(const std::wstring &s) {
	std::vector<std::wstring> elems;

	unsigned i;
	bool in_quote = false;
	std::wstring cur_str = L"";
	for (i = 0; i < s.size(); ++i) {
		wchar_t cur = s[i];

		if (cur == '"') {
			in_quote = !in_quote;
		}

		if (in_quote) {
			cur_str += cur;
		}
		else {
			if (cur == ',') {
				elems.push_back(cur_str);
				cur_str = L"";
			}
			else {
				cur_str += cur;
			}
		}
	}
	elems.push_back(cur_str);

	return elems;
}
