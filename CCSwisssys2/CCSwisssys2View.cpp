
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

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Time Control");
		section_list.InsertColumn(9, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Board Numbers");
		section_list.InsertColumn(10, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Playing Room");
		section_list.InsertColumn(11, &lvColumn);

		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 100;
		lvColumn.pszText = _T("Number of Rounds");
		section_list.InsertColumn(12, &lvColumn);

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
	static TCHAR BASED_CODE szFilter[] = _T("NWSRS Ratings Dat Files (*.dat)|*.dat|");
	CFileDialog FileDialog(TRUE, _T("dat"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->ratings_file = PathName;

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

	section_list.SetItemText(nItem, 9, newSection.time_control.GetBuffer());
	if (newSection.players.size() > 0 && newSection.starting_board_number != L"" && newSection.num_subsections == 1) {
		int last_board_number = newSection.getLastBoardNumber();
		std::wstringstream ss;
		ss << CStringToWString(newSection.starting_board_number) << L"-" << last_board_number;
		section_list.SetItemText(nItem, 10, CString(ss.str().c_str()));
	}
	else {
		section_list.SetItemText(nItem, 10, newSection.starting_board_number.GetBuffer());
	}
	section_list.SetItemText(nItem, 11, newSection.playing_room.GetBuffer());
	{
		std::wstringstream ss;
		ss << newSection.num_rounds;
		section_list.SetItemText(nItem, 12, CString(ss.str().c_str()));
	}
}

void CCCSwisssys2View::refillSections(Sections &s, bool clear, int selection_mark) {
	unsigned i;
	section_list.DeleteAllItems();
	for (i = 0; i < s.size(); ++i) {
		addToSectionList(section_list, s[i], i);
	}
	section_list.SetSelectionMark(selection_mark);
	section_players.DeleteAllItems();
	if (clear) {
		auto pDoc = GetDocument();
		pDoc->sections.clearPlayers();
	}
}

// When they click the add section button.
void CCCSwisssys2View::OnAddSection() {
	Section newSection;
	while (true) {
		auto pDoc = GetDocument();

		bool check;
		SectionEditor se(&newSection, pDoc, check);
		INT_PTR nRet = se.DoModal();
		if (nRet != IDOK) {
			return;
		}

		int conflict = pDoc->sections.conflicts(newSection);

		if (conflict >= 0) {
			CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
			MessageBox(ss, _T("Section Conflict"));
			continue;
		}

		pDoc->sections.push_back(newSection);
		refillSections(pDoc->sections, false, pDoc->sections.size()-1);
		pDoc->SetModifiedFlag();
		return;
	}
}

// Click on the delete section button.
void CCCSwisssys2View::OnDeleteSection() {
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.DeleteItem(selected_row);
		auto pDoc = GetDocument();
		pDoc->sections.erase(pDoc->sections.begin() + selected_row);
		refillSections(pDoc->sections, false, selected_row - 1);
		pDoc->SetModifiedFlag();
	}
}


// Click on the edit section button.
void CCCSwisssys2View::OnEditSection() {
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.SetSelectionMark(-1);
		auto pDoc = GetDocument();
		Section newSection = pDoc->sections[selected_row];
		while (true) {
			bool check;
			SectionEditor se(&newSection, pDoc, check);
			INT_PTR nRet = se.DoModal();
			if (nRet != IDOK) {
				return;
			}

			if (check) {
				int conflict = pDoc->sections.conflicts(newSection, selected_row);

				if (conflict >= 0) {
					CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
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

bool isValidUSCFId(std::wstring &uscf_id) {
	return isNumeric(uscf_id) && uscf_id.length() == 8;
}

void updateUscfFromRatingFile(std::wstring &uscf_id, std::wstring &uscf_rating, const Player &p, std::wofstream &normal_log, bool &warning_condition, log_messages &lm) {
	if (uscf_id == L"N/A" || uscf_id == L"NONE") {
		uscf_id = L"";
	}

	if (p.uscf_id != L"") {
		std::wstring temp_rating = p.uscf_rating;

		if (uscf_id.size() != 0 && uscf_id != p.uscf_id) {
			warning_condition = true;
			lm.uscf_id_cc_no_match << "    WARNING: USCF ID in constant contact " << uscf_id << " not the same as the one in the ratings file " << p.uscf_id << " for player " << p.first_name << " " << p.last_name << " NWSRS ID: " << p.getFullId() << std::endl;
			//normal_log << "WARNING: USCF ID in constant contact " << uscf_id << " not the same as the one in the ratings file " << p.uscf_id << " for player " << p.first_name << " " << p.last_name << " NWSRS ID: " << p.getFullId() << std::endl;
		}
		if (uscf_rating.size() != 0 && uscf_rating != temp_rating) {
			//warning_condition = true;
			//normal_log << "WARNING: USCF rating in constant contact " << uscf_rating << " not the same as the one in the ratings file " << temp_rating << std::endl;
		}

		uscf_id = p.uscf_id;
		uscf_rating = temp_rating;
	}
	else {
		if (isValidUSCFId(uscf_id)) {
			warning_condition = true;
			lm.new_uscf_ids << "    WARNING: USCF ID specified " << uscf_id << " but no USCF ID present in the ratings file for player " << p.last_name << " " << p.first_name << " grade code = " << p.grade << " NWSRS ID: " << p.getFullId() << std::endl;
			//normal_log << "WARNING: USCF ID specified " << uscf_id << " but no USCF ID present in the ratings file for player " << p.last_name << " " << p.first_name << " grade code = " << p.grade << " NWSRS ID: " << p.getFullId() << std::endl;
		}
		else {
			uscf_id = L"";
		}
	}
}

std::vector<int> findExactMatch(const std::wstring &last, const std::wstring &first, const std::vector<Player> &rated_players) {
	unsigned i;
	std::vector<int> ret;

	for (i = 0; i < rated_players.size(); ++i) {
		if (last == rated_players[i].last_name &&
			first == rated_players[i].first_name) {
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

std::wstring removeWhitespace(std::wstring &s) {
	s.erase(std::remove_if(s.begin(), s.end(), iswspace), s.end());
	return s;
}

std::wstring stripNA(const std::wstring &s) {
	if (s == L"NONE" || s == L"NA" || s == L"N/A" || s == L"NEW") {
		return L"";
	}
	else {
		return s;
	}
}

bool isIDInDB(const std::wstring &id, const std::map<std::wstring, unsigned> &db) {
	auto rentry = db.find(id);
	return rentry != db.end();
}

void parents_registering_no_children(
	const std::vector<SectionPlayerInfo> &post_proc,
	const std::map<std::wstring, unsigned> &adult_map,
	bool &error_condition,
	std::wofstream &normal_log) {

	// Initialize number of children to each parent to 0.
	std::map<std::wstring, unsigned> children_per_adult;
	for (auto aiter = adult_map.begin(); aiter != adult_map.end(); ++aiter) {
		children_per_adult.insert(std::pair<std::wstring, unsigned>(aiter->first, 0));
	}
	// For each player.
	for (const SectionPlayerInfo &spi : post_proc) {
		const std::wstring adult_combined = spi.adult_last + spi.adult_first;
		auto citer = children_per_adult.find(adult_combined);
		ASSERT(citer != children_per_adult.end());
		citer->second++;
	}
	// For each adult.
	for (auto citer = children_per_adult.begin(); citer != children_per_adult.end(); ++citer) {
		// If they didn't register any players then throw an error to the log.
		if (citer->second == 0) {
			error_condition = true;
			normal_log << "ERROR: Parent " << citer->first << " did not register any players." << std::endl;
		}
	}
}

// MAIN
std::vector<SectionPlayerInfo> process_cc_file(
	HWND hWnd, 
	CCCSwisssys2Doc *pDoc, 
	std::vector<ConstantContactEntry> &entries, 
	std::map<std::wstring, unsigned> &adult_map, 
	bool &error_condition, 
	bool &warning_condition, 
	bool &info_condition, 
	std::wofstream &normal_log, 
	log_messages &lm) {

	std::vector<SectionPlayerInfo> post_proc;

	std::wifstream cc_file(pDoc->constant_contact_file);
	bool use_cc = true;
	if (!cc_file) {
		use_cc = false;
	}
	cc_file.close();

	unsigned i;

	if (use_cc) {
		std::wstring ccsstr = CStringToWString(pDoc->constant_contact_file);
		entries = load_constant_contact_file(ccsstr, pDoc->nwsrs_four_map, pDoc->uscf_map, pDoc->rated_players, pDoc->school_codes, normal_log);
		if (entries.size() == 0) {
			MessageBox(hWnd, _T("No players loaded from constant contact file."), _T("ERROR"), MB_OK);
			return post_proc;
		}
		
		unsigned num_parents = 0, num_players = 0;

		for (i = 0; i < entries.size(); ++i) {
			bool was_parent = false, was_player = false;

			if (entries[i].isParent()) {
				++num_parents;
				was_parent = true;
				//normal_log << "Parent:" << entries[i].getFirstName() << " " << entries[i].getLastName() << " " << std::endl;
			}
			if (entries[i].isPlayer()) {
				++num_players;
				was_player = true;
				//normal_log << "Player:" << entries[i].getFirstName() << " " << entries[i].getLastName() << " " << std::endl;
			}
			if (was_parent && was_player) {
				MessageBox(hWnd, _T("Some entry was both parent and player."), _T("ERROR"), MB_OK);
				normal_log << "ERROR: entry both parent and player. " << entries[i].getFirstName() << " " << entries[i].getLastName() << std::endl;
			}
			if (!was_parent && !was_player) {
				MessageBox(hWnd, _T("Some entry was neither parent nor player."), _T("ERROR"), MB_OK);
				normal_log << "ERROR: entry neither parent nor player. " << entries[i].getFirstName() << " " << entries[i].getLastName() << std::endl;
			}
		}

		normal_log << "There are " << entries.size() << " entries in the file." << std::endl;
		normal_log << "There are " << num_parents << " parents." << std::endl;
		normal_log << "There are " << num_players << " players.\n" << std::endl;
		if (num_parents + num_players != entries.size()) {
			MessageBox(hWnd, _T("Some problem where number of parents plus number of players not equal to number of entries."), _T("ERROR"), MB_OK);
			//ShellExecute(NULL, _T("open"), _T("c:\\windows\\notepad.exe"), (LPCWSTR)logfilename.c_str(), _T(""), SW_SHOWNORMAL);
			return post_proc;
		}

		std::set<std::wstring> duplicate_detect;

		for (i = 0; i < entries.size(); ++i) {
			if (!entries[i].isPlayer()) {
				std::wstring last_name = entries[i].getLastName();
				std::wstring first_name = entries[i].getFirstName();
				std::wstring combined = last_name + first_name;
				adult_map.insert(std::pair<std::wstring, unsigned>(combined, i));
			}

			if (entries[i].isPlayer()) {
#ifdef DEBUG_MAIN
				normal_log << std::endl;
#endif

				const std::wstring last_name = entries[i].getLastName();
				const std::wstring first_name = entries[i].getFirstName();
				std::wstring school = entries[i].getSchool();
				const std::wstring orig_school = school;
				std::wstring full_id = stripNA(toUpper(removeWhitespace(entries[i].getNwsrsId())));
				const std::wstring full_id_save = full_id;
				const std::wstring upper_last = toUpper(last_name);
				const std::wstring upper_first = toUpper(first_name);
				std::wstring uscf_id = entries[i].getUscfId();
				std::wstring uscf_expr = L"";
				std::wstring uscf_rating = L"";
				const std::wstring full_name = first_name + L" " + last_name;
				const std::wstring unique_key = full_name + L" " + school + L" " + full_id;
				const std::wstring registration_date = entries[i].getRegistrationDate();

				const std::wstring adult_first = entries[i].getAdultFirst();
				const std::wstring adult_last = entries[i].getAdultLast();
				const std::wstring adult_combined = adult_last + adult_first;
				unsigned cc_rating = 4000;
				unsigned nwsrs_rating;
				bool id_perfect = false;

				std::wstring school_check = L"ACMA";
				if (school_check == L"ACMA") {
					school_check = L"ACMA";
				}
				std::wstring LAST_FOUR_TO_USE = L"";
				std::wstring SCHOOL_CODE_TO_USE = L"";
				wchar_t GRADE_CODE_TO_USE = L' ';
				std::wstring FIRST_TO_USE = L"";
				std::wstring LAST_TO_USE = L"";
				bool new_player = false;
				std::wstringstream notes;

				auto corrected_iter = pDoc->get_corrected_iter(unique_key);
				if (corrected_iter != pDoc->saved_school_corrections.end()) {
					LAST_FOUR_TO_USE = corrected_iter->second.id;
					SCHOOL_CODE_TO_USE = corrected_iter->second.school_code;
				}

				// ---------------- Handle last four digits of ID ------------------------------------

				/*
				 * Steps to get the correct id:
				 * 1) See if they entered a USCF ID and convert to NWSRS ID.
				 * 2) See if the full NWSRS ID is in the database and verify the name matches.
				 * 3) Search based on the last 4 digits of the NWSRS ID.  If a match then verify the name matches.
				 * 4) Search based on name.
				 */

				if (full_id.size() == 8) {
					// People can't follow directions and sometimes put USCF ID in NWSRS spot.
					auto uscfiter = pDoc->uscf_map.find(full_id);
					if (uscfiter != pDoc->uscf_map.end()) {
						uscf_id = full_id;
						full_id = pDoc->rated_players[uscfiter->second].getFullId();
						normal_log << "WARNING: USCF ID " << uscf_id << " for player " << first_name << " " << last_name << " was given as the NWSRS ID.  CCSwisssys has fetched the NWSRS ID " << full_id << " from the ratings file." << std::endl;
					}
					else {
						std::wstring inputschoolcode = full_id.substr(0, 3);
						if (!isAlpha(inputschoolcode)) {
							normal_log << "WARNING: Specified ID " << full_id << " for player " << first_name << " " << last_name << " is malformed.  Ignoring the ID and trying to find ID from name." << std::endl;
							full_id = L"";
						}
					}
				}

				// This is outside a LAST_FOUR_TO_USE check for reporting purposes.
				id_perfect = isIDInDB(full_id_save, pDoc->nwsrs_map);

				if (LAST_FOUR_TO_USE == L"") {
					auto rentry = pDoc->nwsrs_map.find(full_id);
					if (rentry != pDoc->nwsrs_map.end()) {
						unsigned rp_index = rentry->second;
						std::wstring cap_db_last = toUpper(pDoc->rated_players[rp_index].last_name);
						std::wstring cap_db_first = toUpper(pDoc->rated_players[rp_index].first_name);
						if (cap_db_last != upper_last || cap_db_first != upper_first) {
							std::wstringstream wss;
							wss << "Name for player " << upper_first << " " << upper_last << " with id " << full_id_save << 
								" does not match the name in the database " << cap_db_first << " " << cap_db_last << ".  Are these the same player?";
							int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);
							if (ret == IDYES) {
								LAST_FOUR_TO_USE = getLastFour(full_id);
							}
							else {
								// The ID is in the database but doesn't match the person's name so they must have entered
								// someone else's ID.  So, let's just forget the ID and search by name.
								full_id = L"";
							}
						}
						else {
							// ID and Name match exactly.
							LAST_FOUR_TO_USE = getLastFour(full_id);
						}
					}
				}

				// Handle ID specification input differences and errors.
				if (full_id.size() != 0 && full_id.size() != 4 && full_id.size() != 8) {
					warning_condition = true;
					if (full_id != L"NONE" && full_id.size() != 0) {
						normal_log << "WARNING: Specified ID " << full_id << " for player " << first_name << " " << last_name << " is malformed.  Ignoring the ID and trying to find ID from name." << std::endl;
					}
					full_id = L"";
				}

				std::wstring last_four_id;
				if (full_id.length() >= 4) {
					last_four_id = getLastFour(full_id);
				}

				if (LAST_FOUR_TO_USE == L"") {
					auto lfi_rentry = pDoc->nwsrs_four_map.find(last_four_id);
					if (lfi_rentry != pDoc->nwsrs_four_map.end()) {
						unsigned rp_index = lfi_rentry->second;
						std::wstring cap_db_last = toUpper(pDoc->rated_players[rp_index].last_name);
						std::wstring cap_db_first = toUpper(pDoc->rated_players[rp_index].first_name);
						if (cap_db_last != upper_last || cap_db_first != upper_first) {
							std::wstringstream wss;
							wss << "Name for player " << upper_first << " " << upper_last << " with id " << full_id_save <<
								" does not match the name in the database " << cap_db_first << " " << cap_db_last << ".  Are these the same player?";
							int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);
							if (ret == IDYES) {
								LAST_FOUR_TO_USE = getLastFour(full_id);
							}
							else {
								// The ID is in the database but doesn't match the person's name so they must have entered
								// someone else's ID.  So, let's just forget the ID and search by name.
								full_id = L"";
							}
						}
						else {
							// ID and Name match exactly.
							LAST_FOUR_TO_USE = getLastFour(full_id);
						}
					}
				}

				// Search by name.
				if (LAST_FOUR_TO_USE == L"") {
					std::vector<int> exact_match = findExactMatch(upper_last, upper_first, pDoc->rated_players);
					bool match_found = false;
					for (auto match = exact_match.begin(); match != exact_match.end(); ++match) {
						std::wstringstream wss;
						wss << "No ID or bad ID " << full_id_save << " specified for player " << upper_first << " " << upper_last << " school: " << school
							<< ".  However, a player with the same first and last name was found in the ratings file with ID: " << pDoc->rated_players[*match].getFullId() <<
							" and grade code: " << pDoc->rated_players[*match].grade <<
							" and school code: " << pDoc->rated_players[*match].school_code << "(" <<
							pDoc->school_codes.findName(pDoc->rated_players[*match].school_code) << ")" <<
							" Please verify if this is the same person and click Yes if they are and No if they aren't.";
						int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);

						if (ret == IDYES) {
							warning_condition = true;
							//normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;
							//normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;

							full_id = pDoc->rated_players[*match].getFullId();
							LAST_FOUR_TO_USE = getLastFour(full_id);
							match_found = true;
							normal_log << "WARNING: Player ID corrected from " << full_id_save << " to " << full_id << " by an exact match of their first and last names in the ratings file for player " << first_name << " " << last_name << std::endl;
							break;
						}
					}
					if (!match_found) {
						if (full_id_save != L"") {
							error_condition = true;
							normal_log << "ERROR: Did not find player ID " << last_four_id << " in the ratings file. " << full_id << " " << last_name << " " << first_name << " " << school << std::endl;
							continue;
						}
						else {
							new_player = true;
							info_condition = true;
							normal_log << "INFO: new tournament player without a previous ID " << first_name << " " << last_name << ", school: " << school << std::endl;
						}
					}
				}

				std::wstring ID_IN_FILE;
				wchar_t GRADE_CODE_IN_FILE;
				std::wstring SCHOOL_CODE_IN_FILE;

				if (!new_player) {
					auto lfi_rentry = pDoc->nwsrs_four_map.find(LAST_FOUR_TO_USE);
					unsigned LAST_FOUR_RENTRY = lfi_rentry->second;

					cc_rating = pDoc->rated_players[LAST_FOUR_RENTRY].get_higher_rating();
					updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[LAST_FOUR_RENTRY], normal_log, warning_condition, lm);
					nwsrs_rating = pDoc->rated_players[LAST_FOUR_RENTRY].nwsrs_rating;
					uscf_expr = pDoc->rated_players[LAST_FOUR_RENTRY].uscf_exp_date;
					ID_IN_FILE = pDoc->rated_players[LAST_FOUR_RENTRY].getFullId();
					GRADE_CODE_IN_FILE = pDoc->rated_players[LAST_FOUR_RENTRY].grade;
					SCHOOL_CODE_IN_FILE = pDoc->rated_players[LAST_FOUR_RENTRY].school_code;
					// Assume that DB is right.  If they want the name change then they'll have to change it later.
					LAST_TO_USE = toUpper(pDoc->rated_players[LAST_FOUR_RENTRY].last_name);
					FIRST_TO_USE = toUpper(pDoc->rated_players[LAST_FOUR_RENTRY].first_name);
				}
				else {
					LAST_TO_USE = upper_last;
					FIRST_TO_USE = upper_first;
				}
				auto cfiter = pDoc->get_corrected_iter(unique_key);
				cfiter->second.id = LAST_FOUR_TO_USE;

				if (LAST_TO_USE != upper_last || FIRST_TO_USE != upper_first) {
					lm.name_change << "    NAME: player named " << upper_first << " " << upper_last << " is named differently in the NWSRS database as " << FIRST_TO_USE << " " << LAST_TO_USE << std::endl;
					notes << "Update name in database from " << FIRST_TO_USE << " " << LAST_TO_USE << " to " << upper_first << " " << upper_last << "? ";
				}

				// ---------------- Handle grade codes ------------------------------------

				wchar_t grade_code;
				try {
					grade_code = entries[i].getGradeCode();
				}
				catch (UnrecognizedGradeCode const&) {
					if (new_player) {
						error_condition = true;
						normal_log << "ERROR: Unrecognized grade code " << entries[i].getGrade() << " for new player " << first_name << " " << last_name << std::endl;
						continue;
					}
					else {
						warning_condition = true;
						normal_log << "WARNING: Unrecognized grade code " << entries[i].getGrade() << " for ID " << full_id_save << " for player " << first_name << " " << last_name << ". Reverting to grade from ratings database." << std::endl;
						grade_code = GRADE_CODE_IN_FILE;
					}
				}

				if (!new_player && GRADE_CODE_IN_FILE != grade_code) {
					notes << "Unexpected grade change from " << CStringToWString(getGradeStringShort(GRADE_CODE_IN_FILE)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << ". ";
					lm.grade_change << "    Unexpected grade change from " << CStringToWString(getGradeStringShort(GRADE_CODE_IN_FILE)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << " for player " << first_name << " " << last_name << std::endl;
					grade_code = GRADE_CODE_IN_FILE;
				}

				// Handle initial rating for new players.
				if (new_player) {
					cc_rating = (grade_code - 'A') * 100;
					nwsrs_rating = cc_rating;
				}

				SCHOOL_TYPE st = getSchoolType(grade_code);

				// ---------------- Handle school codes ------------------------------------
				if (SCHOOL_CODE_TO_USE != L"") {
					school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
				}
				else {
					if (!new_player) {
						ASSERT(SCHOOL_CODE_IN_FILE != L"");
						// If they don't give a school then use the one from their ID.
						if (school == L"") {
							SCHOOL_CODE_TO_USE = SCHOOL_CODE_IN_FILE;
							school = pDoc->school_codes.findName(SCHOOL_CODE_IN_FILE);
#ifdef DEBUG_MAIN
							normal_log << "school not specified...getting it from school code" << school << std::endl;
#endif
						}
						else if (school.length() == 3 && pDoc->school_codes.findName(toUpper(school)) != L"") {
							SCHOOL_CODE_TO_USE = toUpper(school);
							school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
#ifdef DEBUG_MAIN
							normal_log << "school length is 3 so assuming school code " << school_code << std::endl;
#endif
							// If the calculated school code is not what is in their rating and we didn't previously ask the user
							// about this person then ask them now so that we don't silently change a school code that might exactly
							// match what they entered.
							if (SCHOOL_CODE_TO_USE != SCHOOL_CODE_IN_FILE) {
								SchoolSelector ss_dialog(pDoc, school, SCHOOL_CODE_IN_FILE, SCHOOL_CODE_TO_USE, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

								if (ss_dialog.DoModal() == IDOK) {
									school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
#ifdef DEBUG_MAIN
									normal_log << "school code after asking user " << school_code << " " << school << std::endl;
#endif
								}
								else {
									error_condition = true;
									normal_log << "ERROR: no confirmation of change of school code for " << school << " for player " << last_name << " " << first_name << " " << std::endl;
									continue;
								}
							}
						}
						else if (pDoc->school_codes.schoolNameMatchesCode(school, SCHOOL_CODE_IN_FILE)) {
							SCHOOL_CODE_TO_USE = SCHOOL_CODE_IN_FILE;
							school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
						}
						else {
							SchoolSelector ss_dialog(pDoc, school, SCHOOL_CODE_IN_FILE, SCHOOL_CODE_TO_USE, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

							if (ss_dialog.DoModal() == IDOK) {
								school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
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
					else {
						// For new players...
						if (school == L"") {
							error_condition = true;
							normal_log << "ERROR: no school information provided for new player " << first_name << " " << last_name << " " << std::endl;
							continue;
						}
						else if (school.length() == 3 && pDoc->school_codes.findName(toUpper(school)) != L"") {
							SCHOOL_CODE_TO_USE = toUpper(school);
							school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
#ifdef DEBUG_MAIN
							normal_log << "school length is 3 so assuming school code " << school_code << std::endl;
#endif
						}
						else {
							SCHOOL_CODE_TO_USE = pDoc->school_codes.findCodeFromSchoolExactNoSchool(school, st);
							if (SCHOOL_CODE_TO_USE == L"") {
#ifdef DEBUG_MAIN
								normal_log << "school not found exactly without school " << school << std::endl;
#endif
								SchoolSelector ss_dialog(pDoc, school, L"", SCHOOL_CODE_TO_USE, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

								if (ss_dialog.DoModal() == IDOK) {
									school = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
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


				full_id = SCHOOL_CODE_TO_USE + grade_code + LAST_FOUR_TO_USE;

				if (full_id.length() == 8 && SCHOOL_CODE_TO_USE != L"" && SCHOOL_CODE_IN_FILE != SCHOOL_CODE_TO_USE && SCHOOL_CODE_IN_FILE != L"") {
					info_condition = true;
					std::wstring print_school = orig_school;
					if (print_school == L"") {
						print_school = L"<None>";
					}
					lm.school_change << "    INFO: Player " << first_name << " " << last_name << " with ID specified as " << full_id << " and school as " << orig_school << " change of school from " << SCHOOL_CODE_IN_FILE << " to " << SCHOOL_CODE_TO_USE << std::endl;
					//normal_log << "INFO: Player " << first_name << " " << last_name << " with ID specified as " << full_id << " and school as " << orig_school << " change of school from " << ratings_school_code << " to " << school_code << std::endl;

				//	full_id.replace(0, 3, school_code); // Update school code in their ID.
				}

				if (SCHOOL_CODE_TO_USE != L"" && SCHOOL_CODE_IN_FILE != SCHOOL_CODE_TO_USE) {
					std::wstring rsc_name = pDoc->school_codes.findName(SCHOOL_CODE_IN_FILE);
					std::wstring sc_name = pDoc->school_codes.findName(SCHOOL_CODE_TO_USE);
					if (rsc_name != L"") {
						notes << "Verify school change from " << rsc_name << " to " << sc_name << ". ";
					}
				}

				if (!id_perfect) {
					if (full_id_save != L"NONE") {
						lm.imperfect << "    INFO: The incorrectly specified NWSRS ID " << full_id_save << " in the Constant Contact information for player " << first_name << " " << last_name << " has been corrected to ID " << full_id << std::endl;
					}
					//normal_log << "INFO: The incorrectly specified NWSRS ID " << full_id_save << " in the Constant Contact information for player " << first_name << " " << last_name << " has been corrected to ID " << full_id << std::endl;
				}

//				CString cs_last_name(last_name.c_str());
//				CString cs_first_name(first_name.c_str());
				CString cs_last_name(LAST_TO_USE.c_str());
				CString cs_first_name(FIRST_TO_USE.c_str());
				CString cs_school(school.c_str());
				CString cs_school_code(SCHOOL_CODE_TO_USE.c_str());
				CString cs_full_id = CString(full_id.c_str());
				CString cs_registration_date(registration_date.c_str());

				CString cs_adult_first(adult_first.c_str()), cs_adult_last(adult_last.c_str());
				auto spi = SectionPlayerInfo(-((int)i + 1), cs_last_name, cs_first_name, cs_full_id, nwsrs_rating, grade_code, cs_school, cs_school_code, CString(uscf_id.c_str()), CString(uscf_rating.c_str()), CString(uscf_expr.c_str()), notes.str(), new_player, cc_rating, cs_adult_first, cs_adult_last, cs_registration_date);

				auto dditer = duplicate_detect.find(last_four_id);
				if (last_four_id == L"" || dditer == duplicate_detect.end()) {
					post_proc.push_back(spi);
					duplicate_detect.insert(last_four_id);
				}
				else if (last_four_id != L"") {
					normal_log << "DUPLICATE PLAYER: " << first_name << " " << last_name << std::endl;
				}



#if 0
				auto rentry = pDoc->nwsrs_map.find(full_id);
				if (rentry != pDoc->nwsrs_map.end()) {
					id_perfect = true;
				}

				if (!id_perfect && full_id.size() == 8) {
					// People can't follow directions and sometimes put USCF ID in NWSRS spot.
					auto uscfiter = pDoc->uscf_map.find(full_id);
					if (uscfiter != pDoc->uscf_map.end()) {
						uscf_id = full_id;
						full_id = pDoc->rated_players[uscfiter->second].getFullId();
						normal_log << "WARNING: USCF ID " << uscf_id << " for player " << first_name << " " << last_name << " was given as the NWSRS ID.  CCSwisssys has fetched the NWSRS ID " << full_id << " from the ratings file." << std::endl;
					}
					else {
						std::wstring inputschoolcode = full_id.substr(0, 3);
						if (!isAlpha(inputschoolcode)) {
							normal_log << "WARNING: Specified ID " << full_id << " for player " << first_name << " " << last_name << " is malformed.  Ignoring the ID and trying to find ID from name." << std::endl;
							full_id = L"";
						}
					}
				}

				wchar_t grade_code;
				try {
					grade_code = entries[i].getGradeCode();
					if (full_id.size() == 8) {
						auto id_gc = full_id[3];
						if (id_gc != grade_code) {

						}
					}
				}
				catch (UnrecognizedGradeCode const&) {
					if (full_id.size() == 8) {
						grade_code = full_id[3];
					}
				}
				SCHOOL_TYPE st = getSchoolType(grade_code);
				std::wstringstream notes;
				bool unrated = false;
				bool debug_it = false;

				if (upper_first == L"ETHAN") {
					debug_it = true;
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

#ifdef DEBUG_MAIN
				normal_log << "Processing " << first_name << " " << last_name << " " << school << " " << full_id << " " << "last 4: " << last_four_id << " school code: " << ratings_school_code << " " << grade_code << " " << st << std::endl;
#endif

				CString cs_full_id = CString(full_id.c_str());

				rentry = pDoc->nwsrs_map.find(full_id);
				auto lfi_rentry = pDoc->nwsrs_four_map.find(last_four_id);

				if (rentry != pDoc->nwsrs_map.end()) {
					unsigned rp_index = rentry->second;
					cc_rating = pDoc->rated_players[rp_index].get_higher_rating();
					updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[rp_index], normal_log, warning_condition, lm);
					nwsrs_rating = pDoc->rated_players[rp_index].nwsrs_rating;
					std::wstring cap_db_last = toUpper(pDoc->rated_players[rp_index].last_name);
					std::wstring cap_db_first = toUpper(pDoc->rated_players[rp_index].first_name);
					if (cap_db_last != toUpper(last_name) || cap_db_first != toUpper(first_name)) {
						lm.name_change << "    NAME: player named " << toUpper(first_name) << " " << toUpper(last_name) << " is named differently in the NWSRS database as " << cap_db_first << " " << cap_db_last << std::endl;
						//normal_log << "NAME: player named " << toUpper(first_name) << " " << toUpper(last_name) << " is named differently in the NWSRS database as " << cap_db_last << " " << cap_db_first << std::endl;
						notes << "Update name in database from " << cap_db_first << " " << cap_db_last << "? ";
					}
					uscf_expr = pDoc->rated_players[rp_index].uscf_exp_date;
					if (pDoc->rated_players[rp_index].grade != grade_code) {
						notes << "Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[rp_index].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << ". ";
						lm.grade_change << "    Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[rp_index].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << " for player " << first_name << " " << last_name << std::endl;
						grade_code = pDoc->rated_players[rp_index].grade;
					}
				}
				else {
					std::wstring saved_correction_last_four_id = pDoc->get_saved_correction_id(unique_key);
					// No exact full id matching in ratings DB.
					bool originally_something = false;

					if (saved_correction_last_four_id != L"") {
						last_four_id = saved_correction_last_four_id;
						auto lfi_rentry = pDoc->nwsrs_four_map.find(saved_correction_last_four_id);
						int j = lfi_rentry->second;
						cc_rating = pDoc->rated_players[j].get_higher_rating();
						updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[j], normal_log, warning_condition, lm);
						nwsrs_rating = pDoc->rated_players[j].nwsrs_rating;
						uscf_expr = pDoc->rated_players[j].uscf_exp_date;

						//normal_log << "Found by digit ID string from ratings file " << std::endl;
						if (pDoc->rated_players[j].grade != grade_code) {
							notes << "Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[j].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << ". ";
							lm.grade_change << "    Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[j].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << " for player " << first_name << " " << last_name << std::endl;
							grade_code = pDoc->rated_players[j].grade;
						}
#if 0
						if (school_code.length() == 3) {
							full_id = school_code + pDoc->rated_players[j].grade + last_four_id;
							//									full_id = school_code + grade_code + last_four_id;
						}
						else {
							full_id = pDoc->rated_players[j].school_code + pDoc->rated_players[j].grade + last_four_id;
						}
#endif
						normal_log << "WARNING: Player ID corrected from " << full_id_save << " to " << full_id << " by matching the last 4 digits of their specified ID to those IDs in the ratings file for player " << first_name << " " << last_name << std::endl;
					} else {
						if (last_four_id.length() == 4) {
							originally_something = true;
							// They have an ID but the exact ID wasn't found in the list.  So, search by partial ID.
							bool found = false;
							for (unsigned j = 0; j < pDoc->rated_players.size(); ++j) {
								if (pDoc->rated_players[j].id == last_four_id) {
									std::wstringstream wss;
									wss << "No ID or bad ID " << full_id_save << " specified for player " << upper_first << " " << upper_last << " grade code: " << grade_code <<
										" school: " << school << ".  However, a potential match has been found in the ratings file with name " <<
										pDoc->rated_players[j].first_name << " " << pDoc->rated_players[j].last_name <<
										" with ID: " << pDoc->rated_players[j].getFullId() <<
										" and grade code: " << pDoc->rated_players[j].grade <<
										" and school code: " << pDoc->rated_players[j].school_code << "(" <<
										pDoc->school_codes.findName(pDoc->rated_players[j].school_code) << ")" <<
										" Please verify if this is the correct person and click Yes if they are and No if they aren't.";
									int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);

									if (ret == IDYES) {
										cc_rating = pDoc->rated_players[j].get_higher_rating();
										updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[j], normal_log, warning_condition, lm);
										nwsrs_rating = pDoc->rated_players[j].nwsrs_rating;
										uscf_expr = pDoc->rated_players[j].uscf_exp_date;

										//normal_log << "Found by digit ID string from ratings file " << std::endl;
										if (pDoc->rated_players[j].grade != grade_code) {
											notes << "Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[j].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << ". ";
											lm.grade_change << "    Unexpected grade change from " << CStringToWString(getGradeStringShort(pDoc->rated_players[j].grade)) << " to " << CStringToWString(getGradeStringShort(grade_code)) << " for player " << first_name << " " << last_name << std::endl;
											grade_code = pDoc->rated_players[j].grade;
										}
#if 0
										if (school_code.length() == 3) {
											full_id = school_code + pDoc->rated_players[j].grade + last_four_id;
											//									full_id = school_code + grade_code + last_four_id;
										}
										else {
											full_id = pDoc->rated_players[j].school_code + pDoc->rated_players[j].grade + last_four_id;
										}
#endif
										found = true;
										normal_log << "WARNING: Player ID corrected from " << full_id_save << " to " << full_id << " by matching the last 4 digits of their specified ID to those IDs in the ratings file for player " << first_name << " " << last_name << std::endl;

										auto cfiter = pDoc->get_corrected_iter(unique_key);
										cfiter->second.id = last_four_id;

										break;
									}
								}
							}
							// Do somethere here if the player was not found.
							if (!found) {
								full_id = L"";
								//error_condition = true;
								//normal_log << "ERROR: Did not find player ID " << last_four_id << " in the ratings file. " << full_id << " " << last_name << " " << first_name << " " << grade_code << " " << school << std::endl;
							}
						}
					}

					if (full_id == L"NONE" || full_id == L"N/A" || full_id == L"") {
						std::wstring saved_correction_last_four_id = pDoc->get_saved_correction_id(unique_key);
						// No exact full id matching in ratings DB.
						bool originally_something = false;

						if (saved_correction_last_four_id != L"") {
							last_four_id = saved_correction_last_four_id;
							auto lfi_rentry = pDoc->nwsrs_four_map.find(saved_correction_last_four_id);
							int j = lfi_rentry->second;
							cc_rating = pDoc->rated_players[j].get_higher_rating();
							updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[j], normal_log, warning_condition, lm);
							nwsrs_rating = pDoc->rated_players[j].nwsrs_rating;
							uscf_expr = pDoc->rated_players[j].uscf_exp_date;
						}
						else {
							std::vector<int> exact_match = findExactMatch(upper_last, upper_first, grade_code, pDoc->rated_players);
							bool match_found = false;
							for (auto match = exact_match.begin(); match != exact_match.end(); ++match) {
								std::wstringstream wss;
								wss << "No ID or bad ID " << full_id_save << " specified for player " << upper_first << " " << upper_last << " grade code: " << grade_code << " school: " << school
									<< ".  However, a player with the same first and last name was found in the ratings file with ID: " << pDoc->rated_players[*match].getFullId() <<
									" and grade code: " << pDoc->rated_players[*match].grade <<
									" and school code: " << pDoc->rated_players[*match].school_code << "(" <<
									pDoc->school_codes.findName(pDoc->rated_players[*match].school_code) << ")" <<
									" Please verify if this is the same person and click Yes if they are and No if they aren't.";
								int ret = MessageBox(hWnd, WStringToCString(wss.str()), _T("Matching name found in ratings file."), MB_ICONQUESTION | MB_YESNO);

								if (ret == IDYES) {
									warning_condition = true;
									//normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;
									//normal_log << "WARNING: no ID specified but exact match for player found in player list " << last_name << " " << first_name << " grade code = " << grade_code << " " << school << std::endl;

									full_id = pDoc->rated_players[*match].getFullId();
									last_four_id = getLastFour(full_id);
									cc_rating = pDoc->rated_players[*match].get_higher_rating();
									updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[*match], normal_log, warning_condition, lm);
									nwsrs_rating = pDoc->rated_players[*match].nwsrs_rating;
									uscf_expr = pDoc->rated_players[*match].uscf_exp_date;
									match_found = true;
									normal_log << "WARNING: Player ID corrected from " << full_id_save << " to " << full_id << " by an exact match of their first and last names in the ratings file for player " << first_name << " " << last_name << std::endl;
									auto cfiter = pDoc->get_corrected_iter(unique_key);
									cfiter->second.id = getLastFour(full_id);
								}
							}
							if (!match_found) {
								if (originally_something) {
									error_condition = true;
									normal_log << "ERROR: Did not find player ID " << last_four_id << " in the ratings file. " << full_id << " " << last_name << " " << first_name << " " << grade_code << " " << school << std::endl;
								}
								else {
									full_id = school_code + grade_code;
									cc_rating = (grade_code - 'A') * 100;
									nwsrs_rating = cc_rating;
									unrated = true;

									info_condition = true;
									normal_log << "INFO: new tournament player without a previous ID " << first_name << " " << last_name << ", grade: " << grade_code << ", school: " << school << std::endl;
								}
							}
						}
					}
#if 0
					else {
						// They have an ID but the exact ID wasn't found in the list.  So, search by partial ID.
						bool found = false;
						for (unsigned j = 0; j < pDoc->rated_players.size(); ++j) {
							if (pDoc->rated_players[j].id == last_four_id) {
								cc_rating = pDoc->rated_players[j].get_higher_rating();
								updateUscfFromRatingFile(uscf_id, uscf_rating, pDoc->rated_players[j], normal_log, warning_condition, lm);
								nwsrs_rating = pDoc->rated_players[j].nwsrs_rating;
								uscf_expr = pDoc->rated_players[j].uscf_exp_date;

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
#endif
				}

				std::wstring ratings_school_code = L"";
				if (full_id.length() == 8) {
					ratings_school_code = full_id.substr(0, 3);
				}

				// First check if we've previously verified the school for this player.
				std::wstring saved_correction_school_code = pDoc->get_saved_correction_school_code(unique_key);

				if (saved_correction_school_code != L"") {
					school_code = saved_correction_school_code;
					school = pDoc->school_codes.findName(school_code);
				}
				else {
					// School is unverified.
					if (ratings_school_code != L"") {
						if (school == L"") {
							school_code = ratings_school_code;
							school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
							normal_log << "school not specified...getting it from school code" << school << std::endl;
#endif
						}
						else if (school.length() == 3 && pDoc->school_codes.findName(toUpper(school)) != L"") {
							school_code = toUpper(school);
							school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
							normal_log << "school length is 3 so assuming school code " << school_code << std::endl;
#endif
							// If the calculated school code is not what is in their rating and we didn't previously ask the user
							// about this person then ask them now so that we don't silently change a school code that might exactly
							// match what they entered.
							if (school_code != ratings_school_code) {
								SchoolSelector ss_dialog(pDoc, school, ratings_school_code, school_code, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

								if (ss_dialog.DoModal() == IDOK) {
									school = pDoc->school_codes.findName(school_code);
#ifdef DEBUG_MAIN
									normal_log << "school code after asking user " << school_code << " " << school << std::endl;
#endif
								}
								else {
									error_condition = true;
									normal_log << "ERROR: no confirmation of change of school code for " << school << " for player " << last_name << " " << first_name << " " << std::endl;
									continue;
								}
							}
						}
						else if (pDoc->school_codes.schoolNameMatchesCode(school, ratings_school_code)) {
							school_code = ratings_school_code;
							school = pDoc->school_codes.findName(school_code);
						}
						else {
							SchoolSelector ss_dialog(pDoc, school, ratings_school_code, school_code, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

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
					else {
						// ratings school code empty
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
								if (school == L"") {
									error_condition = true;
									normal_log << "ERROR: no school or NWSRS ID entered for player " << last_name << " " << first_name << " " << std::endl;
									continue;
								}
								else {
									SchoolSelector ss_dialog(pDoc, school, ratings_school_code, school_code, full_name, CStringToWString(getGradeString(grade_code)), unique_key);

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
				}

				full_id = school_code + grade_code + last_four_id;

				if (full_id.length() == 8 && school_code != L"" && ratings_school_code != school_code && ratings_school_code != L"") {
					info_condition = true;
					std::wstring print_school = orig_school;
					if (print_school == L"") {
						print_school = L"<None>";
					}
					lm.school_change << "    INFO: Player " << first_name << " " << last_name << " with ID specified as " << full_id << " and school as " << orig_school << " change of school from " << ratings_school_code << " to " << school_code << std::endl;
					//normal_log << "INFO: Player " << first_name << " " << last_name << " with ID specified as " << full_id << " and school as " << orig_school << " change of school from " << ratings_school_code << " to " << school_code << std::endl;

					full_id.replace(0, 3, school_code); // Update school code in their ID.
				}

				if (school_code != L"" && ratings_school_code != school_code) {
					std::wstring rsc_name = pDoc->school_codes.findName(ratings_school_code);
					std::wstring sc_name = pDoc->school_codes.findName(school_code);
					if (rsc_name != L"") {
						notes << "Verify school change from " << rsc_name << " to " << sc_name << ". ";
					}
				}

				if (!id_perfect) {
					if (full_id_save != L"NONE") {
						lm.imperfect << "    INFO: The incorrectly specified NWSRS ID " << full_id_save << " in the Constant Contact information for player " << first_name << " " << last_name << " has been corrected to ID " << full_id << std::endl;
					}
					//normal_log << "INFO: The incorrectly specified NWSRS ID " << full_id_save << " in the Constant Contact information for player " << first_name << " " << last_name << " has been corrected to ID " << full_id << std::endl;
				}

				CString cs_last_name(last_name.c_str());
				CString cs_first_name(first_name.c_str());
				CString cs_school(school.c_str());
				CString cs_school_code(school_code.c_str());
				cs_full_id = full_id.c_str();
				CString cs_registration_date(registration_date.c_str());

				CString cs_adult_first(adult_first.c_str()), cs_adult_last(adult_last.c_str());
				auto spi = SectionPlayerInfo(-((int)i + 1), cs_last_name, cs_first_name, cs_full_id, nwsrs_rating, grade_code, cs_school, cs_school_code, CString(uscf_id.c_str()), CString(uscf_rating.c_str()), CString(uscf_expr.c_str()), notes.str(), unrated, cc_rating, cs_adult_first, cs_adult_last, cs_registration_date);

				//if (pDoc->noshows.find(spi.getUnique()) == pDoc->noshows.end()) {
					auto dditer = duplicate_detect.find(last_four_id);
					if (last_four_id == L"" || dditer == duplicate_detect.end()) {
						post_proc.push_back(spi);
//						post_proc.push_back(SectionPlayerInfo(-((int)i + 1), cs_last_name, cs_first_name, cs_full_id, nwsrs_rating, grade_code, cs_school, cs_school_code, CString(uscf_id.c_str()), CString(uscf_rating.c_str()), CString(uscf_expr.c_str()), notes.str(), unrated, cc_rating, cs_adult_first, cs_adult_last));
						duplicate_detect.insert(last_four_id);
					}
					else if (last_four_id != L"") {
						normal_log << "DUPLICATE PLAYER: " << first_name << " " << last_name << std::endl;
					}
				//}
#endif
			}
		}

		parents_registering_no_children(post_proc, adult_map, error_condition, normal_log);
	}

	return post_proc;
}

bool willExpire(const std::wstring &uscf_expr_date, const CTime &tournament_date) {
	std::wstringstream indate(uscf_expr_date);
	wchar_t ch;
	int month=0, day=0, year=0;
	try {
		indate >> month;
		indate >> ch;
		indate >> day;
		indate >> ch;
		indate >> year;
		year += 2000;
	}
	catch (std::exception &) {
		//return false;
	}

	if (year < tournament_date.GetYear()) return true;
	if (year > tournament_date.GetYear()) return false;
	if (month < tournament_date.GetMonth()) return true;
	if (month > tournament_date.GetMonth()) return false;
	return day < tournament_date.GetDay();
}


// Click on the button to load sections from constant contact file.
void CCCSwisssys2View::OnCreateSections()
{
	auto pDoc = GetDocument();
	
	pDoc->clearPlayers();
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
	log_messages lm;

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
				if (rid.length() == 8 || rid.length() == 4) {
					restricted_set.insert(getLastFour(rid));
				}
				else {
					error_condition = true;
					normal_log << "ERROR: 8-digit ID not found in restricted file.  Found the following instead: " << rid << std::endl;
				}
			}
		}
	}

	entries.clear();
	adult_map.clear();
	auto post_proc = process_cc_file(this->GetSafeHwnd(), pDoc, entries, adult_map, error_condition, warning_condition, info_condition, normal_log, lm);

	std::map<int, int> ssmap;
	Sections with_parent_sections = pDoc->getSectionsWithParents(ssmap);

    for(auto ppiter = post_proc.begin(); ppiter != post_proc.end(); ++ppiter) {
		std::wstring unique_name = ppiter->getUnique();
		if (pDoc->noshows.find(unique_name) != pDoc->noshows.end()) {
			continue;
		}
		auto force_iter = pDoc->force_sections.find(unique_name);
		int target_section = with_parent_sections.foundIn(ppiter->cc_rating, ppiter->grade);
		if (force_iter != pDoc->force_sections.end()) {
			target_section = with_parent_sections.findByName(WStringToCString(force_iter->second));
		}
		if (target_section == -1) {
			error_condition = true;
			normal_log << "ERROR: Doesn't belong in any section. " << CStringToWString(ppiter->full_id) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
		}
		else if (target_section < -1) {
			error_condition = true;
			normal_log << "ERROR: Player can go in multiple sections. " << CStringToWString(ppiter->full_id) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
		}
		else {
			if (restricted_set.size() == 0 || restricted_set.find(getLastFour(CStringToWString(ppiter->full_id))) != restricted_set.end()) {
				if (with_parent_sections[target_section].uscf_required) {
					if (!isValidUSCFId(CStringToWString(ppiter->uscf_id))) {
						warning_condition = true;
						lm.no_uscf_membership << "    WARNING: Player in section requiring USCF membership did not specify a USCF ID and there is not one present in the ratings file. NWSRS=" << CStringToWString(ppiter->full_id) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
						//normal_log << "WARNING: Player in section requiring USCF membership did not specify a USCF ID and there is not one present in the ratings file. NWSRS=" << CStringToWString(ppiter->full_id) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
					}
					else {
						std::wstring ws_uscf_expr = CStringToWString(ppiter->uscf_expr);
						if (ws_uscf_expr.length() > 0 && willExpire(ws_uscf_expr, pDoc->getValidTournamentDate())) {
							warning_condition = true;
							lm.expired_uscf_membership << "    WARNING: Player in section requiring USCF membership will have an expired membership by the date of the tournament. NWSRS=" << CStringToWString(ppiter->full_id) << " USCF=" << CStringToWString(ppiter->uscf_id) << " Expired: " << CStringToWString(ppiter->uscf_expr) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
							//normal_log << "WARNING: Player in section requiring USCF membership will have an expired membership by the date of the tournament. NWSRS=" << CStringToWString(ppiter->full_id) << " USCF=" << CStringToWString(ppiter->uscf_id) << " Expired: " << CStringToWString(ppiter->uscf_expr) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
						}
						else if (ws_uscf_expr.length() == 0) {
							warning_condition = true;
							lm.expired_uscf_membership << "    WARNING: Player in section requiring USCF membership has no verifiable expiration date. NWSRS=" << CStringToWString(ppiter->full_id) << " USCF=" << CStringToWString(ppiter->uscf_id) << " Expired: " << CStringToWString(ppiter->uscf_expr) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
						}
					}
				}
				with_parent_sections[target_section].players.push_back(*ppiter);
			}
			else {
				//error_condition = true;
				warning_condition = true;
				normal_log << "ERROR: Player in constant contact not in the restricted tournament id list. " << CStringToWString(ppiter->full_id) << " " << CStringToWString(ppiter->last_name) << " " << CStringToWString(ppiter->first_name) << " " << ppiter->cc_rating << " " << ppiter->grade << " " << CStringToWString(ppiter->school) << std::endl;
			}
			//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << full_id << " " << last_name << " " << first_name << " " << cc_rating << " " << grade_code << " " << school_code << " uscf rating and id " << uscf_rating << " " << uscf_id << std::endl;
		}
	}

	unsigned i;
	// Add "manage additional registration" players to sections.
	for (i = 0; i < pDoc->mrplayers.size(); ++i) {
		MRPlayer &this_player = pDoc->mrplayers[i];
		std::wstring unique_name = this_player.getUnique();
		auto force_iter = pDoc->force_sections.find(unique_name);
		int target_section = with_parent_sections.foundIn(pDoc->mrplayers[i].nwsrs_rating, pDoc->mrplayers[i].grade);
		if (force_iter != pDoc->force_sections.end()) {
			target_section = with_parent_sections.findByName(WStringToCString(force_iter->second));
		}
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
				CString cs_uscf_expr(this_player.ws_uscf_expr.c_str());

				with_parent_sections[target_section].players.push_back(SectionPlayerInfo(i, cs_last, cs_first, cs_id, this_player.nwsrs_rating, this_player.grade, cs_school_name, cs_school_code, cs_uscf_id, cs_uscf_rating, cs_uscf_expr, L"", false, this_player.nwsrs_rating, L"", L"", L""));
				//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << this_player.ws_id << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_code << " uscf rating and id " << this_player.ws_uscf_rating << " " << this_player.ws_uscf_id << std::endl;
			}
			else {
				error_condition = true;
				normal_log << "ERROR: Player in constant contact not in the restricted tournament id list. " << this_player.ws_id << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_name << std::endl;
			}
		}
	}

	// Go from with_parent_sections back to pDoc->sections.
	for (i = 0; i < with_parent_sections.size(); ++i) {
		auto ssiter = ssmap.find(i);
		ASSERT(ssiter != ssmap.end());
		int real_section_index = ssiter->second;
		if (pDoc->sections[real_section_index].parent_section == -1) {
			// Normal (non-automatic resectioning) section.
			pDoc->sections[real_section_index] = with_parent_sections[i];
		}
		else {
			// Automatic resectioning groups handled here.

			// Take the large parent section and split into sections based on the num_split field of the parent.
			auto split_sections = do_split(&with_parent_sections[i], with_parent_sections[i].num_split, GetSafeHwnd());
			// Get the sections that come from the given parent.
			auto parent_section_number = pDoc->sections[real_section_index].parent_section;
			auto previous_sections_sorted = pDoc->sections.getSectionsWithParentSorted(parent_section_number);
			unsigned j;
			unsigned sec_min = min(split_sections.size(), previous_sections_sorted.size());

			if (with_parent_sections[i].num_split == 0) {
				if (split_sections.size() < previous_sections_sorted.size()) {
					MessageBox(_T("Fewer quad sections need so deleting extraneous sections."), _T("ERROR"), MB_OK);
				}
				if (split_sections.size() > previous_sections_sorted.size()) {
					MessageBox(_T("More quad sections needed than previously so new sections created."), _T("ERROR"), MB_OK);
				}
			}
			else {
				ASSERT(split_sections.size() == previous_sections_sorted.size());
			}

			for (j = 0; j < sec_min; ++j) {
				// Copy from split sections into existing section.
				previous_sections_sorted[j]->copyResectioningFields(split_sections[j]);
			}
			for (j = sec_min; j < split_sections.size(); ++j) {
				// These are new sections.
				split_sections[j].parent_section = parent_section_number;
				pDoc->sections.push_back(split_sections[j]);
			}

			if (with_parent_sections[i].num_split == 0 && split_sections.size() < previous_sections_sorted.size()) {
				for (j = sec_min; j < previous_sections_sorted.size(); ++j) {
					bool eres = pDoc->sections.remove(previous_sections_sorted[j]);
					ASSERT(eres);
				}
			}
		}
	}

	pDoc->sections.makeSubsections();

	normal_log << std::endl;

	normal_log << "INCORRECTLY SPECIFIED NWSRS IDS\n" << "-------------------------------\n" << lm.imperfect.str() << std::endl;
	normal_log << "NAME CHANGES\n" << "------------\n" << lm.name_change.str() << std::endl;
	normal_log << "SCHOOL CHANGES\n" << "--------------\n" << lm.school_change.str() << std::endl;
	normal_log << "GRADE CHANGES\n" << "-------------\n" << lm.grade_change.str() << std::endl;
	normal_log << "MISSING USCF MEMBERSHIP\n" << "-----------------------\n" << lm.no_uscf_membership.str() << std::endl;
	normal_log << "EXPIRED USCF MEMBERSHIP\n" << "-----------------------\n" << lm.expired_uscf_membership.str() << std::endl;
	normal_log << "NEW USCF IDS\n" << "------------\n" << lm.new_uscf_ids.str() << std::endl;
	normal_log << "USCF ID MISMATCH\n" << "----------------\n" << lm.uscf_id_cc_no_match.str() << std::endl;

	normal_log << "\nOnce you have confirmed that you have resolved any INFO, WARNING, or ERROR messages correctly, please delete those lines before sending to the ratings coordinator.\n" << std::endl;

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

	refillSections(pDoc->sections, false);
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
	int first, second, focus;

	if (pNMUpDown->iDelta == -1) {
		// Move a section up.
		if (smark < 1) {
			return;
		}
		first = smark - 1;
		second = smark;
		focus = first;
	} else if (pNMUpDown->iDelta == 1) {
		// Move a section down.
		if (smark >= slen - 1) {
			return;
		}
		first = smark;
		second = smark + 1;
		focus = second;
	}
	else {
		MessageBox(_T("spin control idelta was not -1 or 1"), _T("ERROR"));
		return;
	}

	std::iter_swap(pDoc->sections.begin() + first, pDoc->sections.begin() + second);
	refillSections(pDoc->sections, false, focus);
	pDoc->SetModifiedFlag();
}

#include "helper.h"

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

int getSubsecStartingBoard(unsigned sub_sec, const std::wstring &starting_board) {
	int starting_board_num = 1;
	if (starting_board.length() != 0) {
		std::vector<std::wstring> tokens = tokenize(starting_board, std::wstring(L";"));
		int ssindex = sub_sec - 1;
		if (ssindex < tokens.size() && tokens[ssindex] != L"") {
			starting_board_num = _ttoi(tokens[ssindex].c_str());
		}
	}
	return starting_board_num;
}

std::wstring getSubsecPlayingRoom(unsigned sub_sec, const std::wstring &playing_rooms) {
	std::wstring res = L"";
	if (playing_rooms.length() != 0) {
		std::vector<std::wstring> tokens = tokenize(playing_rooms, std::wstring(L";"));
		int ssindex = sub_sec - 1;
		if (ssindex < tokens.size() && tokens[ssindex] != L"") {
			res = tokens[ssindex].c_str();
		}
	}
	return res;
}

void addToSwisssysSection(std::wofstream &tmt_file,
						  const std::wstring &secname, 
						  SECTION_TYPE sec_type, 
						  const std::wstring &time_control, 
						  const std::wstring &starting_board, 
						  unsigned num_rounds,
	                      unsigned sub_sec,
	                      const std::wstring &playing_room,
	                      bool uscf) {
	tmt_file << "[" << secname << "]" << std::endl;

	std::wstring sec_name_to_print = secname;
	std::wstring room_name = getSubsecPlayingRoom(sub_sec, playing_room);
	if (room_name != L"") {
		sec_name_to_print += L" - " + room_name;
	}
	tmt_file << "Section title = " << sec_name_to_print << std::endl;

	if (sec_type == SWISS) {
		tmt_file << "Event type = 0" << std::endl;
	}
	else {
		tmt_file << "Event type = 3" << std::endl;
	}
	int sb = getSubsecStartingBoard(sub_sec, starting_board);
	tmt_file << "Double blitz = 0" << std::endl;
	tmt_file << "Coin toss = 1" << std::endl;
	tmt_file << "Ratings = 0" << std::endl;
	tmt_file << "Time controls =" << time_control << std::endl;
	tmt_file << "Minimum rating = 0" << std::endl;
	tmt_file << "Maximum rating = 9999" << std::endl;
	tmt_file << "Minimum grade = 0" << std::endl;
	tmt_file << "Maximum grade = 99" << std::endl;
	tmt_file << "Starting board = " << sb << std::endl;
	tmt_file << "Rounds = " << num_rounds << std::endl;
	tmt_file << "Exclude batch = 0" << std::endl;
	tmt_file << "Exclude ratings = " << (uscf ? 0 : 1) << std::endl;
	tmt_file << "Exclude Krause = 0" << std::endl;
	tmt_file << "Rounds paired = 0" << std::endl;
}

#include "xlslib.h"
using namespace xlslib_core;

std::string toString(const std::wstring &ws) {
	std::string s(ws.begin(), ws.end());
	return s;
}

bool SectionIndexRatingsSort(const SectionIndex &a, const SectionIndex &b) {
	return a.section->players[a.index].cc_rating > b.section->players[b.index].cc_rating;
}

unsigned CCCSwisssys2View::createSectionWorksheet(
	std::wofstream &normal_log,
	const std::wstring &output_dir, 
	std::wstring &sec_name, 
	const Section &sec, 
	int subsec,
	std::vector<SectionIndex> &vsi) {

	workbook wb;
	worksheet* settings = wb.sheet("Settings");
	worksheet* players = wb.sheet("Players");
	worksheet* results = wb.sheet("Results");
	worksheet* tables = wb.sheet("Tables");

	workbook checkin_wb;
	worksheet* checkin = checkin_wb.sheet("Check-In");
	checkin->defaultColwidth(8);
	checkin->colwidth(0, 256 * 14);
	checkin->colwidth(1, 256 * 14);
	checkin->colwidth(2, 256 * 35);
	checkin->colwidth(3, 256 * 15);
	checkin->colwidth(4, 256 * 15);
	std::wstringstream header;
	header << "Check-In List for Section " << sec_name;
	checkin->label(0, 0, header.str());
	checkin->label(2, 0, "Last Name");
	checkin->label(2, 1, "First Name");
	checkin->label(2, 2, "School");
	checkin->label(2, 3, "Grade");
	checkin->label(2, 4, "ID");
	checkin->label(2, 5, "Present");
	checkin->label(2, 6, "Notes");

	unsigned i;
	unsigned num_players = 0;
	for (i = 0; i < sec.players.size(); ++i) {
		if (sec.players[i].subsection == subsec) {
			++num_players;
		}
	}

	int starting_board_num = getSubsecStartingBoard(subsec, CStringToWString(sec.starting_board_number));
	std::wstring sec_name_to_print = CStringToWString(sec.name);
#if 0
	std::wstring room_name = getSubsecPlayingRoom(subsec, CStringToWString(sec.playing_room));
	if (room_name != L"") {
		sec_name_to_print += L" - " + room_name;
	}
#endif

	settings->label(0, 0, "FILE_TYPE");     settings->label(1, 0, "SWISSSYS");
	settings->label(0, 1, "VERSION");       settings->label(1, 1, "9.14");
	settings->label(0, 2, "PLR_CNT");       settings->number(1, 2, num_players);
	settings->label(0, 3, "RDS_PLD");       settings->number(1, 3, 0);
	settings->label(0, 4, "RDS_PRD");       settings->number(1, 4, 0);
	settings->label(0, 5, "BYES");
	settings->label(0, 6, "TM_BYES");
	settings->label(0, 7, "QUICK");         settings->number(1, 7, 0);
	settings->label(0, 8, "1ST_BD");        settings->number(1, 8, starting_board_num);
	settings->label(0, 9, "LOW");           settings->number(1, 9, 0);
	settings->label(0, 10, "HIGH");         settings->number(1, 10, 9999);
	settings->label(0, 11, "LAST_RD");      settings->number(1, 11, sec.num_rounds);
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
	settings->label(0, 28, "SEC_TITLE");    settings->label(1, 28, sec_name_to_print);
	settings->label(0, 29, "SEC_TIME");     settings->label(1, 29, CStringToWString(sec.time_control));
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

	std::vector<SectionIndex> this_sorted_name, this_sorted_rating;

	for (i = 0; i < sec.players.size(); ++i) {
		if (sec.players[i].subsection != subsec) continue;
		vsi.push_back(SectionIndex(&sec, i));
		this_sorted_name.push_back(SectionIndex(&sec, i));
		this_sorted_rating.push_back(SectionIndex(&sec, i));
	}

	std::sort(this_sorted_name.begin(), this_sorted_name.end());
	std::sort(this_sorted_rating.begin(), this_sorted_rating.end(), SectionIndexRatingsSort);

	unsigned j;
	for (j = 0; j < this_sorted_rating.size(); ++j) {
		unsigned i = this_sorted_rating[j].index;
		std::wstring name_field = toUpper(CStringToWString(sec.players[i].last_name)) + L", " +
			toUpper(CStringToWString(sec.players[i].first_name));
		players->label(j+1, 0, toString(name_field));
		std::wstring id_field = CStringToWString(sec.players[i].full_id);
		players->label(j+1, 1, toString(id_field));

		if (!sec.players[i].uscf_id.IsEmpty()) {
			std::wstring id2_field = CStringToWString(sec.players[i].uscf_id);
			players->label(j+1, 2, toString(id2_field));
		}

		if (sec.players[i].unrated) {
			players->number(j+1, 3, 0);
		}
		else {
			players->number(j+1, 3, sec.players[i].rating);
		}

		if (!sec.players[i].uscf_rating.IsEmpty()) {
			std::wstring rtng2_field = CStringToWString(sec.players[i].uscf_rating);
			players->label(j+1, 4, toString(rtng2_field));
		}
		auto miter = sibling_club_names.find(sec.players[i].last_name);
		if (miter != sibling_club_names.end()) {
			players->number(j+1, 7, miter->second);
		}
		std::wstring school_code_ws = CStringToWString(sec.players[i].school_code);
		if (school_code_ws != L"HSO") {
			players->label(j+1, 8, toString(school_code_ws));
		}

		std::wstring grade_ws = CStringToWString(getGradeStringShort(sec.players[i].grade));
		players->label(j+1, 11, toString(grade_ws));

		players->number(j+1, 28, 0);
		players->label(j+1, 31, "FALSE");
		players->number(j+1, 38, 0);
	}

	for (j = 0; j < this_sorted_name.size(); ++j) {
		unsigned i = this_sorted_name[j].index;

		checkin->label(3 + j, 0, toString(capWords(CStringToWString(sec.players[i].last_name))));  // last
		checkin->label(3 + j, 1, toString(capWords(CStringToWString(sec.players[i].first_name)))); // first
		checkin->label(3 + j, 2, toString(capWords(CStringToWString(sec.players[i].school)))); // school
		checkin->label(3 + j, 3, toString(CStringToWString(getGradeStringShort(sec.players[i].grade)))); // grade
		checkin->label(3 + j, 4, toString(toUpper(CStringToWString(sec.players[i].full_id)))); // ID

		std::wstringstream notes_field;

		auto miter = sibling_club_names.find(sec.players[i].last_name);
		if (miter != sibling_club_names.end()) {
			notes_field << "Verify sibling also playing in section.";
		}

		if (notes_field.str().length() != 0) {
			notes_field << " ";
		}
		notes_field << sec.players[i].notes;
			
		if (notes_field.str().length() != 0) {
			checkin->label(3 + j, 6, toString(notes_field.str()));
		}
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

	return this_sorted_rating.size();
}

template <class M, class Key>
typename M::iterator findOrAdd(M &m, Key const&k, typename M::mapped_type const& v) {
	return m.insert(typename M::value_type(k, v)).first;
}

void CCCSwisssys2View::createAllCheckinWorksheet(
	std::wofstream &normal_log,
	const std::wstring &output_dir,
	const std::vector<SectionIndex> &vsi) {

	workbook checkin_wb;
	worksheet* checkin = checkin_wb.sheet("Check-In");
	checkin->defaultColwidth(8);
	checkin->colwidth(0, 256 * 8);
	checkin->colwidth(1, 256 * 14);
	checkin->colwidth(2, 256 * 14);
	checkin->colwidth(3, 256 * 14);
	checkin->colwidth(4, 256 * 35);
	checkin->colwidth(5, 256 * 10);
	checkin->colwidth(6, 256 * 15);

	checkin->label(0, 0, "Check-In List for all sections");
	checkin->label(2, 0, "Present");
	checkin->label(2, 1, "Last Name");
	checkin->label(2, 2, "First Name");
	checkin->label(2, 3, "Section");
	checkin->label(2, 4, "School");
	checkin->label(2, 5, "Grade");
	checkin->label(2, 6, "ID");
	checkin->label(2, 7, "Notes");

	workbook td_wb;
	worksheet* td_info = td_wb.sheet("Info");
	worksheet* td_sections = td_wb.sheet("Section");
	td_info->defaultColwidth(8);
	td_info->colwidth(0, 256 * 14);  // last name
	td_info->colwidth(1, 256 * 14);  // first name
	td_info->colwidth(2, 256 * 14);  // section
	td_info->colwidth(3, 256 * 30);  // school
	td_info->colwidth(4, 256 * 6);   // grade
	td_info->colwidth(5, 256 * 10);  // ID
	td_info->colwidth(6, 256 * 6);   // rating
	td_info->colwidth(7, 256 * 18);  // adult
	td_info->colwidth(8, 256 * 11);  // phone
	td_info->colwidth(9, 256 * 30);  // email
	td_info->colwidth(10, 256 * 30); // registration date/time
	td_info->colwidth(11, 256 * 12); // playing room
	td_info->colwidth(12, 256 * 30); // notes

	td_sections->colwidth(0, 256 * 14); // name
	td_sections->colwidth(1, 256 * 14); // min rating
	td_sections->colwidth(2, 256 * 14); // max rating
	td_sections->colwidth(3, 256 * 14); // min grade
	td_sections->colwidth(4, 256 * 14); // max grade
	td_sections->colwidth(5, 256 * 14); // type
	td_sections->colwidth(6, 256 * 14); // num players
	td_sections->colwidth(7, 256 * 14); // subsections
	td_sections->colwidth(8, 256 * 14); // computer
	td_sections->colwidth(9, 256 * 14); // time control
	td_sections->colwidth(10, 256 * 14); // board numbers
	td_sections->colwidth(11, 256 * 14); // playing room
	td_sections->colwidth(12, 256 * 14); // number of rounds

	td_sections->label(0, 0, "Name");
	td_sections->label(0, 1, "Min Rating");
	td_sections->label(0, 2, "Max Rating");
	td_sections->label(0, 3, "Min Grade");
	td_sections->label(0, 4, "Max Grade");
	td_sections->label(0, 5, "Type");
	td_sections->label(0, 6, "Number of players");
	td_sections->label(0, 7, "Subsections");
	td_sections->label(0, 8, "Computer");
	td_sections->label(0, 9, "Time Control");
	td_sections->label(0, 10, "Board Numbers");
	td_sections->label(0, 11, "Playing Room");
	td_sections->label(0, 12, "Number of rounds");

	td_info->label(0, 0, "Last Name");
	td_info->label(0, 1, "First Name");
	td_info->label(0, 2, "Section");
	td_info->label(0, 3, "School");
	td_info->label(0, 4, "Grade");
	td_info->label(0, 5, "ID");
	td_info->label(0, 6, "Rating");
	td_info->label(0, 7, "Adult");
	td_info->label(0, 8, "Phone");
	td_info->label(0, 9, "Email");
	td_info->label(0, 10, "Registration Date");
	td_info->label(0, 11, "Playing Room");
	td_info->label(0, 12, "Notes");

	std::map<const Section *, std::map<unsigned, std::map<CString, unsigned> > > sibling_count;
	std::map<const Section *, std::map<unsigned, std::map<CString, unsigned> > > sibling_club_names;

	unsigned i;
	for (i = 0; i < vsi.size(); ++i) {
		auto siter = findOrAdd(sibling_count, vsi[i].section, std::map<unsigned, std::map<CString, unsigned> >());
		auto scniter = findOrAdd(sibling_club_names, vsi[i].section, std::map<unsigned, std::map<CString, unsigned> >());
		auto uiter = findOrAdd(siter->second, vsi[i].section->players[vsi[i].index].subsection, std::map<CString, unsigned>());
		auto scnuiter = findOrAdd(scniter->second, vsi[i].section->players[vsi[i].index].subsection, std::map<CString, unsigned>());
		auto miter = findOrAdd(uiter->second, vsi[i].section->players[vsi[i].index].last_name, 0);
		miter->second++;
	}

	unsigned next_club_id = 0;

	for (auto siter = sibling_count.begin(); siter != sibling_count.end(); ++siter) {
		for (auto uiter = siter->second.begin(); uiter != siter->second.begin(); ++uiter) {
			for (auto miter = uiter->second.begin(); miter != uiter->second.end(); ++miter) {
				if (miter->second > 1) {
					auto scniter = sibling_club_names.find(siter->first);
					auto scnuiter = scniter->second.find(uiter->first);
					scnuiter->second.insert(std::pair<CString, unsigned>(miter->first, next_club_id++));
				}
			}
		}
	}

	auto pDoc = GetDocument();
	unsigned sec_index = 0;
	for (sec_index = 0; sec_index < pDoc->sections.size(); ++sec_index) {
		td_sections->label(1 + sec_index, 0, toString(CStringToWString(pDoc->sections[sec_index].name)));
		td_sections->number(1 + sec_index, 1, pDoc->sections[sec_index].lower_rating_limit);
		td_sections->number(1 + sec_index, 2, pDoc->sections[sec_index].upper_rating_limit);
		td_sections->label(1 + sec_index, 3, toString(std::wstring(1, pDoc->sections[sec_index].lower_grade_limit)));
		td_sections->label(1 + sec_index, 4, toString(std::wstring(1, pDoc->sections[sec_index].upper_grade_limit)));
		td_sections->label(1 + sec_index, 5, toString(CStringToWString(getSectionTypeString(pDoc->sections[sec_index].sec_type))));
		td_sections->number(1 + sec_index, 6, (unsigned)pDoc->sections[sec_index].players.size());
		td_sections->number(1 + sec_index, 7, pDoc->sections[sec_index].num_subsections);
		td_sections->number(1 + sec_index, 8, pDoc->sections[sec_index].which_computer);
		td_sections->label(1 + sec_index, 9, toString(CStringToWString(pDoc->sections[sec_index].time_control)));

		int last_board_number = pDoc->sections[sec_index].getLastBoardNumber();
		std::wstringstream ss;
		ss << CStringToWString(pDoc->sections[sec_index].starting_board_number) << L"-" << last_board_number;

		td_sections->label(1 + sec_index, 10, toString(ss.str()));
		td_sections->label(1 + sec_index, 11, toString(CStringToWString(pDoc->sections[sec_index].playing_room)));
		td_sections->number(1 + sec_index, 12, pDoc->sections[sec_index].num_rounds);
	}

	unsigned cur_row = 1;
	for (i = 0; i < vsi.size(); ++i) {
		std::wstring sec_name = vsi[i].section->getPrintName(vsi[i].section->players[vsi[i].index].subsection);
		std::wstring playing_room = vsi[i].section->playing_room;
		std::wstring room_name = getSubsecPlayingRoom(vsi[i].section->players[vsi[i].index].subsection, playing_room);

		checkin->label(3 + i, 1, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].last_name))));  // last
		checkin->label(3 + i, 2, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].first_name)))); // first
		checkin->label(3 + i, 3, toString(sec_name)); // section name
		checkin->label(3 + i, 4, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].school)))); // school
		checkin->label(3 + i, 5, toString(CStringToWString(getGradeStringShort(vsi[i].section->players[vsi[i].index].grade)))); // grade
		checkin->label(3 + i, 6, toString(toUpper(CStringToWString(vsi[i].section->players[vsi[i].index].full_id)))); // ID

		td_info->label(1 + i, 0, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].last_name))));  // last
		td_info->label(1 + i, 1, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].first_name)))); // first
		td_info->label(1 + i, 2, toString(sec_name)); // section name
		td_info->label(1 + i, 3, toString(capWords(CStringToWString(vsi[i].section->players[vsi[i].index].school)))); // school
		td_info->label(1 + i, 4, toString(CStringToWString(getGradeStringShort(vsi[i].section->players[vsi[i].index].grade)))); // grade
		td_info->label(1 + i, 5, toString(toUpper(CStringToWString(vsi[i].section->players[vsi[i].index].full_id)))); // ID
		td_info->number(1 + i, 6, vsi[i].section->players[vsi[i].index].cc_rating); // higher of nwsrs and uscf ratings
		td_info->label(1 + i, 7, toString(CStringToWString(vsi[i].section->players[vsi[i].index].adult_first + L" " + vsi[i].section->players[vsi[i].index].adult_last))); // responsible adult name

		std::wstring adult_combined = CStringToWString(vsi[i].section->players[vsi[i].index].adult_last + vsi[i].section->players[vsi[i].index].adult_first);
		std::wstring adult_email = L"";
		std::wstring adult_phone = L"";

		auto adult_iter = adult_map.find(adult_combined);
		if (adult_iter != adult_map.end()) {
			int adult_index = adult_iter->second;
			adult_email = entries[adult_index].getEmail();
			adult_phone = entries[adult_index].getPhone();
		}

		std::wstring ws_phone = adult_phone;
		ws_phone.erase(std::remove(ws_phone.begin(), ws_phone.end(), '('), ws_phone.end());
		ws_phone.erase(std::remove(ws_phone.begin(), ws_phone.end(), ')'), ws_phone.end());
		ws_phone.erase(std::remove(ws_phone.begin(), ws_phone.end(), ' '), ws_phone.end());
		ws_phone.erase(std::remove(ws_phone.begin(), ws_phone.end(), '-'), ws_phone.end());
		td_info->label(1 + i, 8, toString(ws_phone)); // phone
		td_info->label(1 + i, 9, toString(adult_email)); // email
		td_info->label(1 + i, 10, toString(CStringToWString(vsi[i].section->players[vsi[i].index].registration_date))); // registration date/time
		if (room_name != L"") {
			td_info->label(1 + i, 11, toString(room_name)); // playing room
		}
#if 0
		int section_index = pDoc->sections.findByName(WStringToCString(sec_name));
		if (section_index >= 0) {
			td_info->label(1 + i, 11, toString(CStringToWString(pDoc->sections[section_index].playing_room))); // playing room
		}
#endif
		
		std::wstringstream notes_field;

		auto scnuiter = sibling_club_names.find(vsi[i].section)->second.find(vsi[i].section->players[vsi[i].index].subsection)->second;
		auto miter = scnuiter.find(vsi[i].section->players[vsi[i].index].last_name);
		if (miter != scnuiter.end()) {
			notes_field << "Verify sibling also playing in section.";
		}

		if (notes_field.str().length() != 0) {
			notes_field << " ";
		}
		notes_field << vsi[i].section->players[vsi[i].index].notes;

		if (notes_field.str().length() != 0) {
			checkin->label(3 + i, 7, toString(notes_field.str()));
			td_info->label(1 + i, 12, toString(notes_field.str()));
		}

		++cur_row;
	}

	std::wstring checkin_s = output_dir + L"\\all-checkin.xls";
	std::string checkin_filename = toString(checkin_s);
	int err = checkin_wb.Dump(checkin_filename);

	if (err != NO_ERRORS) {
		MessageBox(_T("Failed to create the all section check-in workbook.  Do you have the previous check-in sheet open in Excel?"), _T("Error"));
	}

	std::wstring td_s = output_dir + L"\\td-info.xls";
	std::string td_filename = toString(td_s);
	err = td_wb.Dump(td_filename);

	if (err != NO_ERRORS) {
		MessageBox(_T("Failed to create the td-info workbook.  Do you have the previous td-info sheet open in Excel?"), _T("Error"));
	}
}

void CCCSwisssys2View::OnCreateSwisssysTourney() {
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
				err << "Error " << errno << " when creating directory " << dir_base.str() << ".";
				if (errno == EEXIST) {

				}
				else {
					MessageBox(err.str().c_str(), _T("Error"));
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
				std::wstring sec_name = baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k + 1);
				tmt_file << "Section" << ++cur_sec_num << " = " << sec_name << std::endl;
			}
		}
	}
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		std::wofstream &tmt_file = *(tmts.find(pDoc->sections[j].which_computer)->second);
		if (pDoc->sections[j].num_subsections == 1) {
			addToSwisssysSection(tmt_file, 
				CStringToWString(pDoc->sections[j].name), 
				pDoc->sections[j].sec_type, 
				CStringToWString(pDoc->sections[j].time_control), 
				CStringToWString(pDoc->sections[j].starting_board_number),
				pDoc->sections[j].num_rounds,
				1,
				CStringToWString(pDoc->sections[j].playing_room),
				pDoc->sections[j].uscf_required);
		}
		else {
			for (unsigned k = 0; k < pDoc->sections[j].num_subsections; ++k) {
				addToSwisssysSection(tmt_file, 
					baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k + 1), 
					pDoc->sections[j].sec_type, 
					CStringToWString(pDoc->sections[j].time_control), 
					CStringToWString(pDoc->sections[j].starting_board_number),
					pDoc->sections[j].num_rounds,
					k + 1,
					CStringToWString(pDoc->sections[j].playing_room),
					pDoc->sections[j].uscf_required);
			}
		}
	}
	for (auto tmtiter = tmts.begin(); tmtiter != tmts.end(); ++tmtiter) {
		tmtiter->second->close();
	}

	unsigned total_placed = 0;

	std::vector<SectionIndex> vsi;

	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		auto oditer = output_dirs.find(pDoc->sections[j].which_computer);
		if (pDoc->sections[j].num_subsections == 1) {
			std::wstring sec_name = CStringToWString(pDoc->sections[j].name);
			total_placed += createSectionWorksheet(normal_log, oditer->second, sec_name, pDoc->sections[j], 1, vsi);
		}
		else {
			for (unsigned k = 0; k < pDoc->sections[j].num_subsections; ++k) {
				std::wstring sec_name = baseToNameWithSub(CStringToWString(pDoc->sections[j].name), k + 1);
				total_placed += createSectionWorksheet(normal_log, oditer->second, sec_name, pDoc->sections[j], k+1, vsi);
			}
		}
	}

	std::sort(vsi.begin(), vsi.end());
	createAllCheckinWorksheet(normal_log, output_dir, vsi);

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
			SectionEditor se(&newSection, pDoc, check);
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
			refillSections(pDoc->sections, check, selected_row);
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

		if (selected_section.parent_section != -1) {
			int ret = MessageBox(_T("The selected section cannot be split because it is currently part of an automatic resectioning group.  Would you like to turn off automatic resectioning for that group so that this section can be split?"), _T("Turn off automatic resection?"), MB_ICONQUESTION | MB_YESNO);
			if (ret == IDYES) {
				pDoc->turnOffResectioning(selected_section.parent_section, true);
			}
			else {
				return;
			}
		}

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