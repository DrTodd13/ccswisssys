
// CCSwisssys2Doc.cpp : implementation of the CCCSwisssys2Doc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CCSwisssys2.h"
#endif

#include "CCSwisssys2Doc.h"

#include <propkey.h>
#include <assert.h>
#include <chrono>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCCSwisssys2Doc

IMPLEMENT_DYNCREATE(CCCSwisssys2Doc, CDocument)

BEGIN_MESSAGE_MAP(CCCSwisssys2Doc, CDocument)
	ON_COMMAND(ID_OPTIONS_TOURNAMENTDATE, &CCCSwisssys2Doc::OnOptionsTournamentdate)
END_MESSAGE_MAP()


// CCCSwisssys2Doc construction/destruction

CCCSwisssys2Doc::CCCSwisssys2Doc()
{
	// TODO: add one-time construction code here

}

CCCSwisssys2Doc::~CCCSwisssys2Doc()
{
}

BOOL CCCSwisssys2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	ratings_file = "";
	constant_contact_file = "";
	school_code_file = "";
	restricted_file = "";
	sections.clear();
	mrplayers.clear();
	rated_players.clear();
	save_school_corrections = true;

	return TRUE;
}

std::wstring REG_STR = std::wstring(L"Registered");

// CCCSwisssys2Doc serialization

void Serialize(std::map<std::wstring, std::wstring> &the_map, CArchive& ar) {
	if (ar.IsStoring())
	{
		ar << the_map.size();
		for (auto iter = the_map.begin(); iter != the_map.end(); ++iter) {
			ar << WStringToCString(iter->first) << WStringToCString(iter->second);
		}
	}
	else
	{
		size_t the_size;
		ar >> the_size;
		for (size_t i = 0; i < the_size; ++i) {
			CString a, b;
			ar >> a >> b;
			the_map.insert(std::pair<std::wstring, std::wstring>(CStringToWString(a),CStringToWString(b)));
		}
	}
}

void Serialize(std::map<std::wstring, CorrectedFields> &the_map, CArchive& ar) {
	if (ar.IsStoring())
	{
		ar << the_map.size();
		for (auto iter = the_map.begin(); iter != the_map.end(); ++iter) {
			ar << WStringToCString(iter->first);
			iter->second.Serialize(ar);
		}
	}
	else
	{
		size_t the_size;
		ar >> the_size;
		for (size_t i = 0; i < the_size; ++i) {
			CString a, b;
			ar >> a;
			CorrectedFields cf;
			cf.Serialize(ar);
			the_map.insert(std::pair<std::wstring, CorrectedFields>(CStringToWString(a), cf));
		}
	}
}

void CCCSwisssys2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << ratings_file;
		ar << constant_contact_file;
		ar << school_code_file;
		ar << restricted_file;
		sections.Serialize(ar);
		mrplayers.Serialize(ar);
		::Serialize(saved_school_corrections, ar);
		school_codes.Serialize(ar);
		ar << save_school_corrections;
		ar << m_tournament_date;
		force_sections.Serialize(ar);
		noshows.Serialize(ar);
		parent_sections.Serialize(ar);
	}
	else
	{
		sections.clear();
		mrplayers.clear();

		ar >> ratings_file;
		ar >> constant_contact_file;
		ar >> school_code_file;
		ar >> restricted_file;
		sections.Serialize(ar);
		mrplayers.Serialize(ar);
		::Serialize(saved_school_corrections, ar);
		school_codes.Serialize(ar);
		ar >> save_school_corrections;
		ar >> m_tournament_date;
		force_sections.Serialize(ar);
		noshows.Serialize(ar);
		parent_sections.Serialize(ar);
	}
}

std::wstring removeSubstring(const std::wstring &in, const std::wstring &pattern) {
	std::wstring ret = in;
	std::string::size_type i = ret.find(pattern);
	if (i != std::string::npos) {
		ret.erase(i, pattern.length());
	}
	return ret;
}

void removeSubstringInplace(std::wstring &in, const std::wstring &pattern) {
	std::string::size_type i = in.find(pattern);
	if (i != std::string::npos) {
		in.erase(i, pattern.length());
	}
}

std::wstring removeSchoolSubstr(const std::wstring &name) {
	std::wstring ret = name;
	removeSubstringInplace(ret, L" ELEMENTARY SCHOOL");
	removeSubstringInplace(ret, L" ELEMENTARY");
	removeSubstringInplace(ret, L" MIDDLE SCHOOL");
	removeSubstringInplace(ret, L" HIGH SCHOOL");
	removeSubstringInplace(ret, L" JR HIGH");
	removeSubstringInplace(ret, L" JR. HIGH");
	removeSubstringInplace(ret, L" JR HS");
	removeSubstringInplace(ret, L" MS");
	removeSubstringInplace(ret, L" HS");
	removeSubstringInplace(ret, L" SCHOOL");
	removeSubstringInplace(ret, L" K-8");
	return ret;
}

SCHOOL_TYPE getSchoolType(wchar_t grade) {
	switch (grade) {
	case 'A': return ELEMENTARY;
	case 'B': return ELEMENTARY;
	case 'C': return ELEMENTARY;
	case 'D': return ELEMENTARY;
	case 'E': return ELEMENTARY;
	case 'F': return ELEMENTARY;
	case 'G': return MIDDLE;
	case 'H': return MIDDLE;
	case 'I': return MIDDLE;
	case 'J': return HIGH;
	case 'K': return HIGH;
	case 'L': return HIGH;
	case 'M': return HIGH;
	case 'N': return ADULT;
	default:
		assert(0);
		return UNKNOWN;
	}
}

CString getGradeString(wchar_t grade) {
	switch (grade) {
	case 'A': return CString("A - Kindergarten");
	case 'B': return CString("B - 1st Grade");
	case 'C': return CString("C - 2nd Grade");
	case 'D': return CString("D - 3rd Grade");
	case 'E': return CString("E - 4th Grade");
	case 'F': return CString("F - 5th Grade");
	case 'G': return CString("G - 6th Grade");
	case 'H': return CString("H - 7th Grade");
	case 'I': return CString("I - 8th Grade");
	case 'J': return CString("J - 9th Grade");
	case 'K': return CString("K - 10th Grade");
	case 'L': return CString("L - 11th Grade");
	case 'M': return CString("M - 12th Grade");
	case 'N': return CString("N - Adult");
	default:
		assert(0);
		return CString("Unknown grade.");
	}
}

CString getGradeStringShort(wchar_t grade) {
	switch (grade) {
	case 'A': return CString("K");
	case 'B': return CString("1");
	case 'C': return CString("2");
	case 'D': return CString("3");
	case 'E': return CString("4");
	case 'F': return CString("5");
	case 'G': return CString("6");
	case 'H': return CString("7");
	case 'I': return CString("8");
	case 'J': return CString("9");
	case 'K': return CString("10");
	case 'L': return CString("11");
	case 'M': return CString("12");
	case 'N': return CString("Adult");
	default:
		assert(0);
		return CString("Unknown grade.");
	}
}

CString getSectionTypeString(int type) {
	switch (type) {
	case SWISS: return CString("Swiss");
	case ROUND_ROBIN: return CString("Round Robin");
	default:
		assert(0);
		return CString("Unknown section type.");
	}
}

class FindFieldOperator {
public:
	virtual bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const = 0;
};

class FindFieldByString : public FindFieldOperator {
protected:
	std::wstring match;
public:
	FindFieldByString(const std::wstring &m) : match(m) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return field == match;
	}
};

class FindNwsrsIdField : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
public:
	FindNwsrsIdField(const std::map<std::wstring, unsigned> &m) : nwsrs_map(m) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return field.length() == 8 && !hasNumeric(field.substr(0,3)) && nwsrs_map.find(getLastFour(field)) != nwsrs_map.end();
	}
};

class FindPhoneNumber : public FindFieldOperator {
public:
	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return field.length() == 10 && isNumeric(field);
	}
};

class FindEmail : public FindFieldOperator {
public:
	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return field.find(L"@") != std::wstring::npos;
	}
};

class FindYear : public FindFieldOperator {
protected:
	std::wstring currentYear;
public:
	FindYear() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		struct tm *parts = std::localtime(&now_c);
		std::wstringstream wss;
		wss << parts->tm_year + 1900;
		currentYear = wss.str();
	}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return field.find(currentYear) != std::wstring::npos;
	}
};

class FindInSet : public FindFieldOperator {
protected:
	const std::set<std::wstring> &the_set;
	int exclusion_column;
public:
	FindInSet(int ec, const std::set<std::wstring> &s) : exclusion_column(ec), the_set(s) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		return (index != exclusion_column) && (the_set.find(field) != the_set.end());
	}
};

class FindFirstName : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
	const std::vector<Player> &rated_players;
	int id_field;
public:
	FindFirstName(const std::map<std::wstring, unsigned> &m, const std::vector<Player> &r, int i) : nwsrs_map(m), rated_players(r), id_field(i) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		auto idres = nwsrs_map.find(getLastFour(all_fields[id_field]));
		if (idres == nwsrs_map.end()) return false;

		return toUpper(field) == toUpper(rated_players[idres->second].first_name);
	}
};

class FindLastName : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
	const std::vector<Player> &rated_players;
	int id_field;
public:
	FindLastName(const std::map<std::wstring, unsigned> &m, const std::vector<Player> &r, int i) : nwsrs_map(m), rated_players(r), id_field(i) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		auto idres = nwsrs_map.find(getLastFour(all_fields[id_field]));
		if (idres == nwsrs_map.end()) return false;

		return toUpper(field) == toUpper(rated_players[idres->second].last_name);
	}
};

class FindUscfId : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
	const std::vector<Player> &rated_players;
	int id_field;
public:
	FindUscfId(const std::map<std::wstring, unsigned> &m, const std::vector<Player> &r, int i) : nwsrs_map(m), rated_players(r), id_field(i) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		if (field.length() == 0) return false;

		auto idres = nwsrs_map.find(getLastFour(all_fields[id_field]));
		if (idres == nwsrs_map.end()) return false;
		if (rated_players[idres->second].uscf_id.length() == 0) return false;

		return field == rated_players[idres->second].uscf_id;
	}
};

bool isGrade(const std::wstring &s) {
	if (s == L"K") { return true; }
	if (s == L"k") { return true; }
	if (s == L"1") { return true; }
	if (s == L"2") { return true; }
	if (s == L"3") { return true; }
	if (s == L"4") { return true; }
	if (s == L"5") { return true; }
	if (s == L"6") { return true; }
	if (s == L"7") { return true; }
	if (s == L"8") { return true; }
	if (s == L"9") { return true; }
	if (s == L"10") { return true; }
	if (s == L"11") { return true; }
	if (s == L"12") { return true; }
	return false;
}

class FindGradeField : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
	const std::vector<Player> &rated_players;
	int id_field;
public:
	FindGradeField(const std::map<std::wstring, unsigned> &m, const std::vector<Player> &r, int i) : nwsrs_map(m), rated_players(r), id_field(i) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		if (!isGrade(field)) return false;
		auto idres = nwsrs_map.find(getLastFour(all_fields[id_field]));
		if (idres == nwsrs_map.end()) return false;

		return getGradeCode(field) == rated_players[idres->second].grade;
	}
};

class FindSchoolField : public FindFieldOperator {
protected:
	const std::map<std::wstring, unsigned> &nwsrs_map;
	const std::vector<Player> &rated_players;
	int id_field;
	const AllCodes &school_codes;
public:
	FindSchoolField(const std::map<std::wstring, unsigned> &m, const std::vector<Player> &r, int i, const AllCodes &s) : nwsrs_map(m), rated_players(r), id_field(i), school_codes(s) {}

	bool operator()(int index, const std::wstring &field, const std::vector<std::wstring> &all_fields) const {
		if (field.length() == 0) return false;
		auto idres = nwsrs_map.find(getLastFour(all_fields[id_field]));
		if (idres == nwsrs_map.end()) return false;

		if (school_codes.find(field) != -1) return true;
		if (school_codes.findCodeFromSchoolExact(field) != L"") return true;
		// FIX FIX FIX...could try to do something here about searching for a very close school name match.
		return false;
	}
};

wchar_t getGradeCode(const std::wstring &s) {
	std::wstring up = toUpper(s);

	if (up == L"K") { return L'A'; }
	if (up == L"KINDERGARTEN") { return L'A'; }
	if (up == L"1") { return L'B'; }
	if (up == L"1ST") { return L'B'; }
	if (up == L"FIRST") { return L'B'; }
	if (up == L"2") { return L'C'; }
	if (up == L"2ND") { return L'C'; }
	if (up == L"SECOND") { return L'C'; }
	if (up == L"3") { return L'D'; }
	if (up == L"3RD") { return L'D'; }
	if (up == L"THIRD") { return L'D'; }
	if (up == L"4") { return L'E'; }
	if (up == L"4TH") { return L'E'; }
	if (up == L"FOURTH") { return L'E'; }
	if (up == L"5") { return L'F'; }
	if (up == L"5TH") { return L'F'; }
	if (up == L"FIFTH") { return L'F'; }
	if (up == L"6") { return L'G'; }
	if (up == L"6TH") { return L'G'; }
	if (up == L"SIXTH") { return L'G'; }
	if (up == L"7") { return L'H'; }
	if (up == L"7TH") { return L'H'; }
	if (up == L"SEVENTH") { return L'H'; }
	if (up == L"8") { return L'I'; }
	if (up == L"8TH") { return L'I'; }
	if (up == L"EIGHTH") { return L'I'; }
	if (up == L"9") { return L'J'; }
	if (up == L"9TH") { return L'J'; }
	if (up == L"NINTH") { return L'J'; }
	if (up == L"10") { return L'K'; }
	if (up == L"10TH") { return L'K'; }
	if (up == L"TENTH") { return L'K'; }
	if (up == L"11") { return L'L'; }
	if (up == L"11TH") { return L'L'; }
	if (up == L"ELEVENTH") { return L'L'; }
	if (up == L"12") { return L'M'; }
	if (up == L"12TH") { return L'M'; }
	if (up == L"TWELTH") { return L'M'; }
	if (up == L"A") { return L'N'; }
	if (up == L"ADULT") { return L'N'; }
	if (up.size() > 1) {
		std::wstring first_char = up.substr(0, up.length() - 1);
		return getGradeCode(first_char);
	}
	throw UnrecognizedGradeCode();
	//MessageBox(NULL, _T("Don't know how to convert some grade string into grade code."), _T("ERROR"), MB_OK);
	//exit(-1);
}

bool doAdultCheck(const std::vector<std::wstring> &fields, std::set<int> *empty_player_fields) {
	for (auto iter = empty_player_fields->begin(); iter != empty_player_fields->end(); ++iter) {
		if (fields[*iter] != L"") {
			return true;
		}
	}
	return false;
}

//#define DEBUG_FFWO

void collectNames(std::vector< std::vector<std::wstring> > &cc,
	int regcheck,
	std::set<std::wstring> &firsts,
	std::set<std::wstring> &lasts,
	std::set<int> *empty_player_fields,
	int first_index,
	int last_index) {

	unsigned i;
	for (i = 0; i < cc.size(); ++i) {
		if (regcheck != -1 && cc[i][regcheck] != REG_STR) continue;
		if (doAdultCheck(cc[i], empty_player_fields)) {
			firsts.insert(cc[i][first_index]);
			lasts.insert(cc[i][last_index]);
		}
	}
}

/*
Scans the whole constant contact information for fields having exactly the text "Registered".
If only one such field exists then it becomes the field.  If there are more than one then if one predominates the other
by like 95% then it becomes the field but a warning should be issued.
*/
int findFieldWithOperator(
	std::wofstream &normal_log,
	std::vector< std::vector<std::wstring> > &cc, 
	const FindFieldOperator &matcher, 
	int regcheck = -1, 
	std::set<int> *empty_player_fields = NULL, 
	double cutoff=0.95,
    bool only_players=true) {

	unsigned i,j;
	std::map<int, int> register_fields;

	for (i = 0; i < cc.size(); ++i) {
		if (regcheck != -1 && cc[i][regcheck] != REG_STR) continue;
		if (empty_player_fields != NULL) {
			if (only_players == doAdultCheck(cc[i], empty_player_fields)) continue;
		}

		for (j = 0; j < cc[i].size(); ++j) {
			if (matcher(j, cc[i][j], cc[i])) {
#ifdef DEBUG_FFWO
				normal_log << "findFieldWithOperator match " << cc[i][j] << " " << j << " for player " << cc[i][0] << " " << cc[i][1] << std::endl;
#endif
				auto rfiter = register_fields.find(j);
				if (rfiter == register_fields.end()) {
					register_fields.insert(std::pair<int, int>(j, 1));
				}
				else {
					rfiter->second++;
				}
			}
		}
	}

	int count = 0;
	for (auto iter = register_fields.begin(); iter != register_fields.end(); ++iter) {
#ifdef DEBUG_FFWO
		normal_log << "findFieldWithOperator found " << iter->second << " possibilities for field " << iter->first << std::endl;
#endif
		count += iter->second;
	}
	for (auto iter = register_fields.begin(); iter != register_fields.end(); ++iter) {
		double ratio = iter->second / (double)count;
#ifdef DEBUG_FFWO
		normal_log << "findFieldWithOperator ratio " << ratio << " for field " << iter->first << std::endl;
#endif
		if (ratio > cutoff) {
			return iter->first;
		}
	}
	return -1;
}

std::set<int> findEmptyPlayerFields(std::vector< std::vector<std::wstring> > &cc, 
	const std::map<std::wstring, unsigned> &nwsrs_map, 
	int regcheck, int nwsrsid, std::wofstream &normal_log) {

	unsigned i, j;
	std::set<int> ret;

    // Add all fields except registration and nwsrs id as potential all empty for players.
	for (i = 0; i < cc[0].size(); ++i) {
		if (i != regcheck && i != nwsrsid) {
			ret.insert(i);
		}
	}

	std::map<int, int> register_fields;

	for (i = 0; i < cc.size(); ++i) {
		if (cc[i][regcheck] != REG_STR) continue;
		//normal_log << "checking " << i << " " << cc[i][0] << " " << cc[i][1] << " " << cc[i][nwsrsid] << std::endl;
		if (nwsrs_map.find(getLastFour(cc[i][nwsrsid])) == nwsrs_map.end()) continue;
		//normal_log << "is a player" << std::endl;

		// For every field for registered players, if the field is not empty then remove it from the
		// set of fields that are always empty for players.
		for (j = 0; j < cc[i].size(); ++j) {
			if (cc[i][j] != L"") {
				//normal_log << "erase " << i << " " << j << cc[i][0] << " " << cc[i][1] << " [" << cc[i][j] << "]" << std::endl;
				ret.erase(j);
			}
		}
	}

	return ret;
}

std::vector< ConstantContactEntry > load_constant_contact_file(const std::wstring &filename,
	const std::map<std::wstring, unsigned> &nwsrs_map,
	const std::map<std::wstring, unsigned> &uscf_map,
	const std::vector<Player> &rated_players,
	const AllCodes &school_codes,
	std::wofstream &normal_log) {

	auto ccret = load_csvw_file(filename, true);

	DynamicLocations dynamic_locations;

	std::vector< ConstantContactEntry > ret;

#ifdef DEBUG_FFWO
	normal_log << "Finding registration string." << std::endl;
#endif

	dynamic_locations[REGISTERED] = findFieldWithOperator(normal_log, ccret, FindFieldByString(std::wstring(REG_STR)));
	if (dynamic_locations[REGISTERED] == -1) {
		MessageBox(NULL, _T("Could not automatically detect REGISTERED field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

#ifdef DEBUG_FFWO
	normal_log << "Finding NWSRS ID Field." << std::endl;
#endif

	dynamic_locations[STUDENT_NWSRS_ID] = findFieldWithOperator(normal_log, ccret, FindNwsrsIdField(nwsrs_map), dynamic_locations[REGISTERED], NULL, 0.85);
	if (dynamic_locations[STUDENT_NWSRS_ID] == -1) {
		MessageBox(NULL, _T("Could not automatically detect NWSRS ID field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

#ifdef DEBUG_FFWO
	normal_log << "Finding empty player fields." << std::endl;
#endif

	std::set<int> *empty_player_fields = new std::set<int>;
	*empty_player_fields = findEmptyPlayerFields(ccret, nwsrs_map, dynamic_locations[REGISTERED], dynamic_locations[STUDENT_NWSRS_ID], normal_log);
#ifdef DEBUG_FFWO
	for (auto iter = empty_player_fields->begin(); iter != empty_player_fields->end(); ++iter) {
		normal_log << "Empty player field " << *iter << std::endl;
	}
#endif

#ifdef DEBUG_FFWO
	normal_log << "Finding first name field." << std::endl;
#endif

	dynamic_locations[FIRST_NAME] = findFieldWithOperator(normal_log, ccret, FindFirstName(nwsrs_map, rated_players, dynamic_locations[STUDENT_NWSRS_ID]), dynamic_locations[REGISTERED], empty_player_fields, 0.5);
	if (dynamic_locations[FIRST_NAME] == -1) {
		MessageBox(NULL, _T("Could not automatically detect FIRST NAME field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

#ifdef DEBUG_FFWO
	normal_log << "Finding last name field." << std::endl;
#endif

	dynamic_locations[LAST_NAME] = findFieldWithOperator(normal_log, ccret, FindLastName(nwsrs_map, rated_players, dynamic_locations[STUDENT_NWSRS_ID]), dynamic_locations[REGISTERED], empty_player_fields, 0.5);
	if (dynamic_locations[LAST_NAME] == -1) {
		MessageBox(NULL, _T("Could not automatically detect LAST NAME field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	std::set<std::wstring> firsts, lasts;
	collectNames(ccret, dynamic_locations[REGISTERED], firsts, lasts, empty_player_fields, dynamic_locations[FIRST_NAME], dynamic_locations[LAST_NAME]);

#ifdef DEBUG_FFWO
	normal_log << "Finding USCF ID field." << std::endl;
#endif

	dynamic_locations[STUDENT_USCF_ID] = findFieldWithOperator(normal_log, ccret, FindUscfId(nwsrs_map, rated_players, dynamic_locations[STUDENT_NWSRS_ID]), dynamic_locations[REGISTERED], empty_player_fields);
//	normal_log << "Finding grade code" << std::endl;
	dynamic_locations[STUDENT_GRADE] = findFieldWithOperator(normal_log, ccret, FindGradeField(nwsrs_map, rated_players, dynamic_locations[STUDENT_NWSRS_ID]), dynamic_locations[REGISTERED], empty_player_fields, 0.6);
	if (dynamic_locations[STUDENT_GRADE] == -1) {
		MessageBox(NULL, _T("Could not automatically detect GRADE field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[STUDENT_SCHOOL] = findFieldWithOperator(normal_log, ccret, FindSchoolField(nwsrs_map, rated_players, dynamic_locations[STUDENT_NWSRS_ID], school_codes), dynamic_locations[REGISTERED], empty_player_fields, 0.6);
	if (dynamic_locations[STUDENT_SCHOOL] == -1) {
		MessageBox(NULL, _T("Could not automatically detect SCHOOL field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[ADULT_EMAIL] = findFieldWithOperator(normal_log, ccret, FindEmail(), dynamic_locations[REGISTERED], empty_player_fields, 0.6, false);
	if (dynamic_locations[ADULT_EMAIL] == -1) {
		MessageBox(NULL, _T("Could not automatically detect SCHOOL field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[ADULT_PHONE] = findFieldWithOperator(normal_log, ccret, FindPhoneNumber(), dynamic_locations[REGISTERED], empty_player_fields, 0.6, false);
	if (dynamic_locations[ADULT_PHONE] == -1) {
		MessageBox(NULL, _T("Could not automatically detect SCHOOL field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[RESPONSIBLE_FIRST] = findFieldWithOperator(normal_log, ccret, FindInSet(dynamic_locations[FIRST_NAME], firsts), dynamic_locations[REGISTERED], empty_player_fields, 0.6);
	if (dynamic_locations[RESPONSIBLE_FIRST] == -1) {
		MessageBox(NULL, _T("Could not automatically detect SCHOOL field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[RESPONSIBLE_LAST] = findFieldWithOperator(normal_log, ccret, FindInSet(dynamic_locations[LAST_NAME], lasts), dynamic_locations[REGISTERED], empty_player_fields, 0.6);
	if (dynamic_locations[RESPONSIBLE_LAST] == -1) {
		MessageBox(NULL, _T("Could not automatically detect SCHOOL field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	dynamic_locations[REGISTRATION_DATETIME] = findFieldWithOperator(normal_log, ccret, FindYear(), dynamic_locations[REGISTERED], empty_player_fields, 0.6);
	if (dynamic_locations[REGISTRATION_DATETIME] == -1) {
		MessageBox(NULL, _T("Could not automatically detect registration datetime field in constant contact registration file."), _T("ERROR"), MB_OK);
		return ret;
	}

	unsigned i;
	for (i = 0; i < ccret.size(); ++i) {
		if (ccret[i][dynamic_locations[REGISTERED]] == REG_STR) {
			ret.push_back(ConstantContactEntry(ccret[i], dynamic_locations, empty_player_fields));
		}
	}
	return ret;
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CCCSwisssys2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CCCSwisssys2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CCCSwisssys2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCCSwisssys2Doc diagnostics

#ifdef _DEBUG
void CCCSwisssys2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCCSwisssys2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

bool CCCSwisssys2Doc::loadRatingsFile() {
	rated_players.clear();

	std::wifstream infile(ratings_file);
	unsigned player_index = 0;
	Player p;

	if (!infile) {
		return false;
	}

	while (!infile.eof()) {
		infile >> p;
		if (!infile.eof()) {
			rated_players.push_back(p);
			nwsrs_map.insert(std::pair<std::wstring, unsigned>(p.getFullId(), player_index));
			nwsrs_four_map.insert(std::pair<std::wstring, unsigned>(p.id, player_index));
			if (!p.uscf_id.empty()) {
				uscf_map.insert(std::pair<std::wstring, unsigned>(p.uscf_id, player_index));
			}
			++player_index;
		}
	}

	return true;
}

bool isAlpha(std::wstring &s) {
	for (unsigned i = 0; i < s.length(); ++i) {
		if (!isalpha(s[i])) {
			return false;
		}
	}
	return true;
}

bool isNumeric(const std::wstring &s) {
	return (s.find_first_not_of(L"0123456789") == std::string::npos);
}

bool hasNumeric(std::wstring &s) {
	return (s.find_first_of(L"0123456789") != std::string::npos);
}

std::wstring getSchoolTypeStr(SCHOOL_TYPE &st) {
	switch (st) {
	case ELEMENTARY: return L"ES";
	case MIDDLE: return L"MS";
	case HIGH: return L"HS";
	case ADULT: return L"ADULT";
	default: return L"UNKNOWN";
	}
}

// CCCSwisssys2Doc commands

CTime CCCSwisssys2Doc::getValidTournamentDate() {
	while (m_tournament_date.GetTime() == 0) {
		OnOptionsTournamentdate();
	}
	return m_tournament_date;
}

void CCCSwisssys2Doc::OnOptionsTournamentdate()
{
	CTime temp = m_tournament_date;
	if (temp.GetTime() == 0) {
		temp = CTime::GetCurrentTime();
	}
	DateSelector2 ds(&temp);
	if (ds.DoModal() == IDOK) {
		m_tournament_date = temp;
	}
}

CArchive & operator<<(CArchive &ar, const std::wstring &s) {
	ar << WStringToCString(s);
	return ar;
}

CArchive & operator>>(CArchive &ar, std::wstring &s) {
	CString ins;
	ar >> ins;
	s = CStringToWString(ins);
	return ar;
}

CArchive & operator<<(CArchive &ar, Section &s) {
	s.Serialize(ar);
	return ar;
}

CArchive & operator>>(CArchive &ar, Section &s) {
	s.Serialize(ar);
	return ar;
}

bool SectionPointerRatingReverseSort(const Section *a, const Section *b) {
	return a->upper_rating_limit > b->upper_rating_limit;
}
