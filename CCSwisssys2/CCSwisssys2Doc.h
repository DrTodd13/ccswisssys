
// CCSwisssys2Doc.h : interface of the CCCSwisssys2Doc class
//


#pragma once

#include <vector>
#include "tokenize_csv.h"
//#include <Pathcch.h>
#include <Shlwapi.h>
#include <set>
#include <algorithm>
#include "DateSelector2.h"
#include "helper.h"

typedef int SECTION_TYPE;

#define	SWISS 0
#define ROUND_ROBIN 1

enum SCHOOL_TYPE {
	ELEMENTARY=1,
	MIDDLE=2,
	HIGH=4,
	ADULT=8,
	UNKNOWN=16
};

CString getGradeString(wchar_t grade);
CString getGradeStringShort(wchar_t grade);
SCHOOL_TYPE getSchoolType(wchar_t grade);
std::wstring getSchoolTypeStr(SCHOOL_TYPE &st);
CString getSectionTypeString(int type);
std::wstring CStringToWString(const CString &cs);
CString WStringToCString(const std::wstring &ws);
std::wstring toUpper(const std::wstring &s);
wchar_t getGradeCode(const std::wstring& s);

class UnrecognizedGradeCode {};

enum {
	FIRST_NAME = 0,
	LAST_NAME = 1,
	STUDENT_SCHOOL = 2,
	STUDENT_GRADE = 3,
	STUDENT_NWSRS_ID = 4,
	STUDENT_USCF_ID = 5,
	REGISTERED = 6,
	RESPONSIBLE_FIRST = 7,
	RESPONSIBLE_LAST = 8,
	ADULT_EMAIL = 9,
	ADULT_PHONE = 10,
	REGISTRATION_DATETIME = 11
};


#define NUM_DYNAMIC_LOCATIONS 12

class DynamicLocations {
protected:
	int locations[NUM_DYNAMIC_LOCATIONS];
public:
	DynamicLocations() {
		unsigned i;
		for (i = 0; i < NUM_DYNAMIC_LOCATIONS; ++i) {
			locations[i] = -1;
		}
	}

	std::set<int> get_used_columns(void) const {
		std::set<int> ret;
		unsigned i;
		for (i = 0; i < NUM_DYNAMIC_LOCATIONS; ++i) {
			if (locations[i] != -1) {
				ret.insert(locations[i]);
			}
		}
		return ret;
	}

	int operator [](int i) const { return locations[i]; }
	int& operator [](int i) { return locations[i]; }
};

bool doAdultCheck(const std::vector<std::wstring>& fields, std::set<int>* empty_player_fields);

extern std::wstring REG_STR;

class ConstantContactEntry {
protected:
	std::vector<std::wstring> fields;
	DynamicLocations m_locations;
	std::set<int>* m_empty_player_fields;
	void valid(void) const {
		//if (fields.size() < 18) {
			//std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
		//	exit(-1);
		//}
	}
public:
	ConstantContactEntry(const std::vector<std::wstring>& f, const DynamicLocations& locations, std::set<int>* epf) : fields(f), m_locations(locations), m_empty_player_fields(epf) { valid(); }

	std::wstring getField(unsigned index) const { valid(); return fields[index]; }
	std::wstring getFirstName(void)   const { valid(); return fields[m_locations[FIRST_NAME]]; }
	std::wstring getLastName(void)    const { valid(); return fields[m_locations[LAST_NAME]]; }
	std::wstring getEmail(void)       const { valid(); return fields[m_locations[ADULT_EMAIL]]; }
	std::wstring getPhone(void)       const { valid(); return fields[m_locations[ADULT_PHONE]]; }
	std::wstring getAdultFirst(void)  const { valid(); return fields[m_locations[RESPONSIBLE_FIRST]]; }
	std::wstring getAdultLast(void)   const { valid(); return fields[m_locations[RESPONSIBLE_LAST]]; }
	std::wstring getRegistrationDate(void)   const { valid(); return fields[m_locations[REGISTRATION_DATETIME]]; }

	/*
	std::wstring getAddress1(void)    const { valid(); return fields[4]; }
	std::wstring getCity(void)        const { valid(); return fields[5]; }
	std::wstring getState(void)       const { valid(); return fields[6]; }
	std::wstring getZip(void)         const { valid(); return fields[7]; }
	std::wstring getCellPhone(void)   const { valid(); return fields[8]; }
	std::wstring getVolunteer(void)   const { valid(); return fields[9]; }
	std::wstring getQuestions(void)   const { valid(); return fields[10]; }
	*/
	bool didAdultCheck(void)          const {
		valid();
		return doAdultCheck(fields, m_empty_player_fields);
	}
	std::wstring getSchool(void)      const { valid(); return fields[m_locations[STUDENT_SCHOOL]]; }
	std::wstring getGrade(void)       const { valid(); return fields[m_locations[STUDENT_GRADE]]; }
	//std::wstring getSection(void)     const { valid(); return fields[13]; }
	std::wstring getNwsrsId(void)     const { valid(); return fields[m_locations[STUDENT_NWSRS_ID]]; }
	//std::wstring getNwsrsRating(void) const { valid(); return fields[m_locations[STUDENT_NWSRS_RATING]]; }
	std::wstring getUscfId(void)      const {
		valid();
		if (m_locations[STUDENT_USCF_ID] >= 0) {
			return fields[m_locations[STUDENT_USCF_ID]];
		}
		else return L"";
	}
	//std::wstring getUscfRating(void)  const { valid(); return fields[m_locations[STUDENT_USCF_RATING]]; }

	bool isRegistered(void) const {
		valid();
		return fields[m_locations[REGISTERED]] == REG_STR;
	}

	bool isParent(void) const {
		return didAdultCheck();
		//		return didAdultCheck() && getSchool().empty() && getGrade().empty() && getNwsrsId().empty();
	}

	bool isPlayer(void) const {
		return !didAdultCheck();
	}

	wchar_t getGradeCode(void) const {
		return ::getGradeCode(getGrade());
	}
};

class SectionPlayerInfo {
public:
	int cc_file_index;
	CString last_name;
	CString first_name;
	CString full_id;
	int rating;
	wchar_t grade;
	CString school;
	CString school_code;
	CString uscf_id;
	CString uscf_rating;
	CString uscf_expr;
	int subsection;
	std::wstring notes;
	bool unrated;
	int cc_rating;
	CString adult_first, adult_last, adult_email, adult_phone;
	CString registration_date;
//	ConstantContactEntry cce;

	std::wstring getUnique() {
		return CStringToWString(last_name + first_name + grade + school + adult_first + adult_last);
	}

	SectionPlayerInfo(
		int ccfi, 
		const CString &ln, 
		const CString &fn, 
		const CString &fi, 
		int r, wchar_t g, 
		const CString &s, 
		const CString &sc, 
		const CString &uscfid, 
		const CString &uscfrating, 
		const CString &uscfexpr,
		const std::wstring &n, 
		bool u, 
		int ccr, 
		const CString &af,
		const CString &al,
		const CString &rdt,
		int sub=1) : 
		cc_file_index(ccfi), 
		last_name(ln), 
		first_name(fn), 
		full_id(fi), 
		rating(r), 
		grade(g), 
		school(s), 
		school_code(sc), 
		uscf_id(uscfid), 
		uscf_rating(uscfrating), 
		uscf_expr(uscfexpr),
		subsection(sub), 
		notes(n), 
		unrated(u), 
		cc_rating(ccr),
	    adult_first(af),
	    adult_last(al),
	    registration_date(rdt) {}
};

struct SortByRatingDescending
{
	bool operator()(const SectionPlayerInfo &L, const SectionPlayerInfo &R) { return L.rating > R.rating; }
};

class Section {
public:
	CString name;
	int lower_rating_limit;
	int upper_rating_limit;
	wchar_t lower_grade_limit;
	wchar_t upper_grade_limit;
	SECTION_TYPE sec_type;
	std::vector<SectionPlayerInfo> players;
	unsigned num_subsections;
	bool uscf_required;
	unsigned which_computer;
	CString time_control;
	CString starting_board_number;
	CString playing_room;
	unsigned num_rounds;
	int parent_section;
	int num_split;

	Section() : name(""), lower_rating_limit(0), upper_rating_limit(3000), lower_grade_limit('A'), upper_grade_limit('N'), sec_type(SWISS), num_subsections(1), uscf_required(false), which_computer(1), time_control(""), starting_board_number(""), playing_room(""), num_rounds(5), parent_section(-1), num_split(-1) {}
	
	void copyResectioningFields(const Section &other) {
		lower_rating_limit = other.lower_rating_limit;
		upper_rating_limit = other.upper_rating_limit;
		sec_type = other.sec_type;
		players = other.players;
	}

	int getLastBoardNumber(void) const {
		int cur_start = _ttoi(starting_board_number);
		if (players.size() <= 3) {
			return cur_start;
		}
		else {
			int boards_needed = (players.size() - 2) / 2;
			return cur_start + boards_needed;
		}
	}

	std::wstring getPrintName(unsigned subsec) const {
		if (num_subsections > 1) {
			return baseToNameWithSub(CStringToWString(name), subsec);
		}
		else {
			return CStringToWString(name);
		}
	}

	void makeSubsections() {
		if (num_subsections == 1) return;

		std::map<CString, unsigned> sibling_count;
		std::vector<CString> to_split;
		unsigned skip_siblings = 0;

		for (unsigned i = 0; i < players.size(); ++i) {
			auto miter = sibling_count.find(players[i].last_name);
			if (miter == sibling_count.end()) {
				sibling_count.insert(std::pair<CString, unsigned>(players[i].last_name, 1));
			}
			else {
				miter->second++;
				to_split.push_back(players[i].last_name);
			}
		}

		for (auto sciter = sibling_count.begin(); sciter != sibling_count.end(); ++sciter) {
			if (sciter->second == 1) continue;
			for (unsigned i = skip_siblings; i < players.size(); ++i) {
				if (players[i].last_name == sciter->first) {
					std::iter_swap(players.begin() + skip_siblings++, players.begin() + i);
				}
			}
		}

		// Overall structure is to sort by rating.
		// Then for the most part go through that sorted list and put each player in the next
		// next subsection modulo the number of subsections.
		// We'll try to keep even number in subsections.
		std::sort(players.begin() + skip_siblings, players.end(), SortByRatingDescending());
		unsigned num_players = (unsigned)players.size();
		// Get the number of players per subsection under a strict division.
		unsigned player_per_subsection = num_players / num_subsections;
		// If that number is odd then subtract one so that the base number each section gets is even.
		if (player_per_subsection % 2 == 1) {
			player_per_subsection -= 1;
		}
		unsigned first_allocation = player_per_subsection * num_subsections;

		unsigned i;
		// The simple part where we allocation players round-robin based on rating.
		for (i = 0; i < first_allocation; ++i) {
			players[i].subsection = (i % num_subsections) + 1;
		}
		unsigned next_subsection = 0;
		// Now allocate players two at a time to keep even numbers in sections.
		for (i = first_allocation; i < players.size(); i+=2) {
			// If only one player left then break.
			if (i + 1 >= players.size()) break;
			players[i].subsection = next_subsection + 1;
			players[i+1].subsection = next_subsection + 1;
			next_subsection = (next_subsection + 1) % num_subsections;
		}
		// If a single player was left then add to some subsection.
		if (i < players.size()) {
			players[i].subsection = next_subsection + 1;
		}
	}

	void clearPlayers(void) {
		players.clear();
	}

	bool includes(int rating, wchar_t grade) const {
		return (rating >= lower_rating_limit) && (rating <= upper_rating_limit) && (grade >= lower_grade_limit) && (grade <= upper_grade_limit);
	}

	bool usedRatings() const {
		return lower_rating_limit != 0 || upper_rating_limit != 3000;
	}

	bool usedGrade() const {
		return lower_grade_limit != 'A' || upper_grade_limit != 'N';
	}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			ar << name << lower_rating_limit << upper_rating_limit << lower_grade_limit << upper_grade_limit << sec_type << num_subsections << uscf_required << which_computer << time_control << starting_board_number << playing_room << num_rounds << parent_section << num_split;
		}
		else
		{
			ar >> name >> lower_rating_limit >> upper_rating_limit >> lower_grade_limit >> upper_grade_limit >> sec_type >> num_subsections >> uscf_required >> which_computer >> time_control >> starting_board_number >> playing_room >> num_rounds >> parent_section >> num_split;
		}
	}

	bool rating_conflict(const Section &other) const {
		if ((upper_rating_limit >= other.lower_rating_limit && lower_rating_limit <= other.lower_rating_limit) ||
			(other.upper_rating_limit > lower_rating_limit && other.lower_rating_limit <= lower_rating_limit)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool grade_conflict(const Section &other) const {
		if ((upper_grade_limit >= other.lower_grade_limit && lower_grade_limit <= other.lower_grade_limit) ||
			(other.upper_grade_limit > lower_grade_limit && other.lower_grade_limit <= lower_grade_limit)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool conflicts(const Section &other) const {
		return rating_conflict(other) && grade_conflict(other);
	}
};

class MRPlayer {
public:
	std::wstring ws_last;
	std::wstring ws_first;
	std::wstring ws_id;
	std::wstring ws_school_code;
	std::wstring ws_school_name;
	std::wstring ws_uscf_id;
	unsigned nwsrs_rating;
	std::wstring ws_uscf_rating;
	std::wstring ws_uscf_expr;
	wchar_t grade;

	std::wstring getUnique() {
		return L"MRPlayer" + ws_last + ws_first + ws_id + ws_school_name;
	}

	MRPlayer() {}

	MRPlayer(std::wstring &l, std::wstring &f, std::wstring &i, std::wstring &s, std::wstring &sn, std::wstring &u, unsigned nw, std::wstring &ur, std::wstring &ue, wchar_t g) :
		ws_last(l),
		ws_first(f),
		ws_id(i),
		ws_school_code(s),
		ws_school_name(sn),
		ws_uscf_id(u),
		nwsrs_rating(nw),
		ws_uscf_rating(ur),
		ws_uscf_expr(ue),
		grade(g) {}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			CString cs_last(ws_last.c_str());
			CString cs_first(ws_first.c_str());
			CString cs_id(ws_id.c_str());
			CString cs_school_code(ws_school_code.c_str());
			CString cs_school_name(ws_school_name.c_str());
			CString cs_uscf_id(ws_uscf_id.c_str());
			CString cs_uscf_rating(ws_uscf_rating.c_str());
			CString cs_uscf_expr(ws_uscf_expr.c_str());

			ar << cs_last << cs_first << cs_id << cs_school_code << cs_school_name << cs_uscf_id << nwsrs_rating << cs_uscf_rating << cs_uscf_expr << grade;
		}
		else
		{
			CString cs_last;
			CString cs_first;
			CString cs_id;
			CString cs_school_code;
			CString cs_school_name;
			CString cs_uscf_id;
			CString cs_uscf_rating;
			CString cs_uscf_expr;

			ar >> cs_last >> cs_first >> cs_id >> cs_school_code >> cs_school_name >> cs_uscf_id >> nwsrs_rating >> cs_uscf_rating >> cs_uscf_expr >> grade;
			ws_last = cs_last;
			ws_first = cs_first;
			ws_id = cs_id;
			ws_school_code = cs_school_code;
			ws_school_name = cs_school_name;
			ws_uscf_id = cs_uscf_id;
			ws_uscf_rating = cs_uscf_rating;
			ws_uscf_expr = cs_uscf_expr;
		}
	}
};

template <typename T>
class SerializedVector : public std::vector<T> {
public:
	void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			ar << size();
			for (unsigned i = 0; i < size(); ++i) {
				operator[](i).Serialize(ar);
			}
		}
		else
		{
			std::vector<T>::size_type length;
			ar >> length;

			for (unsigned i = 0; i < length; ++i) {
				T s;
				s.Serialize(ar);
				push_back(s);
			}
		}
	}
};

bool SectionPointerRatingReverseSort(const Section *a, const Section *b);

class Sections : public SerializedVector<Section> {
public:
	std::vector<Section *> getSectionsWithParentSorted(unsigned parent_section) {
		std::vector<Section *> ret;
		for (unsigned i = 0; i < size(); ++i) {
			if (operator[](i).parent_section == parent_section) {
				ret.push_back(&(operator[](i)));
			}
		}
		std::sort(ret.begin(), ret.end(), SectionPointerRatingReverseSort);
		return ret;
	}

	bool remove(Section *sec) {
		for (auto siter = begin(); siter != end(); ++siter) {
			if (&(*siter) == sec) {
				SerializedVector<Section>::erase(siter);
				return true;
			}
		}
		return false;
	}

	void turnOffResectioning(unsigned parent_section) {
		// Find all sections with the given parent_section and set them to -1 which turns off automatic resectioning.
		for (unsigned i = 0; i < size(); ++i) {
			operator[](i).parent_section = -1;
		}
	}

	void deleteWithParentSection(unsigned parent_section) {
		auto psiter = begin();
		while (psiter != end()) {
			if (psiter->parent_section == parent_section) {
				psiter = erase(psiter);
			}
			else {
				++psiter;
			}
		}
	}

	void clear(void) {
		std::vector<Section>::clear();
	}

	void clearPlayers(void) {
		for (unsigned i = 0; i < size(); ++i) {
			operator[](i).clearPlayers();
		}
	}

	void makeSubsections() {
		for (unsigned i = 0; i < size(); ++i) {
			operator[](i).makeSubsections();
		}
	}

	int findByName(CString &name) {
		for (int i = 0; i < size(); ++i) {
			if (operator[](i).name == name) {
				return i;
			}
		}
		return -1;
	}

	int foundIn(int cc_rating, wchar_t grade_code) {
		int found = -1;
		for (unsigned i = 0; i < size(); ++i) {
			if (operator[](i).includes(cc_rating, grade_code)) {
				if (found == -1) {
					found = i;
				}
				else {
					return -2;
				}
			}
		}
		return found;
	}

	int conflicts(const Section &other, int ignore_row=-1) const {
		for (unsigned i = 0; i < size(); ++i) {
			if (i == ignore_row) continue;

			if (operator[](i).conflicts(other)) {
				return i;
			}
		}
		return -1;
	}
};

/*
1 First Name
2 Last Name
3 Email Address
4 Address 1
5 Address 2
6 City
7 State
8 ZIP Code
9 Phone
10 Cell Phone
11 "Every Adult Registrant MUST add at least one student player by using the ""Player Information"" section below.  Please add all associated players by clicking the ""ADD"" button.  You will not be able to modify the entries once registration is complete, so please don't click ""Register"" at the bottom until every student is listed.  Contact us with problems."
12 School
13 Grade
14 Playing Section?
15 "NWSRS Identification - Please look up this information as noted above; if your student is not listed, please enter ""NONE""; otherwise please enter their seven-digit ID number below."
16 "NWSRS Rating - Please find your student's rating by checking the web site above; if your student is unrated, please enter ""N/A"""
17 "USCF Membership ID (required for Open / 1300+ section - otherwise, leave blank).  Look up this information as noted above, and enter the 8-digit ID below."
18 "USCF Rating (required for Open / 1300+ section; otherwise, leave blank)",
19 Are you willing to Volunteer?  We could use your help!,
20 COMMENTS / QUESTIONS?
21 Player of First Name
22 Player of Last Name
23 Registration Date
24 Registration Status
25 Payment Status
26 Payment Type
27 Fee Type
28 Promo Code
29 Code Type
30 Discount Percent
31 Discount Amount
32 Adult Registrant Quantity
33 Adult Registrant Price
34 Scholastic Player Quantity
35 Scholastic Player Price
36 Scholastic Player (late registration) Quantity
37 Scholastic Player (late registration) Price
38 ...
*/

std::wstring removeSubstring(const std::wstring &in, const std::wstring &pattern);
std::wstring removeSchoolSubstr(const std::wstring &name);

class AllCodesEntry {
protected:
	std::vector<std::wstring> fields;
	void valid(void) const {
		if (fields.size() < 5) {
			exit(-1);
		}
	}

	void internalCstr(const std::vector<std::wstring> &f) {
		fields = f;
		valid();
		std::wstring snup = toUpper(getSchoolName());
		fields.push_back(snup);
		fields.push_back(removeSchoolSubstr(snup));
	}

public:
	AllCodesEntry() {}

	AllCodesEntry(const std::vector<std::wstring> &f) { 
		internalCstr(f);
	}

	AllCodesEntry(
		const std::wstring &code, 
		const std::wstring &name, 
		const std::wstring &type, 
		const std::wstring &city, 
		const std::wstring &state) {

		std::vector<std::wstring> fields;
		fields.push_back(code);
		fields.push_back(name);
		fields.push_back(toUpper(type));
		fields.push_back(city);
		fields.push_back(state);
		internalCstr(fields);
	}

	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			for (unsigned i = 0; i < 5; ++i) {
				ar << WStringToCString(fields[i]);
			}
		}
		else
		{
			std::vector<std::wstring> new_fields;

			for (unsigned i = 0; i < 5; ++i) {
				CString one;
				ar >> one;
				new_fields.push_back(CStringToWString(one));
			}

			internalCstr(new_fields);
		}
	}

	std::wstring getSchoolCode(void) const { return fields[0]; }
	std::wstring getSchoolName(void) const { return fields[1]; }
	std::wstring getSchoolNameUpper(void) const { return fields[5]; }
	std::wstring getSchoolNameUpperNoSchool(void) const { return fields[6]; }
	std::wstring getSchoolType(void) const { return fields[2]; }
	std::wstring getSchoolCity(void) const { return fields[3]; }
	std::wstring getSchoolState(void) const { return fields[4]; }

	bool isType(SCHOOL_TYPE &st) const {
		std::wstring sts = getSchoolTypeStr(st);
		std::wstring cur_type = getSchoolType();
		// If there is no information about the school type then assume everything matches.
		return cur_type.length() == 0 || cur_type.find(sts) != std::string::npos;
	}
};

class AllCodesEntryCodeCompare {
public:
	bool operator()(const AllCodesEntry &a, const AllCodesEntry &b) {
		return a.getSchoolCode() < b.getSchoolCode();
	}
};

bool findStringIC(const std::wstring & strHaystack, const std::wstring & strNeedle);

class AllCodes : public std::vector<AllCodesEntry> {
protected:
	std::map<std::wstring, unsigned> map_id_to_index;

	void sortAndIndex() {
		std::sort(begin(), end(), AllCodesEntryCodeCompare());

		map_id_to_index.clear();
		for (size_t i = 0; i < size(); ++i) {
			map_id_to_index.insert(std::pair<std::wstring, unsigned>(operator[](i).getSchoolCode(), i));
		}
	}

public:
	SerializedVector<AllCodesEntry> m_new_schools;

	AllCodes(void) {}

	virtual void Serialize(CArchive& ar) {
		m_new_schools.Serialize(ar);
	}

	void Load(const std::wstring filename) {
		clear();
		map_id_to_index.clear();

		auto ccret = load_csvw_file(filename, false);

		unsigned i;
		for (i = 0; i < ccret.size(); ++i) {
			push_back(AllCodesEntry(ccret[i]));
			map_id_to_index.insert(std::pair<std::wstring, unsigned>(ccret[i][0], i));
		}

		SerializedVector<AllCodesEntry> after_dels;
		for (i = 0; i < m_new_schools.size(); ++i) {
			if (find(m_new_schools[i].getSchoolCode()) == -1) {
				push_back(m_new_schools[i]);
				after_dels.push_back(m_new_schools[i]);
			}
		}
		m_new_schools = after_dels;

		sortAndIndex();
	}

	bool schoolIsType(const std::wstring &code, SCHOOL_TYPE &st) {
		auto iter = map_id_to_index.find(code);
		ASSERT(iter != map_id_to_index.end());
		return operator[](iter->second).isType(st);
	}
	
	void addSchool(const AllCodesEntry &entry) {
		m_new_schools.push_back(entry);
		push_back(entry);
		sortAndIndex();
	}

	int find(const std::wstring &code) const {
		auto iter = map_id_to_index.find(code);
		if (iter == map_id_to_index.end()) {
			return -1;
		}
		else {
			return iter->second;
		}
	}

	std::wstring findName(const std::wstring &code) const {
		int index = find(code);
		if (index == -1) {
			return L"";
		}
		else {
			return operator[](index).getSchoolName();
		}
	}

	std::wstring findNameUpperNoSchool(const std::wstring &code) const {
		int index = find(code);
		if (index == -1) {
			return L"";
		}
		else {
			return operator[](index).getSchoolNameUpperNoSchool();
		}
	}

	bool schoolNameMatchesCode(const std::wstring &name, const std::wstring &code) {
		std::wstring sup = removeSchoolSubstr(toUpper(name));
		std::wstring name_from_code = findNameUpperNoSchool(code);
		return multiWordMatches(sup, name_from_code) > 0;
	}

	// See if there is exactly one exact match for the school in the list.
	std::wstring findCodeFromSchoolExact(const std::wstring &s) const {
		std::wstring sup = toUpper(s);

		std::wstring ret = L"";

		for (unsigned i = 1; i < size(); ++i) {
			if (sup == operator[](i).getSchoolNameUpper()) {
				if (ret == L"") {
					ret = operator[](i).getSchoolCode();
				}
				else {
					return L"";
				}
			}
		}

		return ret;
	}

	bool isExactNoSchool(const std::wstring &s, const std::wstring &cur_code) const {
		std::wstring sup = removeSchoolSubstr(toUpper(s));

		for (unsigned i = 1; i < size(); ++i) {
			if (sup == operator[](i).getSchoolNameUpperNoSchool()) {
				if (operator[](i).getSchoolCode() == cur_code) {
					return true;
				}
			}
		}

		return false;
	}

	std::wstring findCodeFromSchoolExactNoSchool(const std::wstring &s, SCHOOL_TYPE st) const {
		std::wstring sup = removeSchoolSubstr(toUpper(s));

		std::wstring ret = L"";

		for (unsigned i = 1; i < size(); ++i) {
			bool hasRightType = operator[](i).isType(st);
			if (hasRightType && sup == operator[](i).getSchoolNameUpperNoSchool()) {
				if (ret == L"") {
					ret = operator[](i).getSchoolCode();
				}
				else {
					return L"";
				}
			}
		}

		return ret;
	}

	std::wstring findCodeFromSchool(const std::wstring &s, std::wofstream &normal_log) const {
#if 1
		if (s.length() == 3) {
			std::wstring supper = toUpper(s);
			int index = find(supper);
			if (index != -1) {
				return supper;
			}
		}

		int best_index = 0;
		int best_value = LevenshteinDistance(s, operator[](0).getSchoolName());
		//normal_log << s << ". " << operator[](0).getSchoolName() << ". " << best_value << " " << best_index << std::endl;
		for (unsigned i = 1; i < size(); ++i) {
			int nv = LevenshteinDistance(s, operator[](i).getSchoolName());
			//normal_log << s << ". " << operator[](i).getSchoolName() << ". " << best_value << " " << best_index << " " << nv << std::endl;
			if (nv < best_value) {
				best_value = nv;
				best_index = i;
			}
		}
		return operator[](best_index).getSchoolCode();
#else
		auto fset = getPotentialSet(s);
		if (fset.empty()) {
			return L"";
		}
		else {
			if (fset.size() == 1) {
				return *fset.begin();
			}
			else {
				return L"";
			}
		}
#endif
	}

	std::set<std::wstring> getPotentialSet(const std::wstring &partial) const {
		std::set<std::wstring> ret;
		unsigned i;
		for (i = 0; i < size(); ++i) {
			if (findStringIC(operator[](i).getSchoolName(), partial)) {
				ret.insert(operator[](i).getSchoolCode());
			}
		}
		return ret;
	}
};

bool isAlpha(std::wstring &s);
bool isNumeric(const std::wstring &s);
bool hasNumeric(std::wstring &s);

std::vector< ConstantContactEntry > load_constant_contact_file(const std::wstring &filename,
	const std::map<std::wstring, unsigned> &nwsrs_map,
	const std::map<std::wstring, unsigned> &uscf_map,
	const std::vector<Player> &rated_players,
	const AllCodes &school_codes,
	std::wofstream &normal_log,
	DynamicLocations& dynamic_locations);

std::wstring getLastFour(const std::wstring &full);

class CorrectedFields {
public:
	std::wstring school_code;
	std::wstring id;

	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring()) {
			CString sc = WStringToCString(school_code);
			CString si = WStringToCString(id);
			ar << sc << si;
		}
		else {
			CString sc, si;
			ar >> sc >> si;
			school_code = CStringToWString(sc);
			id = CStringToWString(si);
		}
	}

	void update(std::wstring &in_school_code, std::wstring &in_id) {
		if (school_code != L"") in_school_code = school_code;
		if (id != L"") in_id = id;
	}
};

CArchive & operator<<(CArchive &ar, const std::wstring &s);
CArchive & operator>>(CArchive &ar, std::wstring &s);
CArchive & operator<<(CArchive &ar, Section &s);
CArchive & operator>>(CArchive &ar, Section &s);

template <typename A, typename B>
class SerializedMap : public std::map<A, B> {
public:

	virtual A last_key() const {
		return this->rbegin()->first;
	}

	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			int fs_size = size();
			ar << fs_size;
			for (auto fsiter = begin(); fsiter != end(); ++fsiter) {
				ar << fsiter->first << fsiter->second;
			}
		}
		else {
			int fs_size;
			ar >> fs_size;
			for (unsigned i = 0; i < fs_size; ++i) {
				A a;
				B b;
				ar >> a >> b;
				insert(std::pair<A, B>(a, b));
			}
		}
	}
};

template <typename A>
class SerializedSet : public std::set<A> {
public:
	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			int fs_size = size();
			ar << fs_size;
			for (auto fsiter = begin(); fsiter != end(); ++fsiter) {
				ar << *fsiter;
			}
		}
		else {
			int fs_size;
			ar >> fs_size;
			for (unsigned i = 0; i < fs_size; ++i) {
				std::wstring a;
				ar >> a;
				insert(a);
			}
		}
	}
};

class RegDataField {
public:
	int column_number;
	std::wstring column_name;
	std::wstring swisssys_field_name;
	bool put_on_checkin;

	RegDataField(int c, const std::wstring& cn) : column_number(c), column_name(cn), swisssys_field_name(_T("None")), put_on_checkin(false) {}
	RegDataField() : column_number(-1), column_name(_T("")), swisssys_field_name(_T("None")), put_on_checkin(false) {}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring()) {
			ar << column_number << column_name << swisssys_field_name << put_on_checkin;
		}
		else {
			ar >> column_number >> column_name >> swisssys_field_name >> put_on_checkin;
		}
	}
};

class RegDataFieldVector : public SerializedVector<RegDataField> {
public:
	void init_from_file(const std::wstring& ccname, const DynamicLocations& dynamic_locations);
};

class CCCSwisssys2Doc : public CDocument
{
protected: // create from serialization only
	CCCSwisssys2Doc();
	DECLARE_DYNCREATE(CCCSwisssys2Doc)

// Attributes
public:
	CString ratings_file;
	CString constant_contact_file;
	CString school_code_file;
	CString restricted_file;
	Sections sections;
	SerializedVector<MRPlayer> mrplayers;
	AllCodes school_codes;
	std::map<std::wstring, CorrectedFields> saved_school_corrections;
	std::vector<Player> rated_players;
	std::map<std::wstring, unsigned> nwsrs_map;
	std::map<std::wstring, unsigned> nwsrs_four_map;
	std::map<std::wstring, unsigned> uscf_map;
	bool save_school_corrections;
	SerializedSet<std::wstring> noshows;
	SerializedMap<std::wstring, std::wstring> force_sections;  // map of unique player key to the forced section name
	CTime m_tournament_date;
	SerializedMap<unsigned, Section> parent_sections;
	RegDataFieldVector rdfv;
	DynamicLocations dynamic_locations;
	std::map<std::wstring, unsigned> extra_swisssys_fields;
	std::vector<ConstantContactEntry> entries;

// Operations
public:

	bool loadRatingsFile();

	void clearPlayers(void) {
		sections.clearPlayers();
		for (auto psiter = parent_sections.begin(); psiter != parent_sections.end(); ++psiter) {
			psiter->second.clearPlayers();
		}
	}

	void turnOffResectioning(unsigned parent_section, bool keep_sections) {
		if (keep_sections) {
			sections.turnOffResectioning(parent_section);
			parent_sections.erase(parent_section);
		}
		else {
			sections.deleteWithParentSection(parent_section);
			auto ps = parent_sections.find(parent_section);
			ASSERT(ps != parent_sections.end());
			ps->second.num_split = -1;
			sections.push_back(ps->second);
			parent_sections.erase(ps);
		}
		SetModifiedFlag();
	}

	Sections getSectionsWithParents(std::map<int, int> &ssmap) {
		Sections ret;
		std::set<int> placed_parent_sections;

		// For each section...
		for (int i = 0; i < sections.size(); ++i) {
			// If it is a non-automatic resectioned section then place it.
			if (sections[i].parent_section == -1) {
				ssmap.insert(std::pair<int, int>(ret.size(), i));
				ret.push_back(sections[i]);
			}
			else {
				// Else see if we haven't already placed the parent section.
				if (placed_parent_sections.find(sections[i].parent_section) == placed_parent_sections.end()) {
					// We haven't so  place it now.

					// Find the parent section based on its number.
					auto ps = parent_sections.find(sections[i].parent_section);
					// Make sure the parent section exists.
					if (ps == parent_sections.end()) {
						ASSERT(0);
					}
					// Add the parent section.
					ssmap.insert(std::pair<int, int>(ret.size(), i));
					ret.push_back(ps->second);
					// Remember that we've added this parent section.
					placed_parent_sections.insert(sections[i].parent_section);
				}
			}
		}

		ASSERT(placed_parent_sections.size() == parent_sections.size());

		return ret;
	}

	std::map<std::wstring, CorrectedFields>::iterator get_corrected_iter(const std::wstring &key) {
		auto prev_iter = saved_school_corrections.find(key);
		if (prev_iter == saved_school_corrections.end()) {
			saved_school_corrections.insert(std::pair<std::wstring, CorrectedFields>(key, CorrectedFields()));
			prev_iter = saved_school_corrections.find(key);
		}
		return prev_iter;
	}

	std::wstring get_saved_correction_school_code(const std::wstring &key) {
		auto prev_iter = saved_school_corrections.find(key);
		if (prev_iter != saved_school_corrections.end()) {
			if (prev_iter->second.school_code != L"") {
				return prev_iter->second.school_code;
			}
		}
		return L"";
	}

	std::wstring get_saved_correction_id(const std::wstring &key) {
		auto prev_iter = saved_school_corrections.find(key);
		if (prev_iter != saved_school_corrections.end()) {
			if (prev_iter->second.id != L"") {
				return prev_iter->second.id;
			}
		}
		return L"";
	}

	std::wstring getOutputLocation() {
//		if (constant_contact_file.IsEmpty()) {
		while (this->m_strPathName == L"") {
			this->OnFileSaveAs();
		}
			std::wstring copy = CStringToWString(this->m_strPathName);
			wchar_t *buf = _wcsdup(copy.c_str());
//			HRESULT res = PathCchRemoveFileSpec(buf, copy.length());
			HRESULT res = PathRemoveFileSpec(buf);
			return std::wstring(buf);
//		}
//		else {
//			std::wstring copy = CStringToWString(constant_contact_file);
//			wchar_t *buf = _wcsdup(copy.c_str());
//			HRESULT res = PathCchRemoveFileSpec(buf, copy.length());
//			HRESULT res = PathRemoveFileSpec(buf);
//			return std::wstring(buf);
//		}
	}

	std::wstring getFileBase() const {
		CString cur_path = GetPathName();
		if (cur_path.IsEmpty()) return L"";
		std::wstring copy = CStringToWString(cur_path);
		size_t lastindex = copy.find_last_of(L".");
		return copy.substr(0, lastindex);
	}

	CTime getValidTournamentDate();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CCCSwisssys2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnOptionsTournamentdate();
	afx_msg void OnOptionsRegistrationFields();
};

class log_messages {
public:
	std::wstringstream school_change;
	std::wstringstream new_uscf_ids;
	std::wstringstream uscf_id_cc_no_match;
	std::wstringstream name_change;
	std::wstringstream no_uscf_membership;
	std::wstringstream expired_uscf_membership;
	std::wstringstream grade_change;
	std::wstringstream imperfect;
	std::wstringstream grade_code;
};

