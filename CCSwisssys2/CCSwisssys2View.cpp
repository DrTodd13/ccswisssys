
// CCSwisssys2View.cpp : implementation of the CCCSwisssys2View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CCSwisssys2.h"
#endif

#include "CCSwisssys2Doc.h"
#include "CCSwisssys2View.h"
#include "SectionEditor.h"
#include <fstream>
#include <algorithm>
#include "ManageRegistrations.h"
#include "SchoolSelector.h"
#include "SplitSection.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCCSwisssys2View

IMPLEMENT_DYNCREATE(CCCSwisssys2View, CFormView)

BEGIN_MESSAGE_MAP(CCCSwisssys2View, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CCCSwisssys2View::OnRatingsFileBrowse)
	ON_BN_CLICKED(IDC_BUTTON2, &CCCSwisssys2View::OnConstantContactFileBrowse)
	ON_EN_CHANGE(IDC_EDIT1, &CCCSwisssys2View::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CCCSwisssys2View::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCCSwisssys2View::OnAddSection)
	ON_BN_CLICKED(IDC_BUTTON4, &CCCSwisssys2View::OnDeleteSection)
	ON_BN_CLICKED(IDC_BUTTON5, &CCCSwisssys2View::OnEditSection)
	ON_BN_CLICKED(IDC_BUTTON6, &CCCSwisssys2View::OnCreateSections)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CCCSwisssys2View::OnSectionColumnClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CCCSwisssys2View::OnSectionClick)
	ON_BN_CLICKED(IDC_BUTTON7, &CCCSwisssys2View::OnCreateClubFiles)
	ON_BN_CLICKED(IDC_BUTTON8, &CCCSwisssys2View::OnSchoolCodesFileBrowse)
	ON_EN_CHANGE(IDC_EDIT3, &CCCSwisssys2View::OnEnChangeEdit3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CCCSwisssys2View::OnSectionReorder)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST2, &CCCSwisssys2View::OnSectionPlayersColumnSort)
	ON_BN_CLICKED(IDC_BUTTON9, &CCCSwisssys2View::OnManageAdditionalRegistrations)
	ON_BN_CLICKED(IDC_BUTTON10, &CCCSwisssys2View::OnCreateSwisssysTourney)
	ON_BN_CLICKED(IDC_BUTTON11, &CCCSwisssys2View::OnRestrictFileBrowse)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CCCSwisssys2View::OnDoubleClickSectionList)
	ON_BN_CLICKED(IDC_SPLIT_SECTION, &CCCSwisssys2View::OnBnClickedSplitSection)
	ON_COMMAND(ID_OPTIONS_SAVESCHOOLCORRECTIONS, &CCCSwisssys2View::OnOptionsSaveschoolcorrections)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SAVESCHOOLCORRECTIONS, &CCCSwisssys2View::OnUpdateOptionsSaveschoolcorrections)
END_MESSAGE_MAP()

// CCCSwisssys2View construction/destruction

CCCSwisssys2View::CCCSwisssys2View()
	: CFormView(IDD_CCSWISSSYS2_FORM)
{
	first_time = true;
	reset_section_sort();
	last_section_sort = -1;
}

CCCSwisssys2View::~CCCSwisssys2View()
{
}

void CCCSwisssys2View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, ratings_file_edit);
	DDX_Control(pDX, IDC_EDIT2, constant_contact_file_edit);
	DDX_Control(pDX, IDC_LIST1, section_list);
	DDX_Control(pDX, IDC_LIST2, section_players);
	DDX_Control(pDX, IDC_EDIT3, SchoolCodesEdit);
	DDX_Control(pDX, IDC_SPIN1, section_spin_button);
	DDX_Control(pDX, IDC_EDIT5, restricted_edit);
}

BOOL CCCSwisssys2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void addToSectionList(CListCtrl &section_list, Section &newSection, int position);

void CCCSwisssys2View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	if (first_time) {

		first_time = false;

		LVCOLUMN lvColumn;
		int nCol;

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 90;
		lvColumn.pszText = _T("Section Name");
		nCol = section_list.InsertColumn(0, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 70;
		lvColumn.pszText = _T("Min Rating");
		section_list.InsertColumn(1, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 70;
		lvColumn.pszText = _T("Max Rating");
		section_list.InsertColumn(2, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 60;
		lvColumn.pszText = _T("Min Grade");
		section_list.InsertColumn(3, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 65;
		lvColumn.pszText = _T("Max Grade");
		section_list.InsertColumn(4, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 70;
		lvColumn.pszText = _T("Type");
		section_list.InsertColumn(5, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Num Players");
		section_list.InsertColumn(6, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Subsections");
		section_list.InsertColumn(7, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Computer");
		section_list.InsertColumn(8, &lvColumn);

		section_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//-------------------------------------------------------------------------------

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("Last Name");
		nCol = section_players.InsertColumn(0, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("First Name");
		section_players.InsertColumn(1, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 70;
		lvColumn.pszText = _T("NWSRS Rating");
		section_players.InsertColumn(2, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("NWSRS ID");
		section_players.InsertColumn(3, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("Grade");
		section_players.InsertColumn(4, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("School");
		section_players.InsertColumn(5, &lvColumn);
		
		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 70;
		lvColumn.pszText = _T("USCF Rating");
		section_players.InsertColumn(6, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("USCF ID");
		section_players.InsertColumn(7, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 80;
		lvColumn.pszText = _T("Subsection");
		section_players.InsertColumn(8, &lvColumn);

		section_players.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	}

	//-------------------------------------------------------------------------------

	auto pDoc = GetDocument();
	ratings_file_edit.SetWindowText(pDoc->ratings_file);
	constant_contact_file_edit.SetWindowText(pDoc->constant_contact_file);
	SchoolCodesEdit.SetWindowTextW(pDoc->school_code_file);
	restricted_edit.SetWindowTextW(pDoc->restricted_file);
	refillSections(pDoc->sections);
	section_list.SetSelectionMark(-1);
	section_players.SetSelectionMark(-1);

	if (pDoc->school_code_file != L"") {
		pDoc->school_codes.Load(CStringToWString(pDoc->school_code_file));
		if (pDoc->school_codes.empty()) {
			MessageBox(_T("No school codes found in file.  Please select a code file in csv format."), _T("Error"));
		}
	}
}


// CCCSwisssys2View diagnostics

#ifdef _DEBUG
void CCCSwisssys2View::AssertValid() const
{
	CFormView::AssertValid();
}

void CCCSwisssys2View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CCCSwisssys2Doc* CCCSwisssys2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCCSwisssys2Doc)));
	return (CCCSwisssys2Doc*)m_pDocument;
}
#endif //_DEBUG


// CCCSwisssys2View message handlers


void CCCSwisssys2View::OnRatingsFileBrowse()
{
	CFileDialog FileDialog(TRUE, _T("dat"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->ratings_file = PathName;

		//MessageBox((LPCTSTR)PathName, _T("Ratings Filename"));
		ratings_file_edit.SetWindowText(PathName);
		GetDocument()->SetModifiedFlag();
#if 1
		if (!GetDocument()->ratings_file.IsEmpty() && !GetDocument()->loadRatingsFile()) {
			GetDocument()->ratings_file = L"";
			ratings_file_edit.SetWindowText(L"");
		}
#endif
	}
}


void CCCSwisssys2View::OnConstantContactFileBrowse()
{
	CFileDialog FileDialog(TRUE, _T("csv"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->constant_contact_file = PathName;

		//MessageBox((LPCTSTR)PathName, _T("Constant Contact Registration Filename"));
		constant_contact_file_edit.SetWindowText(PathName);
		GetDocument()->SetModifiedFlag();
	}
}


void CCCSwisssys2View::OnEnChangeEdit1()
{
	CString sWindowText;
	ratings_file_edit.GetWindowText(sWindowText);
	GetDocument()->ratings_file = sWindowText;
	GetDocument()->SetModifiedFlag();
#if 1
	if (!GetDocument()->ratings_file.IsEmpty() && !GetDocument()->loadRatingsFile()) {
		GetDocument()->ratings_file = L"";
		ratings_file_edit.SetWindowText(L"");
	}
#endif
}


void CCCSwisssys2View::OnEnChangeEdit2()
{
	CString sWindowText;
	constant_contact_file_edit.GetWindowText(sWindowText);
	GetDocument()->constant_contact_file = sWindowText;
	GetDocument()->SetModifiedFlag();
}

void addToSectionList(CListCtrl &section_list, Section &newSection, int position) {
	LVITEM lvItem;
	int nItem;

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = position;
	lvItem.iSubItem = 0;
	lvItem.pszText = newSection.name.GetBuffer();
	nItem = section_list.InsertItem(&lvItem);

	if (newSection.usedRatings()) {
		std::wstringstream ss;
		ss << newSection.lower_rating_limit;
		section_list.SetItemText(nItem, 1, CString(ss.str().c_str()));
		ss.str(L"");
		ss << newSection.upper_rating_limit;
		section_list.SetItemText(nItem, 2, CString(ss.str().c_str()));
	}
	if (newSection.usedGrade()) {
		section_list.SetItemText(nItem, 3, getGradeString(newSection.lower_grade_limit).GetBuffer());
		section_list.SetItemText(nItem, 4, getGradeString(newSection.upper_grade_limit).GetBuffer());
	}

	section_list.SetItemText(nItem, 5, getSectionTypeString(newSection.sec_type));

	if (newSection.players.size() > 0) {
		std::wstringstream ss;
		ss << newSection.players.size();
		section_list.SetItemText(nItem, 6, CString(ss.str().c_str()));
	}

	if (newSection.num_subsections > 1) {
		std::wstringstream ss;
		ss << newSection.num_subsections;
		section_list.SetItemText(nItem, 7, CString(ss.str().c_str()));
		ss.str(L"");
	}

	{
		std::wstringstream ss;
		ss << newSection.which_computer;
		section_list.SetItemText(nItem, 8, CString(ss.str().c_str()));
		ss.str(L"");
	}
}

void CCCSwisssys2View::refillSections(Sections &s, bool clear) {
	unsigned i;
	section_list.DeleteAllItems();
	for (i = 0; i < s.size(); ++i) {
		addToSectionList(section_list, s[i], i);
	}
	section_list.SetSelectionMark(-1);
	section_players.DeleteAllItems();
	if (clear) {
		auto pDoc = GetDocument();
		pDoc->sections.clearPlayers();
	}
}

// When they click the add section button.
void CCCSwisssys2View::OnAddSection()
{
	Section newSection;
	while (true) {
		bool check;
		SectionEditor se(&newSection, check);
		INT_PTR nRet = se.DoModal();
		if (nRet != IDOK) {
			return;
		}

		auto pDoc = GetDocument();
		//MessageBox(_T("Debugging"), _T("User pressed OK button on dialog."));

		int conflict = pDoc->sections.conflicts(newSection);

		if (conflict >= 0) {
			//std::wstringstream ss;
			CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
			//ss << "New section conflicts with existing section with name " << pDoc->sections[conflict].name << ".";
			//MessageBox(CString(ss.str().c_str()), _T("Section Conflict"));
			MessageBox(ss, _T("Section Conflict"));
			continue;
		}

		pDoc->sections.push_back(newSection);
		refillSections(pDoc->sections);
		pDoc->SetModifiedFlag();
		return;
	}
}

// Click on the delete section button.
void CCCSwisssys2View::OnDeleteSection()
{
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.DeleteItem(selected_row);
		auto pDoc = GetDocument();
		pDoc->sections.erase(pDoc->sections.begin() + selected_row);
		refillSections(pDoc->sections);
		pDoc->SetModifiedFlag();
	}
}


// Click on the edit section button.
void CCCSwisssys2View::OnEditSection()
{
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.SetSelectionMark(-1);
		auto pDoc = GetDocument();
		Section newSection = pDoc->sections[selected_row];
		while (true) {
			bool check;
			SectionEditor se(&newSection, check);
			INT_PTR nRet = se.DoModal();
			if (nRet != IDOK) {
				return;
			}

			//MessageBox(_T("Debugging"), _T("User pressed OK button on dialog."));

			if (check) {
				int conflict = pDoc->sections.conflicts(newSection, selected_row);

				if (conflict >= 0) {
					//std::wstringstream ss;
					CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
					//ss << "New section conflicts with existing section with name " << pDoc->sections[conflict].name << ".";
					//MessageBox(CString(ss.str().c_str()), _T("Section Conflict"));
					MessageBox(ss, _T("Section Conflict"));
					continue;
				}
			}

			pDoc->sections[selected_row] = newSection;
			refillSections(pDoc->sections, check);
			pDoc->SetModifiedFlag();
			return;
		}
	}
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

void updateUscfFromRatingFile(std::wstring &uscf_id, std::wstring &uscf_rating, const Player &p, std::wofstream &normal_log, bool &warning_condition) {
	if (p.uscf_id != L"") {
		std::wstring temp_rating = p.uscf_rating;

		if (uscf_id.size() != 0 && uscf_id != p.uscf_id) {
			warning_condition = true;
			normal_log << "WARNING: USCF ID in constant contact " << uscf_id << " not the same as the one in the ratings file " << p.uscf_id << " for player " << p.first_name << " " << p.last_name << std::endl;
		}
		if (uscf_rating.size() != 0 && uscf_rating != temp_rating) {
			//warning_condition = true;
			//normal_log << "WARNING: USCF rating in constant contact " << uscf_rating << " not the same as the one in the ratings file " << temp_rating << std::endl;
		}

		uscf_id = p.uscf_id;
		uscf_rating = temp_rating;
	}
	else {
		if (isNumeric(uscf_id) && uscf_id.length() == 8) {
			warning_condition = true;
			normal_log << "WARNING: USCF ID specified " << uscf_id << " but no USCF ID present in the ratings file for player " << p.last_name << " " << p.first_name << " grade code = " << p.grade << std::endl;
		}
		else {
			uscf_id = L"";
		}
	}
}

std::vector<int> findExactMatch(const std::wstring &last, const std::wstring &first, wchar_t grade, const std::vector<Player> &rated_players) {
	unsigned i;
	std::vector<int> ret;

	for (i = 0; i < rated_players.size(); ++i) {
		if (last == rated_players[i].last_name &&
			first == rated_players[i].first_name 
			//&& grade == rated_players[i].grade
			) {
			ret.push_back(i);
		}
	}
	return ret;
}

std::wstring getLastFour(const std::wstring &full) {
	int index = max(0, (int)full.length() - 4);
	return full.substr(index);
}

//#define DEBUG_MAIN

std::vector<SectionPlayerInfo> process_cc_file(HWND hWnd, CCCSwisssys2Doc *pDoc, bool &error_condition, bool &warning_condition, bool &info_condition, std::wofstream &normal_log) {
	std::vector<SectionPlayerInfo> post_proc;

	std::wifstream cc_file(pDoc->constant_contact_file);
	bool use_cc = true;
	if (!cc_file) {
		use_cc = false;
		//MessageBox(_T("Could not load constant contact registration file."), _T("Constant contact file incorrect."));
		//return;
	}
	cc_file.close();

	unsigned i;

	if (use_cc) {
		std::wstring ccsstr = CStringToWString(pDoc->constant_contact_file);
		auto entries = load_constant_contact_file(ccsstr, pDoc->nwsrs_four_map, pDoc->uscf_map, pDoc->rated_players, pDoc->school_codes, normal_log);
		if (entries.size() == 0) {
			MessageBox(hWnd, _T("No players loaded from constant contact file."), _T("ERROR"), MB_OK);
			return post_proc;
		}


		unsigned num_parents = 0, num_players = 0;

		for (i = 0; i < entries.size(); ++i) {
			bool was_parent = false, was_player = false;

			//        std::cout << entries[i].getFirstName() << " " << entries[i].getLastName() << " " << entries[i].didAdultCheck() << " school=" << entries[i].getSchool() << " grade=" << entries[i].getGrade() << " section=" << entries[i].getSection() << " nwsrsid=" << entries[i].getNwsrsId() << std::endl;
			if (entries[i].isParent()) {
				++num_parents;
				was_parent = true;
			}
			if (entries[i].isPlayer()) {
				++num_players;
				was_player = true;
			}
			if (was_parent && was_player) {
				MessageBox(hWnd, _T("Some entry was both parent and player."), _T("ERROR"), MB_OK);
				normal_log << "ERROR: entry both parent and player. " << entries[i].getFirstName() << " " << entries[i].getLastName() << std::endl;
			}
			if (!was_parent && !was_player) {
				MessageBox(hWnd, _T("Some entry was neither parent nor player."), _T("ERROR"), MB_OK);
				normal_log << "ERROR: entry neither parent anorplayer. " << entries[i].getFirstName() << " " << entries[i].getLastName() << std::endl;
			}
		}

		//	std::cout << "There are " << entries.size() << " entries in the file." << std::endl;
		//	std::cout << "There are " << num_parents << " parents." << std::endl;
		//	std::cout << "There are " << num_players << " players." << std::endl;
		if (num_parents + num_players != entries.size()) {
			MessageBox(hWnd, _T("Some problem where number of parents plus number of players not equal to number of entries."), _T("ERROR"), MB_OK);
			//ShellExecute(NULL, _T("open"), _T("c:\\windows\\notepad.exe"), (LPCWSTR)logfilename.c_str(), _T(""), SW_SHOWNORMAL);
			return post_proc;
		}

		for (i = 0; i < entries.size(); ++i) {
			if (entries[i].isPlayer() && pDoc->noshows.find(i) == pDoc->noshows.end()) {
#ifdef DEBUG_MAIN
				normal_log << std::endl;
#endif

				std::wstring last_name = entries[i].getLastName();
				std::wstring first_name = entries[i].getFirstName();
				std::wstring school = entries[i].getSchool();
				std::wstring orig_school = school;
				std::wstring full_id = entries[i].getNwsrsId();
				//std::wstring given_rating = entries[i].getNwsrsRating();
				std::wstring upper_last = last_name;
				std::wstring upper_first = first_name;
				std::wstring uscf_id = entries[i].getUscfId();
				//std::wstring uscf_rating = entries[i].getUscfRating();
				std::wstring uscf_rating = L"";
				wchar_t grade_code = entries[i].getGradeCode();
				SCHOOL_TYPE st = getSchoolType(grade_code);
				std::wstringstream notes;
				bool unrated = false;

				upper_last = toUpper(upper_last);
				upper_first = toUpper(upper_first);
				full_id = toUpper(full_id);

				if (upper_first == L"ANIKA") {
					upper_first = L"ANIKA";
				}

				// Handle ID specification input differences and errors.
				if (full_id == L"NONE" || (full_id.size() != 0 && full_id.size() != 4 && full_id.size() != 8)) {
					full_id = L"";

					warning_condition = true;
					if (full_id != L"NONE" && full_id.size() != 0) {
						normal_log << "WARNING: Specified ID " << full_id << " for player " << first_name << " " << last_name << " is malformed.  Ignoring the ID and trying to find ID from name." << std::endl;
					}
				}

				// If correctly specified, get the unchanging unique part of the ID (the last 4 characters)
				std::wstring last_four_id;
				if (full_id.length() >= 4) {
					last_four_id = getLastFour(full_id);

				}

				std::wstring school_code = L"";

				std::wstring ratings_school_code = L"";
				if (full_id.length() == 8) {
					ratings_school_code = full_id.substr(0, 3);
				}

#ifdef DEBUG_MAIN
				normal_log << "Processing " << first_name << " " << last_name << " " << school << " " << full_id << " " << "last 4: " << last_four_id << " school code: " << ratings_school_code << " " << grade_code << " " << st << std::endl;
#endif

				if (school == L"") {
					school_code = ratings_school_code;
					school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
					normal_log << "school not specified...getting it from school code" << school << std::endl;
#endif
				}
				else if (school.length() == 3) {
					school_code = toUpper(school);
					school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
					normal_log << "school length is 3 so assuming school code " << school_code << std::endl;
#endif
				}
				else if (pDoc->school_codes.isExactNoSchool(school, ratings_school_code) &&
					pDoc->school_codes.schoolIsType(ratings_school_code, st)) {
					school_code = ratings_school_code;
				}
				else {
					school_code = pDoc->school_codes.findCodeFromSchoolExactNoSchool(school, st);
					if (school_code == L"") {
#ifdef DEBUG_MAIN
						normal_log << "school not found exactly without school " << school << std::endl;
#endif
						auto prev_iter = pDoc->saved_school_corrections.find(school);
						if (prev_iter != pDoc->saved_school_corrections.end()) {
							school_code = prev_iter->second;
							school = pDoc->school_codes.findName(school_code);
						}
						else {
							if (school == L"") {
								error_condition = true;
								normal_log << "ERROR: no school or NWSRS ID entered for player " << last_name << " " << first_name << " " << std::endl;
								continue;
							}
							else {
								SchoolSelector ss_dialog(pDoc, school, ratings_school_code, school_code);

								if (ss_dialog.DoModal() == IDOK) {
									school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
									normal_log << "school code after asking user " << school_code << " " << school << std::endl;
#endif
								}
								else {
									error_condition = true;
									normal_log << "ERROR: no school code correction entered for " << school << " for player " << last_name << " " << first_name << " " << std::endl;
									continue;
								}
							}
						}
					}
				}

				if (full_id.length() == 8 && school_code != L"" && ratings_school_code != school_code) {
					info_condition = true;
					normal_log << "INFO: Player " << first_name << " " << last_name << " with ID specified as " << full_id << " and school as " << orig_school << " change of school from " << ratings_school_code << " to " << school_code << std::endl;

					full_id.replace(0, 3, school_code); // Update school code in their ID.
				}

				CString cs_full_id = CString(full_id.c_str());

				auto rentry = pDoc->nwsrs_map.find(full_id);
				unsigned cc_rating = 4000;
				unsigned nwsrs_rating;

				if (rentry != pDoc->nwsrs_map.end()) {
					cc_rating = pDoc->rated_players[rentry->second].get_higher_rating();
					updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[rentry->second], normal_log, warning_condition);
					nwsrs_rating = pDoc->rated_players[rentry->second].nwsrs_rating;
					std::wstring cap_db_last = toUpper(pDoc->rated_players[rentry->second].last_name);
					std::wstring cap_db_first = toUpper(pDoc->rated_players[rentry->second].first_name);
					if (cap_db_last != toUpper(last_name) || cap_db_first != toUpper(first_name)) {
						normal_log << "NAME: player named " << toUpper(last_name) << " " << toUpper(first_name) << " is named differently in the NWSRS database as " << cap_db_last << " " << cap_db_first << std::endl;
						notes << "Update name in database from " << cap_db_first << " " << cap_db_last << "? ";
					}
				}
				else {
					if (full_id == L"NONE" || full_id == L"N/A" || full_id == L"") {
						std::vector<int> exact_match = findExactMatch(upper_last, upper_first, grade_code, pDoc->rated_players);
						bool match_found = false;
						for (auto match = exact_match.begin(); match != exact_match.end(); ++match) {
							std::wstringstream wss;
							wss << "No ID specified for player " << upper_first << " " << upper_last << " grade code: " << grade_code << " school: " << school
								<< ".  However, a player with the same first and last name was found in the ratings file with ID: " << pDoc->rated_players[*match].getFullId() <<
								" and grade code: " << pDoc->rated_players[*match].grade <<
								" and school code: " << pDoc->rated_players[*match].school_code << "(" <<
								pDoc->school_codes.findName(pDoc->rated_players[*match].school_code) << ")" <<
								" Please verify if this is the same person and click Yes if they are and No if they aren't.";
							int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);

							if (ret == IDYES) {
								warning_condition = true;
								//normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;
								normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;

								full_id = pDoc->rated_players[*match].getFullId();
								cc_rating = pDoc->rated_players[*match].get_higher_rating();
								updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[*match], normal_log, warning_condition);
								nwsrs_rating = pDoc->rated_players[*match].nwsrs_rating;
								match_found = true;
							}
						}
						if (!match_found) {
							full_id = school_code + grade_code;
							cc_rating = (grade_code - 'A') * 100;
							nwsrs_rating = cc_rating;
							unrated = true;

							info_condition = true;
							normal_log << "INFO: new tournament player without a previous ID " << first_name << " " << last_name << ", grade: " << grade_code << ", school: " << school << std::endl;
						}
					}
					else {
						// They have an ID but the exact ID wasn't found in the list.  So, search by partial ID.
						bool found = false;
						for (unsigned j = 0; j < pDoc->rated_players.size(); ++j) {
							if (pDoc->rated_players[j].id == last_four_id) {
								cc_rating = pDoc->rated_players[j].get_higher_rating();
								updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[j], normal_log, warning_condition);
								nwsrs_rating = pDoc->rated_players[j].nwsrs_rating;

								//normal_log << "Found by digit ID string from ratings file " << std::endl;
								if (pDoc->rated_players[j].grade != grade_code) {
									notes << "Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[j].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << ". ";
									grade_code = pDoc->rated_players[j].grade;
								}
								if (school_code.length() == 3) {
									full_id = school_code + pDoc->rated_players[j].grade + last_four_id;
									//									full_id = school_code + grade_code + last_four_id;
								}
								else {
									full_id = pDoc->rated_players[j].school_code + pDoc->rated_players[j].grade + last_four_id;
								}
								found = true;
								break;
							}
						}
						// Do somethere here if the player was not found.
						if (!found) {
							error_condition = true;
							normal_log << "ERROR: Did not find player ID " << last_four_id << " in the ratings file. " << full_id << " " << last_name << " " << first_name << " " << grade_code << " " << school << std::endl;
						}
					}
				}

				CString cs_last_name(last_name.c_str());
				CString cs_first_name(first_name.c_str());
				CString cs_school(school.c_str());
				CString cs_school_code(school_code.c_str());
				cs_full_id = full_id.c_str();

				post_proc.push_back(SectionPlayerInfo(-((int)i+1), cs_last_name, cs_first_name, cs_full_id, nwsrs_rating, grade_code, cs_school, cs_school_code, CString(uscf_id.c_str()), CString(uscf_rating.c_str()), notes.str(), unrated, cc_rating));
			}
		}
	}

	normal_log << "\nOnce you have confirmed that you have resolved INFO and WARNING messages correctly, please delete those lines before sending to the ratings coordinator." << std::endl;

	return post_proc;
}

// Click on the button to load sections from constant contact file.
void CCCSwisssys2View::OnCreateSections()
{
	auto pDoc = GetDocument();

	pDoc->sections.clearPlayers();
	section_players.DeleteAllItems();

	std::wifstream rfile(pDoc->ratings_file);
	if (!rfile) {
		MessageBox(_T("Could not load ratings file."), _T("Ratings data file not found."));
		return;
	}
	rfile.close();

	std::wstring output_dir = pDoc->getOutputLocation();
	std::wstring logfilename = output_dir + _T("\\ccswslog.txt");
	std::wofstream normal_log(logfilename);

	Player p;
	std::set<std::wstring> restricted_set;

	bool error_condition = false, warning_condition = false, info_condition = false;

	unsigned player_index = 0;

	if (pDoc->restricted_file != L"") {
		std::wifstream rfile(pDoc->restricted_file);
		std::wstring rid;
		while (!rfile.eof()) {
			rfile >> rid;
			if (!rfile.eof()) {
				if (rid.length() == 8) {
					restricted_set.insert(getLastFour(rid));
				}
				else {
					error_condition = true;
					normal_log << "ERROR: 8-digit ID not found in restricted file.  Found the following instead: " << rid << std::endl;
				}
			}
		}
	}

	auto post_proc = process_cc_file(this->GetSafeHwnd(), pDoc, error_condition, warning_condition, info_condition, normal_log);

    for(auto ppiter = post_proc.begin(); ppiter != post_proc.end(); ++ppiter) {
		auto force_iter = pDoc->force_sections.find(ppiter->cc_file_index);
		int target_section = pDoc->sections.foundIn(ppiter->cc_rating, ppiter->grade);
		if (force_iter != pDoc->force_sections.end()) {
			target_section = pDoc->sections.findByName(force_iter->second);
		}
		if (target_section == -1) {
			error_condition = true;
			normal_log << "ERROR: Doesn't belong in any section. " << ppiter->full_id << " " << ppiter->last_name << " " << ppiter->first_name << " " << ppiter->cc_rating << " " << ppiter->grade << " " << ppiter->school << std::endl;
		}
		else if (target_section < -1) {
			error_condition = true;
			normal_log << "ERROR: Player can go in multiple sections. " << ppiter->full_id << " " << ppiter->last_name << " " << ppiter->first_name << " " << ppiter->cc_rating << " " << ppiter->grade << " " << ppiter->school << std::endl;
		}
		else {
			if (restricted_set.size() == 0 || restricted_set.find(getLastFour(CStringToWString(ppiter->full_id))) != restricted_set.end()) {
				pDoc->sections[target_section].players.push_back(*ppiter);
			}
			else {
				error_condition = true;
				normal_log << "ERROR: Player in constant contact not in the restricted tournament id list. " << ppiter->full_id << " " << ppiter->last_name << " " << ppiter->first_name << " " << ppiter->cc_rating << " " << ppiter->grade << " " << ppiter->school << std::endl;
			}
			//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << full_id << " " << last_name << " " << first_name << " " << cc_rating << " " << grade_code << " " << school_code << " uscf rating and id " << uscf_rating << " " << uscf_id << std::endl;
		}
	}

	unsigned i;
	// Add "manage additional registration" players to sections.
	for (i = 0; i < pDoc->mrplayers.size(); ++i) {
		MRPlayer &this_player = pDoc->mrplayers[i];
		int target_section = pDoc->sections.foundIn(pDoc->mrplayers[i].nwsrs_rating, pDoc->mrplayers[i].grade);
		if (target_section == -1) {
			error_condition = true;
			normal_log << "ERROR: Doesn't belong in any section. " << this_player.ws_id << " " << this_player.nwsrs_rating << " " << this_player.ws_last << " " << this_player.ws_first << " " << " " << this_player.grade << " " << this_player.ws_school_code << " " << this_player.ws_school_name << std::endl;
		}
		else if (target_section < -1) {
			error_condition = true;
			normal_log << "ERROR: Player can go in multiple sections. " << this_player.ws_id << " " << this_player.nwsrs_rating << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_code << " " << this_player.ws_school_name << std::endl;
		}
		else {
			if (restricted_set.size() == 0 || restricted_set.find(getLastFour(this_player.ws_id)) != restricted_set.end()) {
				CString cs_last(this_player.ws_last.c_str());
				CString cs_first(this_player.ws_first.c_str());
				CString cs_id(this_player.ws_id.c_str());
				CString cs_school_name(this_player.ws_school_name.c_str());
				CString cs_school_code(this_player.ws_school_code.c_str());
				CString cs_uscf_id(this_player.ws_uscf_id.c_str());
				CString cs_uscf_rating(this_player.ws_uscf_rating.c_str());

				pDoc->sections[target_section].players.push_back(SectionPlayerInfo(i, cs_last, cs_first, cs_id, this_player.nwsrs_rating, this_player.grade, cs_school_name, cs_school_code, cs_uscf_id, cs_uscf_rating, L"", false, this_player.nwsrs_rating));
				//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << this_player.ws_id << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_code << " uscf rating and id " << this_player.ws_uscf_rating << " " << this_player.ws_uscf_id << std::endl;
			}
			else {
				error_condition = true;
				normal_log << "ERROR: Player in constant contact not in the restricted tournament id list. " << this_player.ws_id << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_name << std::endl;
			}
		}
	}

	pDoc->sections.makeSubsections();

	if (pDoc->school_codes.m_new_schools.size() > 0) {
		info_condition = true;
		normal_log << "\nNew school codes created for this tournament.  New school information is below:" << std::endl;
		for (i = 0; i < pDoc->school_codes.m_new_schools.size(); ++i) {
			normal_log << pDoc->school_codes.m_new_schools[i].getSchoolCode() << "," <<
				pDoc->school_codes.m_new_schools[i].getSchoolName() << "," <<
				pDoc->school_codes.m_new_schools[i].getSchoolType() << "," <<
				pDoc->school_codes.m_new_schools[i].getSchoolCity() << "," <<
				pDoc->school_codes.m_new_schools[i].getSchoolState() << std::endl;
		}
		normal_log << std::endl;
	}

	normal_log.close();

	if (error_condition) {
		MessageBox(_T("Some players not added to a section.  Please read the log."), _T("Error"));
	}
	else if (warning_condition) {
		MessageBox(_T("Some potential issues were found.  Please read the log and report to ratings coordinator."), _T("Warning"));
	}
	else if (info_condition) {
		MessageBox(_T("Log contains information to report to ratings coordinator.  Please review the log."), _T("Warning"));
	}
	else {
		MessageBox(_T("All players added to sections successfully."), _T("Success"));
	}

	if (error_condition || warning_condition || info_condition) {
		ShellExecute(NULL, _T("open"), _T("c:\\windows\\notepad.exe"), (LPCWSTR)logfilename.c_str(), _T(""), SW_SHOWNORMAL);
	}

	for (i = 0; i < pDoc->sections.size(); ++i) {
		std::wstringstream ss;
		ss << pDoc->sections[i].players.size();
		section_list.SetItemText(i, 6, CString(ss.str().c_str()));
	}
//	section_list.SetItemState(, ~LVIS_SELECTED, LVIS_SELECTED);
//	section_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
//	section_list.SetSelectionMark(0);
	int id = section_list.GetNextItem(-1, LVNI_FOCUSED);
	if (id != -1) {
		section_list.SetItemState(id, 0, LVIS_FOCUSED | LVIS_SELECTED);
	}
}


void CCCSwisssys2View::OnSectionColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
}


void CCCSwisssys2View::OnSectionClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState & LVIS_SELECTED) {
		int selectedItem = pNMLV->iItem;
		auto pDoc = GetDocument();
		section_players.DeleteAllItems();

		reset_section_sort();
		last_section_sort = -1;

		for (unsigned i = 0; i < pDoc->sections[selectedItem].players.size(); ++i) {
			LVITEM lvItem;
			int nItem;

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = i;
			lvItem.iSubItem = 0;
			lvItem.pszText = pDoc->sections[selectedItem].players[i].last_name.GetBuffer();
			nItem = section_players.InsertItem(&lvItem);

			section_players.SetItemText(i, 1, pDoc->sections[selectedItem].players[i].first_name.GetBuffer());
			std::wstringstream ss;
			ss << pDoc->sections[selectedItem].players[i].rating;
			section_players.SetItemText(i, 2, CString(ss.str().c_str()));
			ss.str(L"");

			section_players.SetItemText(i, 3, pDoc->sections[selectedItem].players[i].full_id.GetBuffer());
			section_players.SetItemText(i, 4, getGradeString(pDoc->sections[selectedItem].players[i].grade));
			section_players.SetItemText(i, 5, pDoc->sections[selectedItem].players[i].school.GetBuffer());
			section_players.SetItemText(i, 6, pDoc->sections[selectedItem].players[i].uscf_rating.GetBuffer());
			section_players.SetItemText(i, 7, pDoc->sections[selectedItem].players[i].uscf_id.GetBuffer());

			if (pDoc->sections[selectedItem].num_subsections > 1) {
				ss << pDoc->sections[selectedItem].players[i].subsection;
				section_players.SetItemText(i, 8, CString(ss.str().c_str()));
				ss.str(L"");
			}

			section_players.SetItemData(i, (DWORD_PTR)&pDoc->sections[selectedItem].players[i]);
		}
		pDoc->SetModifiedFlag();
	}
	*pResult = 0;
}


void CCCSwisssys2View::OnCreateClubFiles()
{
	auto pDoc = GetDocument();
	std::wstring output_dir = pDoc->getOutputLocation(); // pDoc->CDocument->m_strPathName

	unsigned total_placed = 0;;
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		if (pDoc->sections[j].players.size() == 0) {
			continue;
		}
			
		unsigned i;
		std::vector<std::wofstream> section_files;
		for (i = 0; i < pDoc->sections[j].num_subsections; ++i) {
			std::wstring filename;
			if (pDoc->sections[j].num_subsections == 1) {
				filename = output_dir + L"\\Section_" + CStringToWString(pDoc->sections[j].name) + L".txt";
			}
			else {
				filename = output_dir + L"\\Section_" + CStringToWString(pDoc->sections[j].name) + L"_" + wchar_t('A'+i) + L".txt";
			}
			section_files.push_back(std::wofstream(filename));
		}

		//std::wstring filename = output_dir + L"\\Section_" + CStringToWString(pDoc->sections[j].name) + L".txt";
		//std::wofstream section_file(filename);

		for (i = 0; i < pDoc->sections[j].players.size(); ++i) {
			std::wofstream &section_file = section_files[pDoc->sections[j].players[i].subsection - 1];
			section_file << L"NAME = " << toUpper(CStringToWString(pDoc->sections[j].players[i].last_name)) << L", " <<
				toUpper(CStringToWString(pDoc->sections[j].players[i].first_name)) << std::endl;
			section_file << "RATING = " << pDoc->sections[j].players[i].rating << std::endl;
			section_file << "ID# = " << CStringToWString(pDoc->sections[j].players[i].full_id) << std::endl;
			std::wstring school_code_ws = CStringToWString(pDoc->sections[j].players[i].school_code);
			if (school_code_ws != L"HSO") {
				section_file << "TEAM = " << school_code_ws << std::endl;
			}
			if (!pDoc->sections[j].players[i].uscf_id.IsEmpty()) {
				section_file << "ID2 = " << CStringToWString(pDoc->sections[j].players[i].uscf_id) << std::endl;
			}
			if (!pDoc->sections[j].players[i].uscf_rating.IsEmpty()) {
				section_file << "RTNG2 = " << CStringToWString(pDoc->sections[j].players[i].uscf_rating) << std::endl;
			}
			section_file << "AGE = " << CStringToWString(getGradeStringShort(pDoc->sections[j].players[i].grade));
			section_file << std::endl;

			++total_placed;
		}
	}
	if (total_placed == 0) {
		MessageBox(_T("No players in any section.  Please click the Create Sections button first."), _T("Error"));
	}
	else {
		MessageBox(_T("Swisssys files created successfully."), _T("Information"));
	}
}


void CCCSwisssys2View::OnSchoolCodesFileBrowse()
{
	CFileDialog FileDialog(TRUE, _T("csv"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		auto pDoc = GetDocument();

		pDoc->school_code_file = PathName;

		SchoolCodesEdit.SetWindowText(PathName);

		pDoc->school_codes.Load(CStringToWString(pDoc->school_code_file));
		if (pDoc->school_codes.empty()) {
			MessageBox(_T("No school codes found in file.  Please select a code file in csv format."), _T("Error"));
		}
		pDoc->SetModifiedFlag();
	}
}


void CCCSwisssys2View::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString sWindowText;
	SchoolCodesEdit.GetWindowText(sWindowText);
	GetDocument()->school_code_file = sWindowText;
	GetDocument()->SetModifiedFlag();
}


void CCCSwisssys2View::OnSectionReorder(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	int smark = section_list.GetSelectionMark();
	if (smark < 0) {
		return;
	}

	auto pDoc = GetDocument();
	int slen = (int)pDoc->sections.size();
	int first, second;

	if (pNMUpDown->iDelta == -1) {
		// Move a section up.
		if (smark < 1) {
			return;
		}
		first = smark - 1;
		second = smark;
	} else if (pNMUpDown->iDelta == 1) {
		// Move a section down.
		if (smark >= slen - 1) {
			return;
		}
		first = smark;
		second = smark + 1;
	}
	else {
		MessageBox(_T("spin control idelta was not -1 or 1"), _T("ERROR"));
		return;
	}

	std::iter_swap(pDoc->sections.begin() + first, pDoc->sections.begin() + second);
	refillSections(pDoc->sections);
	pDoc->SetModifiedFlag();
}

template <class T>
int listCompareItem(T &a, T &b, bool rev=false) {
	if (a < b) return rev ? 1 : -1;
	if (a > b) return rev ? -1 : 1;
	return 0;
}

int listCompareItemInt(int a, int b, bool rev=false) {
	if (a < b) return rev ? 1 : -1;
	if (a > b) return rev ? -1 : 1;
	return 0;
}

class spc_sort_info {
public:
	int column;
	bool rev;
	spc_sort_info(int c, bool r) : column(c), rev(r) {}
};

int CALLBACK section_players_compare(LPARAM l1, LPARAM l2, LPARAM lsort) {
	SectionPlayerInfo *p1 = (SectionPlayerInfo*)l1;
	SectionPlayerInfo *p2 = (SectionPlayerInfo*)l2;
	spc_sort_info *sort_info = (spc_sort_info*)lsort;
	switch (sort_info->column) {
	case 0:
		return listCompareItem(p1->last_name, p2->last_name, sort_info->rev);
	case 1:
		return listCompareItem(p1->first_name, p2->first_name, sort_info->rev);
	case 2:
		return listCompareItemInt(p1->rating, p2->rating, sort_info->rev);
	case 3:
		return listCompareItem(p1->full_id, p2->full_id, sort_info->rev);
	case 4:
		return listCompareItem(p1->grade, p2->grade, sort_info->rev);
	case 5:
		return listCompareItem(p1->school, p2->school, sort_info->rev);
	case 6:
	  {
		std::wstring p1_rating = CStringToWString(p1->uscf_rating);
		if (p1_rating == L"") p1_rating = L"0";
		std::wstring p2_rating = CStringToWString(p2->uscf_rating);
		if (p2_rating == L"") p2_rating = L"0";
		return listCompareItemInt(std::stoi(p1_rating), std::stoi(p2_rating), sort_info->rev);
	  }
	case 7:
		return listCompareItem(p1->uscf_id, p2->uscf_id, sort_info->rev);
	default:
		return 0;
	}
}

void CCCSwisssys2View::OnSectionPlayersColumnSort(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	int column = pNMLV->iSubItem;
	if (column == last_section_sort) {
		section_sort[column] = !section_sort[column];
	}
	else {
		reset_section_sort();
	}
	last_section_sort = column;
	spc_sort_info sort_info(column, section_sort[column]);
	section_players.SortItems(section_players_compare, (LPARAM)&sort_info);
}

void CCCSwisssys2View::OnManageAdditionalRegistrations()
{
	ManageRegistrations mr_dialog(GetDocument());
	mr_dialog.DoModal();
}

void addToSwisssysSection(std::wofstream &tmt_file, const std::wstring &secname, SECTION_TYPE sec_type) {
	tmt_file << "[" << secname << "]" << std::endl;
	tmt_file << "Section title = " << secname << std::endl;
	if (sec_type == SWISS) {
		tmt_file << "Event type = 0" << std::endl;
	}
	else {
		tmt_file << "Event type = 3" << std::endl;
	}
	tmt_file << "Double blitz = 0" << std::endl;
	tmt_file << "Coin toss = 1" << std::endl;
	tmt_file << "Ratings = 0" << std::endl;
	tmt_file << "Time controls =" << std::endl;
	tmt_file << "Minimum rating = 0" << std::endl;
	tmt_file << "Maximum rating = 9999" << std::endl;
	tmt_file << "Minimum grade = 0" << std::endl;
	tmt_file << "Maximum grade = 99" << std::endl;
	tmt_file << "Starting board = 1" << std::endl;
	tmt_file << "Rounds = 0" << std::endl;
	tmt_file << "Exclude batch = 0" << std::endl;
	tmt_file << "Exclude ratings = 0" << std::endl;
	tmt_file << "Exclude Krause = 0" << std::endl;
	tmt_file << "Rounds paired = 0" << std::endl;
}

std::wstring baseToNameWithSub(const std::wstring &base, unsigned sub) {
	wchar_t subid = L'A' + sub;
	std::wstring subidws(1, subid);
	std::wstring subsecname = base + L" - " + subidws;
	return subsecname;
}

#include "xlslib.h"
using namespace xlslib_core;

std::string toString(const std::wstring &ws) {
	std::string s(ws.begin(), ws.end());
	return s;
}

unsigned CCCSwisssys2View::createSectionWorksheet(
	std::wofstream &normal_log,
	const std::wstring &output_dir, 
	std::wstring &sec_name, 
	const Section &sec, 
	int subsec) {

	workbook wb;
	worksheet* settings = wb.sheet("Settings");
	worksheet* players = wb.sheet("Players");
	worksheet* results = wb.sheet("Results");
	worksheet* tables = wb.sheet("Tables");

	workbook checkin_wb;
	worksheet* checkin = checkin_wb.sheet("Check-In");
	checkin->defaultColwidth(8);
	checkin->colwidth(0, 256 * 40);
	std::wstringstream header;
	header << "Check-In List for Section " << sec_name;
	checkin->label(0, 0, header.str());
	checkin->label(2, 0, "Player");
	checkin->label(2, 1, "Present");
	checkin->label(2, 2, "Notes");

	unsigned i;
	unsigned num_players = 0;
	for (i = 0; i < sec.players.size(); ++i) {
		if (sec.players[i].subsection == subsec) {
			++num_players;
		}
	}

	settings->label(0, 0, "FILE_TYPE");     settings->label(1, 0, "SWISSSYS");
	settings->label(0, 1, "VERSION");       settings->label(1, 1, "9.14");
	settings->label(0, 2, "PLR_CNT");       settings->number(1, 2, num_players);
	settings->label(0, 3, "RDS_PLD");       settings->number(1, 3, 0);
	settings->label(0, 4, "RDS_PRD");       settings->number(1, 4, 0);
	settings->label(0, 5, "BYES");
	settings->label(0, 6, "TM_BYES");
	settings->label(0, 7, "QUICK");         settings->number(1, 7, 0);
	settings->label(0, 8, "1ST_BD");        settings->number(1, 8, 1);
	settings->label(0, 9, "LOW");           settings->number(1, 9, 0);
	settings->label(0, 10, "HIGH");         settings->number(1, 10, 9999);
	settings->label(0, 11, "LAST_RD");      settings->number(1, 11, 0);
	settings->label(0, 12, "COIN");         settings->number(1, 12, 10);
	settings->label(0, 13, "TM_CUT");       settings->number(1, 13, 999);
	settings->label(0, 14, "ALL_IN");       settings->label(1, 14, "TRUE");
	settings->label(0, 15, "1ST_TIME");     settings->label(1, 15, "TRUE");
	settings->label(0, 16, "SORTED");       settings->label(1, 16, "FALSE");
	settings->label(0, 17, "ACCEL");        settings->label(1, 17, "FALSE");
	settings->label(0, 18, "USE_TMS");      settings->label(1, 18, "FALSE");
	settings->label(0, 19, "BLITZ");        settings->label(1, 19, "FALSE");
	settings->label(0, 20, "TM_PAIRS");     settings->label(1, 20, "FALSE");
	settings->label(0, 21, "FR_TM");        settings->label(1, 21, "FALSE");
	settings->label(0, 22, "R_ROBIN");      settings->label(1, 22, sec.sec_type == ROUND_ROBIN ? "TRUE" : "FALSE");
	settings->label(0, 23, "TBL_SET");      settings->label(1, 23, "FALSE");
	settings->label(0, 24, "BERGER");       settings->label(1, 24, "FALSE");
	settings->label(0, 25, "FR_TBL");       settings->label(1, 25, "FALSE");
	settings->label(0, 26, "LOGIC");        settings->label(1, 26, "FALSE");
	settings->label(0, 27, "GOT_TMS");      settings->label(1, 27, "TRUE");
	settings->label(0, 28, "SEC_TITLE");
	settings->label(0, 29, "SEC_TIME");
	settings->label(0, 30, "TNMT_TITLE");
	settings->label(0, 31, "TNMT_TIME");
	settings->label(0, 32, "LADDER");       settings->label(1, 32, "FALSE");
	settings->label(0, 33, "DIVIDED");      settings->label(1, 33, "FALSE");
	settings->label(0, 34, "LOW GRD");      settings->number(1, 34, 0);
	settings->label(0, 35, "HIGH GRD");     settings->number(1, 35, 99);
	settings->number(0, 36, -1);            settings->number(1, 36, -1);
	settings->label(0, 37, "DBL_ACCEL");    settings->label(1, 37, "FALSE");
	settings->label(0, 38, "TEAMS_ONLY");   settings->label(1, 38, "FALSE");
	settings->number(0, 39, -1);            settings->label(1, 39, "TRUE");
	settings->label(0, 40, "RATING_TO_USE");       settings->number(1, 40, 0);
	settings->label(0, 41, "EXCLUDE_BATCH");       settings->label(1, 41, "FALSE");
	settings->label(0, 42, "EXCLUDE_RATINGS");       settings->label(1, 42, "FALSE");
	settings->label(0, 43, "EXCLUDE_KRAUSE");       settings->label(1, 43, "FALSE");

	tables->label(0, 3, "NAT_WH");
	tables->label(0, 4, "NAT_BL");
	tables->label(0, 5, "NAT_BD");
	tables->label(0, 6, "A_NAT_WH");
	tables->label(0, 7, "A_NAT_BL");
	tables->label(0, 8, "A_NAT_BD");

	int cur_col = 0;
	players->label(0, cur_col++, "NAME");
	players->label(0, cur_col++, "ID");
	players->label(0, cur_col++, "ID2");
	players->label(0, cur_col++, "RATING");
	players->label(0, cur_col++, "RATING2");
	players->label(0, cur_col++, "EXP1");
	players->label(0, cur_col++, "EXP2");
	players->label(0, cur_col++, "CLUB");
	players->label(0, cur_col++, "TEAM");
	players->label(0, cur_col++, "TITLE");
	players->label(0, cur_col++, "COMPUTER");
	players->label(0, cur_col++, "AGE");
	players->label(0, cur_col++, "SEX");
	players->label(0, cur_col++, "CLASS");
	players->label(0, cur_col++, "FEES1");
	players->label(0, cur_col++, "FEES2");
	players->label(0, cur_col++, "FEES3");
	players->label(0, cur_col++, "ADDRESS");
	players->label(0, cur_col++, "CITY");
	players->label(0, cur_col++, "STATE");
	players->label(0, cur_col++, "ZIP");
	players->label(0, cur_col++, "NO_BYE");
	players->label(0, cur_col++, "DOB");
	players->label(0, cur_col++, "PHONE");
	players->label(0, cur_col++, "MEMO");
	players->label(0, cur_col++, "BD_ORD");
	players->label(0, cur_col++, "ADV_BYES");
	players->label(0, cur_col++, "PRE_PAIRED");
	players->label(0, cur_col++, "SCORE");
	players->label(0, cur_col++, "RES_CH");
	players->label(0, cur_col++, "WITHDREW");
	players->label(0, cur_col++, "PAIRED");
	players->label(0, cur_col++, "FLAGGED	");
	players->label(0, cur_col++, "NO_PRIZE");
	players->label(0, cur_col++, "FIDE_RATE");
	players->label(0, cur_col++, "OLD_ID");
	players->label(0, cur_col++, "RES_BRD");
	players->label(0, cur_col++, "ESTIMATED");
	players->label(0, cur_col++, "SECTION_NUM");
	players->label(0, cur_col++, "BOARDS");
	players->label(0, cur_col++, "ADJ_SCORES");
	players->label(0, cur_col++, "NON_FIDE_RDS");

	std::map<CString, unsigned> sibling_count;

	for (i = 0; i < sec.players.size(); ++i) {
		if (sec.players[i].subsection != subsec) continue;
		auto miter = sibling_count.find(sec.players[i].last_name);
		if (miter == sibling_count.end()) {
			sibling_count.insert(std::pair<CString, unsigned>(sec.players[i].last_name, 1));
		}
		else {
			miter->second++;
		}
	}

	std::map<CString, unsigned> sibling_club_names;
	unsigned next_club_id = 0;

	for (auto miter = sibling_count.begin(); miter != sibling_count.end(); ++miter) {
		if (miter->second > 1) {
			sibling_club_names.insert(std::pair<CString, unsigned>(miter->first, next_club_id++));
			normal_log << "*) Potential siblings found in section " << sec_name << " with last name " << CStringToWString(miter->first) << std::endl;
			normal_log << "        Please verify they are siblings.  If not, remove the players' 'club' designation with Players->Tinker" << std::endl;
			//normal_log << "        If they are siblings, add club pairing restriction with Setup->Rules for pairing->Pair Restrictions->Federation/Club" << std::endl;
		}
	}

	unsigned cur_row = 1;
	for (i = 0; i < sec.players.size(); ++i) {
		if (sec.players[i].subsection != subsec) continue;

		std::wstring name_field = toUpper(CStringToWString(sec.players[i].last_name)) + L", " +
			toUpper(CStringToWString(sec.players[i].first_name));
		checkin->label(3 + i, 0, toString(name_field));
		players->label(cur_row, 0, toString(name_field));

		std::wstring id_field = CStringToWString(sec.players[i].full_id);
		players->label(cur_row, 1, toString(id_field));

		if (!sec.players[i].uscf_id.IsEmpty()) {
			std::wstring id2_field = CStringToWString(sec.players[i].uscf_id);
			players->label(cur_row, 2, toString(id2_field));
		}

		if (sec.players[i].unrated) {
			players->number(cur_row, 3, 0);
		}
		else {
			players->number(cur_row, 3, sec.players[i].rating);
		}

		if (!sec.players[i].uscf_rating.IsEmpty()) {
			std::wstring rtng2_field = CStringToWString(sec.players[i].uscf_rating);
			players->label(cur_row, 4, toString(rtng2_field));
		}

		std::wstringstream notes_field;

		auto miter = sibling_club_names.find(sec.players[i].last_name);
		if (miter != sibling_club_names.end()) {
			players->number(cur_row, 7, miter->second);
			notes_field << "Verify sibling also playing in section.";
		}

		std::wstring school_code_ws = CStringToWString(sec.players[i].school_code);
		if (school_code_ws != L"HSO") {
			players->label(cur_row, 8, toString(school_code_ws));
		}

		std::wstring grade_ws = CStringToWString(getGradeStringShort(sec.players[i].grade));
		players->label(cur_row, 11, toString(grade_ws));

		players->number(cur_row, 28, 0);
		players->label(cur_row, 31, "FALSE");
		players->number(cur_row, 38, 0);

		if (notes_field.str().length() != 0) {
			notes_field << " ";
		}
		notes_field << sec.players[i].notes;
			
		if (notes_field.str().length() != 0) {
			checkin->label(3 + i, 2, toString(notes_field.str()));
		}

		++cur_row;
	}

	std::wstring s = output_dir + L"\\" + sec_name + L".S0C";
	std::string filename = toString(s);
	int err = wb.Dump(filename);

	if (err != NO_ERRORS) {
		MessageBox(_T("Failed to create workbook."), _T("Error"));
	}

	std::wstring checkin_s = output_dir + L"\\" + sec_name + L"-checkin.xls";
	std::string checkin_filename = toString(checkin_s);
	err = checkin_wb.Dump(checkin_filename);

	if (err != NO_ERRORS) {
		MessageBox(_T("Failed to section check-in workbook."), _T("Error"));
	}

	return cur_row - 1;
}

void CCCSwisssys2View::OnCreateSwisssysTourney()
{
	auto pDoc = GetDocument();

	std::wstring output_dir = pDoc->getOutputLocation();
	std::wstring logfilename = output_dir + _T("\\TD_notes.txt");
	std::wofstream normal_log(logfilename);

	std::wstring base = pDoc->getFileBase();
	if (base == L"") {
		BOOL save_res = pDoc->DoFileSave();
		if (save_res) {
			base = pDoc->getFileBase();
			//assert(base != L"");
		}
		else {
			MessageBox(_T("You must save the file first before you can generate a Swisssys tournament as the tournament file will go in the same directory as your saved filed."), _T("Error"));
			return;
		}
	}

	normal_log << "Tournament Director Notes for tournament named " << base << std::endl;
	normal_log << "---------------------------------------------------------------------------------\n" << std::endl;
	normal_log << "1) Turn on team pairing restriction for all sections.  Remove with a couple rounds to play." << std::endl;
	normal_log << "        To add/remove, click on each section, Setup->Rules for pairing->Pair Restrictions->Team" << std::endl;
	normal_log << "2) Set the number of rounds, board numbers, and time control for each section.  Verify section type." << std::endl;
	normal_log << "        Verify the section type, either swiss or round-robin." << std::endl;
	normal_log << "        If there are N players in a section and time for N-1 rounds, use a round-robin." << std::endl;
	normal_log << "        To set, click on Setup->Multi-section tournament setup->Section Name->View/Edit Section" << std::endl;
	normal_log << "3) General announcements" << std::endl;
	normal_log << "        a) Touch-move, touch-take." << std::endl;
//	normal_log << "            Castling is a king move, move the king two squares first, then the rook." << std::endl;
	normal_log << "        b) Raise your hand if you have a question or problem." << std::endl;
	normal_log << "            For example, if you're not sure your opponent did en-passant correctly." << std::endl;
	normal_log << "        c) Players own the result of their games." << std::endl;
	normal_log << "        d) Particularly in lower sections, there's plenty of time so take your time and don't rush." << std::endl;
	normal_log << "4) If there are sibling restrictions noted below, remove with one or two rounds to play." << std::endl;
	normal_log << "        To remove, click on each section, Setup->Rules for pairing->Pair Restrictions->Federation/Club" << std::endl;
	normal_log << "5) Check-in lists were generated for each section and stored in the same directory as the Swisssys tmt file." << std::endl;
	normal_log << std::endl;

	//unsigned total_sections = 0;
	std::map<unsigned, unsigned> pairings_computers;
	std::map<unsigned, unsigned> cur_sec_nums;
	std::map<unsigned, std::wstring> output_dirs;

	// Fill in a set with all the unique computer numbers so we know how many we need.
	// Calculate the total number of sections.
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		pairings_computers.insert(std::pair<unsigned,unsigned>(pDoc->sections[j].which_computer,0));
		auto pciter = pairings_computers.find(pDoc->sections[j].which_computer);
		pciter->second += pDoc->sections[j].num_subsections;
		cur_sec_nums.insert(std::pair<unsigned, unsigned>(pDoc->sections[j].which_computer, 0));
	}

	std::map<unsigned, std::wofstream *> tmts;
	// For each pairing computer we need...
	for (auto pciter = pairings_computers.begin(); pciter != pairings_computers.end(); ++pciter) {
		std::wstringstream fss;
		// If we have more than one pairing computer then we create directories for the tournaments.
		if (pairings_computers.size() > 1) {
			std::wstringstream dir_base;
			// The directories are named computerN where N is the pairing computer number.
			dir_base << output_dir << "\\computer" << pciter->first;
			int mkdir_res = _wmkdir(dir_base.str().c_str());
			if (mkdir_res != 0) {
				std::wstringstream err;
				err << errno;
				MessageBox(err.str().c_str(), _T("Error"));
				if (errno == EEXIST) {

				}
			}
			fss << dir_base.str() << "\\computer" << pciter->first << ".tmt";
			output_dirs.insert(std::pair<unsigned, std::wstring>(pciter->first, dir_base.str()));
		}
		else {
			// If only one pairing computer then create in the same directory.
			fss << base << ".tmt";
			output_dirs.insert(std::pair<unsigned, std::wstring>(pciter->first, output_dir));
		}
		std::wstring filename = fss.str();
		// Create mapping from pairing computer to open file to write to.
		tmts.insert(std::pair<unsigned, std::wofstream *>(pciter->first, new std::wofstream(filename)));

		std::wofstream &tmt_file = *(tmts.find(pciter->first)->second);
		tmt_file << "[Setup]" << std::endl;
		tmt_file << "Version = 9.06" << std::endl;
		tmt_file << "[Tournament info]" << std::endl;
		tmt_file << "Title =" << std::endl;
		tmt_file << "Time controls =" << std::endl;
		tmt_file << "[Section count]" << std::endl;
		tmt_file << "Total = " << pciter->second << std::endl;
		tmt_file << "[Sections]" << std::endl;
	}

	// Write each section to the tournament file
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		std::wofstream &tmt_file = *(tmts.find(pDoc->sections[j].which_computer)->second);
		auto csniter = cur_sec_nums.find(pDoc->sections[j].which_computer);
		unsigned &cur_sec_num = csniter->second;
		if (pDoc->sections[j].num_subsections == 1) {
			tmt_file << "Section" << ++cur_sec_num << " = " << CStringToWString(pDoc->sections[j].name) << std::endl;
		}
		else {
			for (unsigned k = 0; k < pDoc->sections[j].num_subsections; ++k) {
				std::wstring sec_name = baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k);
				tmt_file << "Section" << ++cur_sec_num << " = " << sec_name << std::endl;
			}
		}
	}
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		std::wofstream &tmt_file = *(tmts.find(pDoc->sections[j].which_computer)->second);
		if (pDoc->sections[j].num_subsections == 1) {
			addToSwisssysSection(tmt_file, CStringToWString(pDoc->sections[j].name), pDoc->sections[j].sec_type);
		}
		else {
			for (unsigned k = 0; k < pDoc->sections[j].num_subsections; ++k) {
				addToSwisssysSection(tmt_file, baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k), pDoc->sections[j].sec_type);
			}
		}
	}
	for (auto tmtiter = tmts.begin(); tmtiter != tmts.end(); ++tmtiter) {
		tmtiter->second->close();
	}

	unsigned total_placed = 0;

	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		auto oditer = output_dirs.find(pDoc->sections[j].which_computer);
		if (pDoc->sections[j].num_subsections == 1) {
			std::wstring sec_name = CStringToWString(pDoc->sections[j].name);
			total_placed += createSectionWorksheet(normal_log, oditer->second, sec_name, pDoc->sections[j], 1);
		}
		else {
			for (unsigned k = 0; k < pDoc->sections[j].num_subsections; ++k) {
				std::wstring sec_name = baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k);
				total_placed += createSectionWorksheet(normal_log, oditer->second, sec_name, pDoc->sections[j], k+1);
			}
		}
	}

	if (total_placed == 0) {
		MessageBox(_T("No players in any section.  Please click the Create Sections button first."), _T("Error"));
	}
	else {
		MessageBox(_T("Swisssys tournament created successfully."), _T("Information"));
	}

	normal_log.close();
	ShellExecute(NULL, _T("open"), _T("c:\\windows\\notepad.exe"), (LPCWSTR)logfilename.c_str(), _T(""), SW_SHOWNORMAL);
}


void CCCSwisssys2View::OnRestrictFileBrowse()
{
	CFileDialog FileDialog(TRUE, _T("txt"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->restricted_file = PathName;

		//MessageBox((LPCTSTR)PathName, _T("Ratings Filename"));
		restricted_edit.SetWindowText(PathName);
		GetDocument()->SetModifiedFlag();
	}
}


void CCCSwisssys2View::OnDoubleClickSectionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int selected_row = pNMItemActivate->iItem;
	if (selected_row >= 0) {
		section_list.SetSelectionMark(-1);
		auto pDoc = GetDocument();
		Section newSection = pDoc->sections[selected_row];
		while (true) {
			bool check;
			SectionEditor se(&newSection, check);
			INT_PTR nRet = se.DoModal();
			if (nRet != IDOK) {
				return;
			}

			//MessageBox(_T("Debugging"), _T("User pressed OK button on dialog."));

			if (check) {
				int conflict = pDoc->sections.conflicts(newSection, selected_row);

				if (conflict >= 0) {
					//std::wstringstream ss;
					CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
					//ss << "New section conflicts with existing section with name " << pDoc->sections[conflict].name << ".";
					//MessageBox(CString(ss.str().c_str()), _T("Section Conflict"));
					MessageBox(ss, _T("Section Conflict"));
					continue;
				}
			}

			pDoc->sections[selected_row] = newSection;
			refillSections(pDoc->sections, check);
			pDoc->SetModifiedFlag();
			break;
		}
	}

	*pResult = 0;
}


void CCCSwisssys2View::OnBnClickedSplitSection()
{
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.SetSelectionMark(-1);
		auto pDoc = GetDocument();
		Section selected_section = pDoc->sections[selected_row];

		if (selected_section.players.size() > 2) {
			SplitSection ss(&selected_section, pDoc, selected_row);
			INT_PTR nRet = ss.DoModal();
			if (nRet == IDOK) {
				refillSections(pDoc->sections, false);
				pDoc->SetModifiedFlag();
			}
		}
		else {
			MessageBox(_T("Can't split a section with less than 3 players."), _T("Information"), MB_OK);
		}
	}
}


void CCCSwisssys2View::OnOptionsSaveschoolcorrections()
{
	auto pDoc = GetDocument();
	pDoc->save_school_corrections = !pDoc->save_school_corrections;
	pDoc->saved_school_corrections.clear();
	pDoc->SetModifiedFlag();
}


void CCCSwisssys2View::OnUpdateOptionsSaveschoolcorrections(CCmdUI *pCmdUI)
{
	auto pDoc = GetDocument();
	CMenu *pMenu = this->GetParent()->GetMenu();
	if (pMenu != NULL) {
		pMenu->CheckMenuItem(ID_OPTIONS_SAVESCHOOLCORRECTIONS, (pDoc->save_school_corrections ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
	}
}