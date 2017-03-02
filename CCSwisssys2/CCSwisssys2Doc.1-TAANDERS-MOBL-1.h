
// CCSwisssys2Doc.h : interface of the CCCSwisssys2Doc class
//


#pragma once

#include <vector>
#include "tokenize_csv.h"
#include <Pathcch.h>
#include <set>
#include <algorithm>

typedef int SECTION_TYPE;

#define	SWISS 0
#define ROUND_ROBIN 1

CString getGradeString(wchar_t grade);
CString getSectionTypeString(int type);
std::wstring CStringToWString(const CString &cs);
int LevenshteinDistance(const std::wstring &s, const std::wstring &t);
std::wstring toUpper(const std::wstring &s);

class SectionPlayerInfo {
public:
	CString last_name;
	CString first_name;
	CString full_id;
	int rating;
	wchar_t grade;
	CString school;
	CString school_code;
	CString uscf_id;
	CString uscf_rating;

	SectionPlayerInfo(const CString &ln, const CString &fn, const CString &fi, int r, wchar_t g, const CString &s, const CString &sc, const CString &uscfid, const CString &uscfrating) : last_name(ln), first_name(fn), full_id(fi), rating(r), grade(g), school(s), school_code(sc), uscf_id(uscfid), uscf_rating(uscfrating) {}
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

	Section() : name(""), lower_rating_limit(0), upper_rating_limit(3000), lower_grade_limit('A'), upper_grade_limit('M'), sec_type(SWISS) {}
	
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
		return lower_grade_limit != 'A' || upper_grade_limit != 'M';
	}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			ar << name << lower_rating_limit << upper_rating_limit << lower_grade_limit << upper_grade_limit << sec_type;
		}
		else
		{
			ar >> name >> lower_rating_limit >> upper_rating_limit >> lower_grade_limit >> upper_grade_limit >> sec_type;
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
	wchar_t grade;

	MRPlayer() {}

	MRPlayer(std::wstring &l, std::wstring &f, std::wstring &i, std::wstring &s, std::wstring &sn, std::wstring &u, unsigned nw, std::wstring &ur, wchar_t g) :
		ws_last(l),
		ws_first(f),
		ws_id(i),
		ws_school_code(s),
		ws_school_name(sn),
		ws_uscf_id(u),
		nwsrs_rating(nw),
		ws_uscf_rating(ur),
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

			ar << cs_last << cs_first << cs_id << cs_school_code << cs_school_name << cs_uscf_id << nwsrs_rating << cs_uscf_rating << grade;
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

			ar >> cs_last >> cs_first >> cs_id >> cs_school_code >> cs_school_name >> cs_uscf_id >> nwsrs_rating >> cs_uscf_rating >> grade;
			ws_last = cs_last;
			ws_first = cs_first;
			ws_id = cs_id;
			ws_school_code = cs_school_code;
			ws_school_name = cs_school_name;
			ws_uscf_id = cs_uscf_id;
			ws_uscf_rating = cs_uscf_rating;
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
			unsigned length;
			ar >> length;

			for (unsigned i = 0; i < length; ++i) {
				T s;
				s.Serialize(ar);
				push_back(s);
			}
		}
	}
};

class Sections : public SerializedVector<Section> {
public:
	void clear(void) {
		std::vector<Section>::clear();
	}

	void clearPlayers(void) {
		for (unsigned i = 0; i < size(); ++i) {
			operator[](i).clearPlayers();
		}
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

class AllCodesEntry {
protected:
	std::vector<std::wstring> fields;
	void valid(void) const {
		if (fields.size() < 5) {
			//std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
			exit(-1);
		}
	}

public:
	AllCodesEntry(const std::vector<std::wstring> &f) : fields(f) { valid(); }

	std::wstring getSchoolCode(void) const { return fields[0]; }
	std::wstring getSchoolName(void) const { return fields[1]; }
	std::wstring getSchoolType(void) const { return fields[2]; }
	std::wstring getSchoolCity(void) const { return fields[3]; }
	std::wstring getSchoolState(void) const { return fields[4]; }
};

bool findStringIC(const std::wstring & strHaystack, const std::wstring & strNeedle);

class AllCodes : public std::vector<AllCodesEntry> {
protected:
	std::map<std::wstring, unsigned> map_id_to_index;

public:
	AllCodes(void) {}

	void Load(const std::wstring filename) {
		auto ccret = load_csvw_file(filename, true);

		unsigned i;
		for (i = 0; i < ccret.size(); ++i) {
			push_back(AllCodesEntry(ccret[i]));
			map_id_to_index.insert(std::pair<std::wstring, unsigned>(ccret[i][0], i));
		}
	}

	int find(const std::wstring &code) {
		auto iter = map_id_to_index.find(code);
		if (iter == map_id_to_index.end()) {
			return -1;
		}
		else {
			return iter->second;
		}
	}

	std::wstring findName(const std::wstring &code) {
		int index = find(code);
		if (index == -1) {
			return L"";
		}
		else {
			return operator[](index).getSchoolName();
		}
	}

	std::wstring findCodeFromSchool(const std::wstring &s, std::wofstream &normal_log) {
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
		normal_log << s << ". " << operator[](0).getSchoolName() << ". " << best_value << " " << best_index << std::endl;
		for (int i = 1; i < size(); ++i) {
			int nv = LevenshteinDistance(s, operator[](i).getSchoolName());
			normal_log << s << ". " << operator[](i).getSchoolName() << ". " << best_value << " " << best_index << " " << nv << std::endl;
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

	std::set<std::wstring> getPotentialSet(const std::wstring &partial) {
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

auto REG_STR = L"Registered";
auto ADULT_STR = L"\"OK, I'll add my players below!\"";

enum {
	FIRST_NAME = 0,
	LAST_NAME = 1,
	ADULT_CHECK = 2,
	STUDENT_SCHOOL = 3,
	STUDENT_GRADE = 4,
	STUDENT_NWSRS_ID = 5,
	STUDENT_USCF_ID = 6,
	REGISTERED = 7
};

extern int allstars2016[];

class ConstantContactEntry {
protected:
	std::vector<std::wstring> fields;
	int * m_locations;
	void valid(void) const {
		if (fields.size() < 18) {
			//std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
			exit(-1);
		}
	}
public:
	ConstantContactEntry(const std::vector<std::wstring> &f, int * locations) : fields(f), m_locations(locations) { valid(); }

	std::wstring getFirstName(void)   const { valid(); return fields[m_locations[FIRST_NAME]]; }
	std::wstring getLastName(void)    const { valid(); return fields[m_locations[LAST_NAME]]; }
	/*
	std::wstring getEmail(void)       const { valid(); return fields[2]; }
	std::wstring getPhone(void)       const { valid(); return fields[3]; }
	std::wstring getAddress1(void)    const { valid(); return fields[4]; }
	std::wstring getCity(void)        const { valid(); return fields[5]; }
	std::wstring getState(void)       const { valid(); return fields[6]; }
	std::wstring getZip(void)         const { valid(); return fields[7]; }
	std::wstring getCellPhone(void)   const { valid(); return fields[8]; }
	std::wstring getVolunteer(void)   const { valid(); return fields[9]; }
	std::wstring getQuestions(void)   const { valid(); return fields[10]; }
	*/
	bool didAdultCheck(void)          const { valid(); return fields[m_locations[ADULT_CHECK]] == L"\"OK, I'll add my players below!\""; }
	std::wstring getSchool(void)      const { valid(); return fields[m_locations[STUDENT_SCHOOL]]; }
	std::wstring getGrade(void)       const { valid(); return fields[m_locations[STUDENT_GRADE]]; }
	//std::wstring getSection(void)     const { valid(); return fields[13]; }
	std::wstring getNwsrsId(void)     const { valid(); return fields[m_locations[STUDENT_NWSRS_ID]]; }
	//std::wstring getNwsrsRating(void) const { valid(); return fields[m_locations[STUDENT_NWSRS_RATING]]; }
	std::wstring getUscfId(void)      const { valid(); return fields[m_locations[STUDENT_USCF_ID]]; }
	//std::wstring getUscfRating(void)  const { valid(); return fields[m_locations[STUDENT_USCF_RATING]]; }

	bool isRegistered(void) const {
		valid();
		return fields[m_locations[REGISTERED]] == L"Registered";
	}

	bool isParent(void) const {
		return didAdultCheck();
//		return didAdultCheck() && getSchool().empty() && getGrade().empty() && getNwsrsId().empty();
	}

	bool isPlayer(void) const {
		return !didAdultCheck() && !getNwsrsId().empty();
	}

	wchar_t getGradeCode(void) const {
		auto gg = getGrade();
		if (gg == L"K") { return L'A'; }
		if (gg == L"1") { return L'B'; }
		if (gg == L"2") { return L'C'; }
		if (gg == L"3") { return L'D'; }
		if (gg == L"4") { return L'E'; }
		if (gg == L"5") { return L'F'; }
		if (gg == L"6") { return L'G'; }
		if (gg == L"7") { return L'H'; }
		if (gg == L"8") { return L'I'; }
		if (gg == L"9") { return L'J'; }
		if (gg == L"10") { return L'K'; }
		if (gg == L"11") { return L'L'; }
		if (gg == L"12") { return L'M'; }
		MessageBox(NULL, _T("Don't know how to convert some grade string into grade code."), _T("ERROR"), MB_OK);
		exit(-1);
	}
};

std::vector< ConstantContactEntry > load_constant_contact_file(const std::wstring &filename, int * locations);

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
	Sections sections;
	SerializedVector<MRPlayer> mrplayers;
	AllCodes school_codes;

// Operations
public:

	std::wstring getOutputLocation() const {
		if (constant_contact_file.IsEmpty()) {
			std::wstring copy = CStringToWString(this->m_strPathName);
			wchar_t *buf = _wcsdup(copy.c_str());
			HRESULT res = PathCchRemoveFileSpec(buf, copy.length());
			return std::wstring(buf);
		}
		else {
			std::wstring copy = CStringToWString(constant_contact_file);
			wchar_t *buf = _wcsdup(copy.c_str());
			HRESULT res = PathCchRemoveFileSpec(buf, copy.length());
			return std::wstring(buf);
		}
	}

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
};

