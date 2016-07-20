#include "stdafx.h"
#include "DatFile.h"
#include <sstream>

std::wstring trim(const std::wstring& str, const std::wstring& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::wstring::npos)
		return L""; // no content

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::wstring read_fixed(std::wistream &in, unsigned length) {
	wchar_t buf[128] = L"";
	in.read(buf, length);
	return trim(std::wstring(buf));
}

wchar_t read_fixed_char(std::wistream &in, unsigned length) {
	std::wstring temp = read_fixed(in, length);
	return temp[0];
}

unsigned read_fixed_unsigned(std::wistream &in, unsigned length) {
	std::wstring temp = read_fixed(in, length);
	return _ttoi(temp.c_str());
}

std::wistream& operator >> (std::wistream &in, Player &p) {
	p.last_name = read_fixed(in, 25);
	p.first_name = read_fixed(in, 25);
	p.school_code = read_fixed(in, 4);
	p.grade = read_fixed_char(in, 2);
	p.id = read_fixed(in, 6);
	p.nwsrs_rating = read_fixed_unsigned(in, 5);
	p.num_games = read_fixed_unsigned(in, 5);
	p.games_ytd = read_fixed_unsigned(in, 5);
	p.highest_rating_this_year = read_fixed_unsigned(in, 6);
	p.lowest_rating = read_fixed_unsigned(in, 6);
	p.start_rating = read_fixed_unsigned(in, 6);
	p.kirks_code = read_fixed_unsigned(in, 5);
	p.last_action_date = read_fixed(in, 9);
	p.rating_basis = read_fixed_char(in, 2);
	p.uscf_last_name = read_fixed(in, 25);
	p.uscf_first_name = read_fixed(in, 25);
	p.uscf_id = read_fixed(in, 9);
	p.uscf_exp_date = read_fixed(in, 9);
	p.uscf_rating = read_fixed(in, 6);
	unsigned uscf_rating = _ttoi(p.uscf_rating.c_str());
	if (uscf_rating == 0) {
		p.uscf_rating = L"";
	}
	else {
		std::wstringstream ss;
		ss << uscf_rating;
		p.uscf_rating = ss.str();
	}
//	p.uscf_rating = read_fixed_unsigned(in, 6);
	p.uscf_prov_codes = read_fixed(in, 5);
	p.uscf_rating_date = read_fixed(in, 9);
	p.state = read_fixed(in, 3);
	wchar_t c;
	//std::cout << "New line code is " << (int)'\n' << std::endl;
	do {
		in.get(c);
		//std::cout << "Found char " << c << " " << (int)c << std::endl;
	} while (c != '\n' && !in.eof());

	return in;
}

std::wostream& operator<<(std::wostream &os, Player &p) {
	os << p.last_name << " ";
	os << p.first_name << " ";
	os << p.school_code << " ";
	os << p.grade << " ";
	os << p.id << " ";
	os << p.nwsrs_rating << " ";
	os << p.num_games << " ";
	os << p.games_ytd << " ";
	os << p.highest_rating_this_year << " ";
	os << p.lowest_rating << " ";
	os << p.start_rating << " ";
	os << p.kirks_code << " ";
	os << p.last_action_date << " ";
	os << p.rating_basis << " ";
	os << p.uscf_last_name << " ";
	os << p.uscf_first_name << " ";
	os << p.uscf_id << " ";
	os << p.uscf_exp_date << " ";
	os << p.uscf_rating << " ";
	os << p.uscf_prov_codes << " ";
	os << p.uscf_rating_date << " ";
	os << p.state;

	return os;
}

