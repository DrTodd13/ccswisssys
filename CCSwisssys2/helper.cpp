#include "stdafx.h"
#include <sstream>
#include <iterator>
#include "helper.h"
#include <vector>

int listCompareItemInt(int a, int b, bool rev) {
	if (a < b) return rev ? 1 : -1;
	if (a > b) return rev ? -1 : 1;
	return 0;
}

int listCompareItemFloat(float a, float b, bool rev) {
	if (a < b) return rev ? 1 : -1;
	if (a > b) return rev ? -1 : 1;
	return 0;
}

std::vector<std::wstring> tokenize(const std::wstring &in) {
	std::wistringstream iss(in);
    return std::vector<std::wstring>((std::istream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t> >(iss)),
		                              std::istream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t> >());
}