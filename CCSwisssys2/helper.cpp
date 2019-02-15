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

std::vector<std::wstring> tokenize(std::wstring in, const std::wstring &tokens) {
	std::vector<std::wstring> result;
	while (in.size()) {
		int index = in.find_first_of(tokens);
		if (index != std::wstring::npos) {
			result.push_back(in.substr(0, index));
			in = in.substr(index + 1);
			if (in.size() == 0) {
				result.push_back(in);
			}
		}
		else {
			result.push_back(in);
			in = L"";
		}
	}
	return result;
}

std::wstring capWords(const std::wstring &s) {
	std::wstring ret = s;

	ret[0] = toupper(ret[0]);

	unsigned i;
	for (i = 1; i < s.length(); ++i) {
		if (ret[i - 1] == L' ') {
			ret[i] = toupper(ret[i]);
		}
		else {
			ret[i] = tolower(ret[i]);
		}
	}

	return ret;
}

bool isNumeric(const CString &cs) {
	if (cs.SpanIncluding(_T("0123456789")) == cs) return true;
	else return false;
}

std::wstring CStringToWString(const CString &cs) {
	LPCWSTR temp1 = cs.GetString();
	return std::wstring(temp1);
}

CString WStringToCString(const std::wstring &ws) {
	return CString(ws.c_str());
}

std::wstring intToString(unsigned v) {
	std::wstringstream ss;
	ss << v;
	return ss.str();
}

int multiWordMatches(const std::vector<std::wstring> &atokens, const std::vector<std::wstring> &btokens) {
	int matches = 0;

	unsigned i, j;
	for (i = 0; i < atokens.size(); ++i) {
		for (j = 0; j < btokens.size(); ++j) {
			if (LevenshteinDistance(atokens[i], btokens[j]) <= 1) {
				++matches;
			}
		}
	}

	return matches;
}

int multiWordMatches(const std::wstring &a, const std::wstring &b) {
	std::vector<std::wstring> atokens = tokenize(a);
	std::vector<std::wstring> btokens = tokenize(b);

	if (atokens.size() > btokens.size()) {
		std::swap(atokens, btokens);
	}

	return multiWordMatches(atokens, btokens);
}

float multiWordScore(const std::wstring &a, const std::wstring &b) {
	std::vector<std::wstring> atokens = tokenize(a);
	std::vector<std::wstring> btokens = tokenize(b);

	if (atokens.size() > btokens.size()) {
		std::swap(atokens, btokens);
	}

	unsigned alen = atokens.size();
	unsigned blen = btokens.size();

	int matches = multiWordMatches(atokens, btokens);

	if (matches > 0) {
		return (float)matches / (alen > blen ? alen : blen);
	}
	else {
		unsigned min_dist = UINT_MAX;

		std::vector<unsigned> bindices(blen);
		std::iota(std::begin(bindices), std::end(bindices), 0);

		do {
			unsigned this_dist = 0;
			for (unsigned i = 0; i < alen; ++i) {
				for (unsigned j = 0; j < alen; ++j) {
					this_dist += LevenshteinDistance(atokens[i], btokens[bindices[i]]);
				}
			}
			min_dist = min(min_dist, this_dist);
		} while (next_combination(bindices.begin(), bindices.begin() + alen, bindices.end()));

		return -min_dist;
	}
}

int LevenshteinDistance(const std::wstring &s, const std::wstring &t) {
	std::wstring supper, tupper;

	supper = toUpper(s);
	tupper = toUpper(t);

	// degenerate cases
	if (supper == tupper) return 0;
	unsigned tlen = (unsigned)t.length();
	unsigned slen = (unsigned)s.length();

	if (slen == 0) return tlen;
	if (tlen == 0) return slen;

	unsigned v0len = tlen + 1;
	unsigned v1len = tlen + 1;

	// create two work vectors of integer distances
	int *v0 = new int[v0len];
	int *v1 = new int[v1len];

	// initialize v0 (the previous row of distances)
	// this row is A[0][i]: edit distance for an empty s
	// the distance is just the number of characters to delete from t
	for (unsigned i = 0; i < v0len; i++)
		v0[i] = i;

	for (unsigned i = 0; i < slen; i++)
	{
		// calculate v1 (current row distances) from the previous row v0

		// first element of v1 is A[i+1][0]
		//   edit distance is delete (i+1) chars from s to match empty t
		v1[0] = i + 1;

		// use formula to fill in the rest of the row
		for (unsigned j = 0; j < tlen; j++)
		{
			int cost = (supper[i] == tupper[j]) ? 0 : 1;
			v1[j + 1] = min(min(v1[j] + 1, v0[j + 1] + 1), v0[j] + cost);
		}

		// copy v1 (current row) to v0 (previous row) for next iteration
		for (unsigned j = 0; j < v0len; j++)
			v0[j] = v1[j];
	}

	return v1[min(slen, tlen)];
}

std::wstring toUpper(const std::wstring &s) {
	std::wstring ret = s;

	for (unsigned j = 0; j < s.size(); ++j) {
		ret[j] = toupper(ret[j]);
	}

	return ret;
}

std::wstring baseToNameWithSub(const std::wstring &base, unsigned sub) {
	wchar_t subid = L'A' + (sub - 1);
	std::wstring subidws(1, subid);
	std::wstring subsecname = base + L" - " + subidws;
	return subsecname;
}

