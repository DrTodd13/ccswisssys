#pragma once

#include <vector>
#include <algorithm>
#include <numeric>

template <class T>
int listCompareItem(T &a, T &b, bool rev = false) {
	if (a < b) return rev ? 1 : -1;
	if (a > b) return rev ? -1 : 1;
	return 0;
}

int listCompareItemInt(int a, int b, bool rev = false);
int listCompareItemFloat(float a, float b, bool rev = false);

std::wstring removeSchoolSubstr(const std::wstring &name);
std::wstring toUpper(const std::wstring &s);
std::vector<std::wstring> tokenize(const std::wstring &in);
