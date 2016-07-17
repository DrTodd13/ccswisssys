
// CCSwisssys2Doc.h : interface of the CCCSwisssys2Doc class
//


#pragma once

#include <vector>
#include "tokenize_csv.h"
#include <Pathcch.h>
#include <set>

typedef int SECTION_TYPE;

#define	SWISS 0
#define ROUND_ROBIN 1

CString getGradeString(char grade);
CString getSectionTypeString(int type);
std::string CStringToString(const CString &cs);
std::wstring CStringToWString(const CString &cs);

class SectionPlayerInfo {
public:
	CString last_name;
	CString first_name;
	CString full_id;
	int rating;
	char grade;
	CString school;
	CString school_code;
	CString uscf_id;
	CString uscf_rating;

	SectionPlayerInfo(const CString &ln, const CString &fn, const CString &fi, int r, char g, const CString &s, const CString &sc, const CString &uscfid, const CString &uscfrating) : last_name(ln), first_name(fn), full_id(fi), rating(r), grade(g), school(s), school_code(sc), uscf_id(uscfid), uscf_rating(uscfrating) {}
};

class Section {
public:
	CString name;
	int lower_rating_limit;
	int upper_rating_limit;
	char lower_grade_limit;
	char upper_grade_limit;
	SECTION_TYPE sec_type;
	std::vector<SectionPlayerInfo> players;

	Section() : name(""), lower_rating_limit(0), upper_rating_limit(3000), lower_grade_limit('A'), upper_grade_limit('M'), sec_type(SWISS) {}
	
	void clearPlayers(void) {
		players.clear();
	}

	bool includes(int rating, char grade) const {
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
	std::wstring ws_school;
	std::wstring ws_uscf_id;
	unsigned nwsrs_rating, uscf_rating;
	char grade;

	MRPlayer() {}

	MRPlayer(std::wstring &l, std::wstring &f, std::wstring &i, std::wstring &s, std::wstring &u, unsigned nw, unsigned ur, char g) :
		ws_last(l),
		ws_first(f),
		ws_id(i),
		ws_school(s),
		ws_uscf_id(u),
		nwsrs_rating(nw),
		uscf_rating(ur),
		grade(g) {}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
			CString cs_last(ws_last.c_str());
			CString cs_first(ws_first.c_str());
			CString cs_id(ws_id.c_str());
			CString cs_school(ws_school.c_str());
			CString cs_uscf_id(ws_uscf_id.c_str());

			ar << cs_last << cs_first << cs_id << cs_school << cs_uscf_id << nwsrs_rating << uscf_rating << grade;
		}
		else
		{
			CString cs_last;
			CString cs_first;
			CString cs_id;
			CString cs_school;
			CString cs_uscf_id;

			ar >> cs_last >> cs_first >> cs_id >> cs_school >> cs_uscf_id >> nwsrs_rating >> uscf_rating >> grade;
			ws_last = cs_last;
			ws_first = cs_first;
			ws_id = cs_id;
			ws_school = cs_school;
			ws_uscf_id = cs_uscf_id;
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

	int foundIn(int cc_rating, char grade_code) {
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
	std::vector<std::string> fields;
	void valid(void) const {
		if (fields.size() < 5) {
			//std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
			exit(-1);
		}
	}

public:
	AllCodesEntry(const std::vector<std::string> &f) : fields(f) { valid(); }

	std::string getSchoolCode(void) const { return fields[0]; }
	std::string getSchoolName(void) const { return fields[1]; }
	std::string getSchoolType(void) const { return fields[2]; }
	std::string getSchoolCity(void) const { return fields[3]; }
	std::string getSchoolState(void) const { return fields[4]; }
};

bool findStringIC(const std::string & strHaystack, const std::string & strNeedle);

class AllCodes : public std::vector<AllCodesEntry> {
public:
	AllCodes(void) {}

	void Load(const std::string filename) {
			auto ccret = load_csv_file(filename, true);

		unsigned i;
		for (i = 0; i < ccret.size(); ++i) {
			push_back(AllCodesEntry(ccret[i]));
		}
	}

	std::set<std::wstring> getPotentialSet(const std::wstring &partial) {
		std::set<std::wstring> ret;
		unsigned i;
		for (i = 0; i < size(); ++i) {
			if (findStringIC(operator[](i), partial)) {
				ret.insert()
			}
		}
		return ret;
	}
};

class ConstantContactEntry {
protected:
	std::vector<std::string> fields;
	void valid(void) const {
		if (fields.size() < 18) {
			//std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
			exit(-1);
		}
	}
public:
	ConstantContactEntry(const std::vector<std::string> &f) : fields(f) { valid(); }

	std::string getFirstName(void)   const { valid(); return fields[0]; }
	std::string getLastName(void)    const { valid(); return fields[1]; }
	std::string getEmail(void)       const { valid(); return fields[2]; }
	bool didAdultCheck(void)         const { valid(); return !fields[10].empty(); }
	std::string getSchool(void)      const { valid(); return fields[11]; }
	std::string getGrade(void)       const { valid(); return fields[12]; }
	std::string getSection(void)     const { valid(); return fields[13]; }
	std::string getNwsrsId(void)     const { valid(); return fields[14]; }
	std::string getNwsrsRating(void) const { valid(); return fields[15]; }
	std::string getUscfId(void)      const { valid(); return fields[16]; }
	std::string getUscfRating(void)  const { valid(); return fields[17]; }

	bool isParent(void) const {
		return didAdultCheck() && getSchool().empty() && getGrade().empty() && getSection().empty() && getNwsrsId().empty();
	}

	bool isPlayer(void) const {
		return !didAdultCheck() && !getSchool().empty() && !getGrade().empty() && !getSection().empty() && !getNwsrsId().empty();
	}

	char getGradeCode(void) const {
		auto gg = getGrade();
		if (gg == "K") { return 'A'; }
		if (gg == "1") { return 'B'; }
		if (gg == "2") { return 'C'; }
		if (gg == "3") { return 'D'; }
		if (gg == "4") { return 'E'; }
		if (gg == "5") { return 'F'; }
		if (gg == "6") { return 'G'; }
		if (gg == "7") { return 'H'; }
		if (gg == "8") { return 'I'; }
		if (gg == "9") { return 'J'; }
		if (gg == "10") { return 'K'; }
		if (gg == "11") { return 'L'; }
		if (gg == "12") { return 'M'; }
		MessageBox(NULL, _T("Don't know how to convert some grade string into grade code."), _T("ERROR"), MB_OK);
		exit(-1);
	}
};

std::vector< ConstantContactEntry > load_constant_contact_file(const std::string &filename);

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

// Operations
public:

	std::wstring getOutputLocation() const {
		std::wstring copy = CStringToWString(constant_contact_file);
		wchar_t *buf = _wcsdup(copy.c_str());
		HRESULT res = PathCchRemoveFileSpec(buf, copy.length());
		return std::wstring(buf);
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
