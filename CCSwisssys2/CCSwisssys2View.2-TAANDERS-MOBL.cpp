
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCCSwisssys2View

IMPLEMENT_DYNCREATE(CCCSwisssys2View, CFormView)

BEGIN_MESSAGE_MAP(CCCSwisssys2View, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CCCSwisssys2View::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCCSwisssys2View::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &CCCSwisssys2View::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CCCSwisssys2View::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCCSwisssys2View::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CCCSwisssys2View::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CCCSwisssys2View::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CCCSwisssys2View::OnBnClickedButton6)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CCCSwisssys2View::OnHdnItemclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CCCSwisssys2View::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON7, &CCCSwisssys2View::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CCCSwisssys2View::OnBnClickedButton8)
	ON_EN_CHANGE(IDC_EDIT3, &CCCSwisssys2View::OnEnChangeEdit3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CCCSwisssys2View::OnDeltaposSpin1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST2, &CCCSwisssys2View::OnLvnColumnclickList2)
	ON_BN_CLICKED(IDC_BUTTON9, &CCCSwisssys2View::OnBnClickedButton9)
END_MESSAGE_MAP()

// CCCSwisssys2View construction/destruction

CCCSwisssys2View::CCCSwisssys2View()
	: CFormView(IDD_CCSWISSSYS2_FORM)
{
	// TODO: add construction code here
	first_time = true;
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
	ResizeParentToFit();

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

		section_players.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	}

	//-------------------------------------------------------------------------------

	auto pDoc = GetDocument();
	ratings_file_edit.SetWindowText(pDoc->ratings_file);
	constant_contact_file_edit.SetWindowText(pDoc->constant_contact_file);
	SchoolCodesEdit.SetWindowTextW(pDoc->school_code_file);
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


void CCCSwisssys2View::OnBnClickedButton1()
{
	CFileDialog FileDialog(TRUE, _T("dat"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->ratings_file = PathName;

		//MessageBox((LPCTSTR)PathName, _T("Ratings Filename"));
		ratings_file_edit.SetWindowText(PathName);
	}
}


void CCCSwisssys2View::OnBnClickedButton2()
{
	CFileDialog FileDialog(TRUE, _T("csv"));

	if (FileDialog.DoModal() == IDOK)
	{
		CString PathName = FileDialog.GetPathName();
		// Do something with 'PathName'
		GetDocument()->constant_contact_file = PathName;

		//MessageBox((LPCTSTR)PathName, _T("Constant Contact Registration Filename"));
		constant_contact_file_edit.SetWindowText(PathName);
	}
}


void CCCSwisssys2View::OnEnChangeEdit1()
{
	CString sWindowText;
	ratings_file_edit.GetWindowText(sWindowText);
	GetDocument()->ratings_file = sWindowText;
}


void CCCSwisssys2View::OnEnChangeEdit2()
{
	CString sWindowText;
	constant_contact_file_edit.GetWindowText(sWindowText);
	GetDocument()->constant_contact_file = sWindowText;
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
}

void CCCSwisssys2View::refillSections(Sections &s) {
	unsigned i;
	section_list.DeleteAllItems();
	for (i = 0; i < s.size(); ++i) {
		addToSectionList(section_list, s[i], i);
	}
	section_list.SetSelectionMark(-1);
	section_players.DeleteAllItems();
	auto pDoc = GetDocument();
	pDoc->sections.clearPlayers();
}

// When they click the add section button.
void CCCSwisssys2View::OnBnClickedButton3()
{
	Section newSection;
	while (true) {
		SectionEditor se(&newSection);
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
		return;
	}
}

// Click on the delete section button.
void CCCSwisssys2View::OnBnClickedButton4()
{
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.DeleteItem(selected_row);
		auto pDoc = GetDocument();
		pDoc->sections.erase(pDoc->sections.begin() + selected_row);
		refillSections(pDoc->sections);
	}
}


// Click on the edit section button.
void CCCSwisssys2View::OnBnClickedButton5()
{
	int selected_row = section_list.GetSelectionMark();
	if (selected_row >= 0) {
		section_list.SetSelectionMark(-1);
		auto pDoc = GetDocument();
		Section newSection = pDoc->sections[selected_row];
		while (true) {
			SectionEditor se(&newSection);
			INT_PTR nRet = se.DoModal();
			if (nRet != IDOK) {
				return;
			}

			//MessageBox(_T("Debugging"), _T("User pressed OK button on dialog."));

			int conflict = pDoc->sections.conflicts(newSection, selected_row);

			if (conflict >= 0) {
				//std::wstringstream ss;
				CString ss = _T("New section conflicts with existing section with name ") + pDoc->sections[conflict].name + _T(".");
				//ss << "New section conflicts with existing section with name " << pDoc->sections[conflict].name << ".";
				//MessageBox(CString(ss.str().c_str()), _T("Section Conflict"));
				MessageBox(ss, _T("Section Conflict"));
				continue;
			}

			pDoc->sections[selected_row] = newSection;

			refillSections(pDoc->sections);
			/*
			section_list.DeleteItem(selected_row);

			LVITEM lvItem;
			int nItem;

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = selected_row;
			lvItem.iSubItem = 0;
			lvItem.pszText = newSection.name.GetBuffer();
			nItem = section_list.InsertItem(&lvItem);

			if (newSection.usedRatings()) {
				std::wstringstream ss;
				ss << newSection.lower_rating_limit;
				section_list.SetItemText(nItem, 1, CString(ss.str().c_str()));
				ss.str("");
				ss << newSection.upper_rating_limit;
				section_list.SetItemText(nItem, 2, CString(ss.str().c_str()));
			}
			if (newSection.usedGrade()) {
				section_list.SetItemText(nItem, 3, getGradeString(newSection.lower_grade_limit).GetBuffer());
				section_list.SetItemText(nItem, 4, getGradeString(newSection.upper_grade_limit).GetBuffer());
			}

			section_list.SetItemText(nItem, 5, getSectionTypeString(newSection.sec_type));
			*/
			return;
		}
	}
}

std::wstring CStringToWString(const CString &cs) {
	LPCWSTR temp1 = cs.GetString();
	return std::wstring(temp1);
}

std::wstring intToString(unsigned v) {
	std::wstringstream ss;
	ss << v;
	return ss.str();
}

void updateUscfFromRatingFile(std::wstring &uscf_id, std::wstring &uscf_rating, const Player &p, std::wofstream &normal_log, bool &warning_condition) {
	std::wstring temp_rating = p.uscf_rating;

	if (uscf_id.size() != 0 && uscf_id != p.uscf_id) {
		warning_condition = true;
		normal_log << "WARNING: USCF ID in constant contact " << uscf_id << " not the same as the one in the ratings file " << p.uscf_id << std::endl;
	}
	if (uscf_rating.size() != 0 && uscf_rating != temp_rating) {
		//warning_condition = true;
		//normal_log << "WARNING: USCF rating in constant contact " << uscf_rating << " not the same as the one in the ratings file " << temp_rating << std::endl;
	}

	uscf_id = p.uscf_id;
	uscf_rating = temp_rating;
}

// Click on the button to load sections from constant contact file.
void CCCSwisssys2View::OnBnClickedButton6()
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

	std::wifstream infile(pDoc->ratings_file);
	Player p;
	std::map<std::wstring, unsigned> nwsrs_map;
	std::map<std::wstring, unsigned> uscf_map;
	std::vector<Player> rated_players;

	unsigned player_index = 0;

	while (!infile.eof()) {
		infile >> p;
		if (!infile.eof()) {
			rated_players.push_back(p);
			nwsrs_map.insert(std::pair<std::wstring, unsigned>(p.getFullId(), player_index));
			if (!p.uscf_id.empty()) {
				uscf_map.insert(std::pair<std::wstring, unsigned>(p.uscf_id, player_index));
			}
			++player_index;
		}
	}

	std::wifstream cc_file(pDoc->constant_contact_file);
	bool use_cc = true;
	if (!cc_file) {
		use_cc = false;
		//MessageBox(_T("Could not load constant contact registration file."), _T("Constant contact file incorrect."));
		//return;
	}
	cc_file.close();

	bool error_condition = false, warning_condition = false;
	unsigned i;

	if (use_cc) {
		std::wstring ccsstr = CStringToWString(pDoc->constant_contact_file);
		auto entries = load_constant_contact_file(ccsstr, allstars2016);

		unsigned num_parents = 0, num_players = 0;

		for (i = 0; i < entries.size(); ++i) {
			//        std::cout << entries[i].getFirstName() << " " << entries[i].getLastName() << " " << entries[i].didAdultCheck() << " school=" << entries[i].getSchool() << " grade=" << entries[i].getGrade() << " section=" << entries[i].getSection() << " nwsrsid=" << entries[i].getNwsrsId() << std::endl;
			if (entries[i].isParent()) {
				++num_parents;
			}
			if (entries[i].isPlayer()) {
				++num_players;
			}
		}

		//	std::cout << "There are " << entries.size() << " entries in the file." << std::endl;
		//	std::cout << "There are " << num_parents << " parents." << std::endl;
		//	std::cout << "There are " << num_players << " players." << std::endl;
		if (num_parents + num_players != entries.size()) {
			MessageBox(_T("Some problem where number of parents plus number of players not equal to number of entries."), _T("ERROR"));
			return;
		}

		for (i = 0; i < entries.size(); ++i) {
			if (entries[i].isPlayer()) {
				std::wstring last_name = entries[i].getLastName();
				std::wstring first_name = entries[i].getFirstName();
				std::wstring school = entries[i].getSchool();
				std::wstring full_id = entries[i].getNwsrsId();
				//std::wstring given_rating = entries[i].getNwsrsRating();
				std::wstring upper_last = last_name;
				std::wstring upper_first = first_name;
				std::wstring uscf_id = entries[i].getUscfId();
				//std::wstring uscf_rating = entries[i].getUscfRating();
				std::wstring uscf_rating = L"";

				upper_last = toUpper(upper_last);
				upper_first = toUpper(upper_first);
				full_id = toUpper(full_id);
				//given_rating = toUpper(given_rating);

				std::wstring last_four_id;
				if (full_id.length() >= 4) {
					last_four_id = full_id.substr(full_id.length() - 4);
				}

				std::wstring school_code;
				if (full_id.length() == 8) {
					school_code = full_id.substr(0, 3);
				}

				if (school.length() == 3) {
					school_code = toUpper(school);
					full_id.replace(0, 3, school_code); // Update school code in their ID.
				}
				school = L"";

				if (school == L"" && school_code != L"") {
					school = pDoc->school_codes.findName(school_code);
				}

				CString cs_full_id = CString(full_id.c_str());

				auto rentry = nwsrs_map.find(full_id);
				unsigned cc_rating = 4000;
				unsigned nwsrs_rating;
				wchar_t grade_code = entries[i].getGradeCode();

				if (rentry != nwsrs_map.end()) {
					cc_rating = rated_players[rentry->second].get_higher_rating();
					if (rated_players[rentry->second].uscf_id != L"") {
						updateUscfFromRatingFile(uscf_id, uscf_rating, rated_players[rentry->second], normal_log, warning_condition);
					}
					nwsrs_rating = rated_players[rentry->second].nwsrs_rating;
				}
				else {
					if (full_id == L"NONE" || full_id == L"N/A") {
#if 0
						if (given_rating != L"N/A" && given_rating != L"NONE") {
							// No ID but specified a rating.
							warning_condition = true;
							normal_log << "WARNING: rating was specified but not ID, will ignore specified rating for player " << last_name << " " << first_name << " rating = " << given_rating << " " << grade_code << " " << school << std::endl;
						}
#endif
						std::wstring school_code = pDoc->school_codes.findCodeFromSchool(school, normal_log);
						if (school_code == L"") {
							warning_condition = true;
							normal_log << "WARNING: a match for school name " << school << " was not found.  Using a generic school code instead.  Fix this during reporting process or in swisssys for player " << last_name << " " << first_name << " " << grade_code << " " << std::endl;
							school_code = L"XXX";
						}
						full_id = school_code + grade_code + L"XXXX";
						cc_rating = (grade_code - 'A') * 100;
						nwsrs_rating = cc_rating;
					}
					else {
						// They have an ID but the exact ID wasn't found in the list.  So, search by name
						// and partial ID.
						bool found = false;
						for (unsigned j = 0; j < rated_players.size(); ++j) {
							if (//rated_players[j].last_name == upper_last &&
								//rated_players[j].first_name == upper_first &&
								rated_players[j].id == last_four_id) {
								cc_rating = rated_players[j].get_higher_rating();
								if (rated_players[j].uscf_id != L"") {
									updateUscfFromRatingFile(uscf_id, uscf_rating, rated_players[j], normal_log, warning_condition);
								}
								nwsrs_rating = rated_players[j].nwsrs_rating;

								//normal_log << "Found by digit ID string from ratings file " << std::endl;
								if (rated_players[j].grade > grade_code) {
									grade_code = rated_players[j].grade;
								}
								if (school_code.length() == 3) {
									full_id = school_code + grade_code + last_four_id;
								}
								else {
									full_id = rated_players[j].school_code + rated_players[j].grade + last_four_id;
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

				int target_section = pDoc->sections.foundIn(cc_rating, grade_code);
				if (target_section == -1) {
					error_condition = true;
					normal_log << "ERROR: Doesn't belong in any section. " << full_id << " " << last_name << " " << first_name << " " << cc_rating << " " << grade_code << " " << school << std::endl;
				}
				else if (target_section < -1) {
					error_condition = true;
					normal_log << "ERROR: Player can go in multiple sections. " << full_id << " " << last_name << " " << first_name << " " << cc_rating << " " << grade_code << " " << school << std::endl;
				}
				else {
					pDoc->sections[target_section].players.push_back(SectionPlayerInfo(cs_last_name, cs_first_name, cs_full_id, nwsrs_rating, grade_code, cs_school, cs_school_code, CString(uscf_id.c_str()), CString(uscf_rating.c_str())));
					//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << full_id << " " << last_name << " " << first_name << " " << cc_rating << " " << grade_code << " " << school_code << " uscf rating and id " << uscf_rating << " " << uscf_id << std::endl;
				}
			}
		}
	}

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
			CString cs_last(this_player.ws_last.c_str());
			CString cs_first(this_player.ws_first.c_str());
			CString cs_id(this_player.ws_id.c_str());
			CString cs_school_name(this_player.ws_school_name.c_str());
			CString cs_school_code(this_player.ws_school_code.c_str());
			CString cs_uscf_id(this_player.ws_uscf_id.c_str());
			CString cs_uscf_rating(this_player.ws_uscf_rating.c_str());

			pDoc->sections[target_section].players.push_back(SectionPlayerInfo(cs_last, cs_first, cs_id, this_player.nwsrs_rating, this_player.grade, cs_school_name, cs_school_code, cs_uscf_id, cs_uscf_rating));
			//normal_log << "Went in section " << CStringToWString(pDoc->sections[target_section].name) << " " << this_player.ws_id << " " << this_player.ws_last << " " << this_player.ws_first << " " << this_player.nwsrs_rating << " " << this_player.grade << " " << this_player.ws_school_code << " uscf rating and id " << this_player.ws_uscf_rating << " " << this_player.ws_uscf_id << std::endl;
		}
	}

	normal_log.close();

	if (error_condition) {
		MessageBox(_T("Some players not added to a section.  Please read the log."), _T("Error"));
	}
	else if (warning_condition) {
		MessageBox(_T("Some potential issues were found.  Please read the log."), _T("Warning"));
	}
	else {
		MessageBox(_T("All players added to sections successfully."), _T("Success"));
	}

	if (error_condition || warning_condition) {
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


void CCCSwisssys2View::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CCCSwisssys2View::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState & LVIS_SELECTED) {
		int selectedItem = pNMLV->iItem;
		auto pDoc = GetDocument();
		section_players.DeleteAllItems();
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

			section_players.SetItemData(i, (DWORD_PTR)&pDoc->sections[selectedItem].players[i]);
		}
	}
	*pResult = 0;
}


void CCCSwisssys2View::OnBnClickedButton7()
{
	auto pDoc = GetDocument();
	std::wstring output_dir = pDoc->getOutputLocation(); // pDoc->CDocument->m_strPathName

	unsigned total_placed = 0;;
	for (unsigned j = 0; j < pDoc->sections.size(); ++j) {
		if (pDoc->sections[j].players.size() == 0) {
			continue;
		}
			
		std::wstring filename = output_dir + L"\\Section_" + CStringToWString(pDoc->sections[j].name) + L".txt";
		std::wofstream section_file(filename);

		for (unsigned i = 0; i < pDoc->sections[j].players.size(); ++i) {
			section_file << L"Name = " << toUpper(CStringToWString(pDoc->sections[j].players[i].last_name)) << L", " <<
				toUpper(CStringToWString(pDoc->sections[j].players[i].first_name)) << std::endl;
			section_file << "Rating = " << pDoc->sections[j].players[i].rating << std::endl;
			section_file << "ID# = " << CStringToWString(pDoc->sections[j].players[i].full_id) << std::endl;
			section_file << "Team = " << CStringToWString(pDoc->sections[j].players[i].school_code) << std::endl;
			if (!pDoc->sections[j].players[i].uscf_id.IsEmpty()) {
				section_file << "ID2 = " << CStringToWString(pDoc->sections[j].players[i].uscf_id) << std::endl;
			}
			if (!pDoc->sections[j].players[i].uscf_rating.IsEmpty()) {
				section_file << "RTNG2 = " << CStringToWString(pDoc->sections[j].players[i].uscf_rating) << std::endl;
			}
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


void CCCSwisssys2View::OnBnClickedButton8()
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
}


void CCCSwisssys2View::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	int smark = section_list.GetSelectionMark();
	if (smark < 0) {
		return;
	}

	auto pDoc = GetDocument();
	int slen = pDoc->sections.size();
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
}

template <class T>
int listCompareItem(T &a, T &b) {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

int listCompareItemInt(int a, int b) {
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

int CALLBACK section_players_compare(LPARAM l1, LPARAM l2, LPARAM lsort) {
	SectionPlayerInfo *p1 = (SectionPlayerInfo*)l1;
	SectionPlayerInfo *p2 = (SectionPlayerInfo*)l2;
	switch (lsort) {
	case 0:
		return listCompareItem(p1->last_name, p2->last_name);
	case 1:
		return listCompareItem(p1->first_name, p2->first_name);
	case 2:
		return listCompareItem(p1->rating, p2->rating);
	case 3:
		return listCompareItem(p1->full_id, p2->full_id);
	case 4:
		return listCompareItem(p1->grade, p2->grade);
	case 5:
		return listCompareItem(p1->school, p2->school);
	case 6:
		return listCompareItemInt(std::stoi(CStringToWString(p1->uscf_rating)), std::stoi(CStringToWString(p2->uscf_rating)));
	case 7:
		return listCompareItem(p1->uscf_id, p2->uscf_id);
	default:
		return 0;
	}
}

void CCCSwisssys2View::OnLvnColumnclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	section_players.SortItems(section_players_compare, pNMLV->iSubItem);
}


void CCCSwisssys2View::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	ManageRegistrations mr_dialog(GetDocument());

	mr_dialog.DoModal();
}
