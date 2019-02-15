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

template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last) {
	/* Credits: Mark Nelson http://marknelson.us */
	if ((first == last) || (first == k) || (last == k))
		return false;
	Iterator i1 = first;
	Iterator i2 = last;
	++i1;
	if (last == i1)
		return false;
	i1 = last;
	--i1;
	i1 = k;
	--i2;
	while (first != i1) {
		if (*--i1 < *i2) {
			Iterator j = k;
			while (!(*i1 < *j)) ++j;
			std::iter_swap(i1, j);
			++i1;
			++j;
			i2 = k;
			std::rotate(i1, j, last);
			while (last != j) {
				++j;
				++i2;
			}
			std::rotate(k, i2, last);
			return true;
		}
	}
	std::rotate(first, k, last);
	return false;
}

int listCompareItemInt(int a, int b, bool rev = false);
int listCompareItemFloat(float a, float b, bool rev = false);

std::wstring removeSchoolSubstr(const std::wstring &name);
std::wstring toUpper(const std::wstring &s);
std::vector<std::wstring> tokenize(const std::wstring &in);
std::vector<std::wstring> tokenize(std::wstring in, const std::wstring &tokens);
std::wstring capWords(const std::wstring &s);
bool isNumeric(const CString &cs);
std::wstring CStringToWString(const CString &cs);
CString WStringToCString(const std::wstring &ws);
std::wstring intToString(unsigned v);
float multiWordScore(const std::wstring &a, const std::wstring &b);
int LevenshteinDistance(const std::wstring &s, const std::wstring &t);
int multiWordMatches(const std::wstring &a, const std::wstring &b);
std::wstring baseToNameWithSub(const std::wstring &base, unsigned sub);
