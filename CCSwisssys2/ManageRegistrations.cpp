// ManageRegistrations.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "ManageRegistrations.h"
#include "afxdialogex.h"
#include "SchoolSelector.h"
#include "ForceSection.h"


// ManageRegistrations dialog

IMPLEMENT_DYNAMIC(ManageRegistrations, CDialogEx)

ManageRegistrations::ManageRegistrations(CCCSwisssys2Doc *doc, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MANAGE_REGISTRATIONS, pParent)
{
	pDoc = doc;
	inProcessingChange = false;

	std::wofstream normal_log;
	log_messages lm;
	bool error_condition = false, warning_condition = false, info_condition = false;
	std::vector<ConstantContactEntry> entries;
	std::map<std::wstring, unsigned> adult_map;
	post_proc = process_cc_file(this->GetSafeHwnd(), pDoc, entries, adult_map, error_condition, warning_condition, info_condition, normal_log, lm);
}

ManageRegistrations::~ManageRegistrations()
{
}

void ManageRegistrations::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, LastNameEdit);
	DDX_Control(pDX, IDC_EDIT2, FirstNameEdit);
	DDX_Control(pDX, IDC_EDIT3, NWSRS_ID_Edit);
	DDX_Control(pDX, IDC_LIST2, RegisteredPlayers);
	DDX_Control(pDX, IDC_LIST3, PossiblePlayers);
	DDX_Control(pDX, IDC_EDIT4, school_code_edit);
	DDX_Control(pDX, IDC_EDIT5, school_name_edit);
	DDX_Control(pDX, IDC_COMBO2, grade_combo_box);
}


BEGIN_MESSAGE_MAP(ManageRegistrations, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &ManageRegistrations::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &ManageRegistrations::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &ManageRegistrations::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &ManageRegistrations::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &ManageRegistrations::OnEnChangeEdit5)
	ON_BN_CLICKED(IDC_BUTTON1, &ManageRegistrations::OnAddNewPlayer)
	ON_BN_CLICKED(IDC_BUTTON2, &ManageRegistrations::OnConstantContactFileBrowse)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &ManageRegistrations::OnNMDblclkList3)
	ON_CBN_SELCHANGE(IDC_COMBO2, &ManageRegistrations::OnCbnSelchangeCombo2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &ManageRegistrations::OnNMDblclkList2)
	ON_BN_CLICKED(IDC_FORCE_SECTION, &ManageRegistrations::OnForceSection)
	ON_BN_CLICKED(IDC_AUTO_SECTION, &ManageRegistrations::OnBnClickedAutoSection)
	ON_BN_CLICKED(IDC_WITHDRAWAL, &ManageRegistrations::OnBnClickedWithdrawal)
	ON_BN_CLICKED(IDC_REENTER, &ManageRegistrations::OnBnClickedReenter)
END_MESSAGE_MAP()

bool findStringIC(const std::wstring & strHaystack, const std::wstring & strNeedle)
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != strHaystack.end());
}

bool insensitiveCompare(const std::wstring &full, const std::wstring &part) {
	if (part.length() > full.length()) return false;

	unsigned i;
	for (i = 0; i < part.length(); ++i) {
		if (toupper(part[i]) != toupper(full[i])) {
			return false;
		}
	}
	return true;
}

// ManageRegistrations message handlers

void addToRegisteredList(CListCtrl &the_list, 
	int entry,
	std::wstring &ws_last, 
	std::wstring &ws_first, 
	int nwsrs_rating, 
	std::wstring &ws_id, 
	wchar_t grade,
	std::wstring &ws_school_code,
	std::wstring &ws_school_name,
	std::wstring &ws_uscf_rating,
	std::wstring &ws_uscf_id,
	std::wstring &ws_forced_section,
	std::wstring &ws_withdrawn,
	int itemData) {

	LVITEM lvItem;
	int nItem;

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = entry;
	lvItem.iSubItem = 0;
	lvItem.pszText = &ws_last[0];
	nItem = the_list.InsertItem(&lvItem);

	the_list.SetItemText(nItem, 1, &ws_first[0]);
	std::wstringstream ss;
	ss << nwsrs_rating;
	the_list.SetItemText(nItem, 2, CString(ss.str().c_str()));
	ss.str(L"");

	the_list.SetItemText(nItem, 3, &ws_id[0]);
	the_list.SetItemText(nItem, 4, getGradeString(grade));
	the_list.SetItemText(nItem, 5, &ws_school_code[0]);
	the_list.SetItemText(nItem, 5, &ws_school_code[0]);
	the_list.SetItemText(nItem, 6, &ws_school_name[0]);
	the_list.SetItemText(nItem, 7, &ws_uscf_rating[0]);
	the_list.SetItemText(nItem, 8, &ws_uscf_id[0]);
	the_list.SetItemText(nItem, 9, &ws_forced_section[0]);
	the_list.SetItemText(nItem, 10, &ws_withdrawn[0]);

	the_list.SetItemData(nItem, (DWORD_PTR)itemData);
}

void addToPossibleList(CListCtrl &the_list,
	int entry,
	std::wstring &ws_last,
	std::wstring &ws_first,
	int nwsrs_rating,
	std::wstring &ws_id,
	wchar_t grade,
	std::wstring &ws_school_code,
	std::wstring &ws_school_name,
	std::wstring &ws_uscf_rating,
	std::wstring &ws_uscf_id,
	int itemData) {

	LVITEM lvItem;
	int nItem;

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = entry;
	lvItem.iSubItem = 0;
	lvItem.pszText = &ws_last[0];
	nItem = the_list.InsertItem(&lvItem);

	the_list.SetItemText(nItem, 1, &ws_first[0]);
	std::wstringstream ss;
	ss << nwsrs_rating;
	the_list.SetItemText(nItem, 2, CString(ss.str().c_str()));
	ss.str(L"");

	the_list.SetItemText(nItem, 3, &ws_id[0]);
	the_list.SetItemText(nItem, 4, getGradeString(grade));
	the_list.SetItemText(nItem, 5, &ws_school_code[0]);
	the_list.SetItemText(nItem, 5, &ws_school_code[0]);
	the_list.SetItemText(nItem, 6, &ws_school_name[0]);
	the_list.SetItemText(nItem, 7, &ws_uscf_rating[0]);
	the_list.SetItemText(nItem, 8, &ws_uscf_id[0]);

	the_list.SetItemData(nItem, (DWORD_PTR)itemData);
}

void ManageRegistrations::updateRegistered() {
	RegisteredPlayers.SetSelectionMark(-1);
	RegisteredPlayers.DeleteAllItems();

	unsigned mrindex;

	for (mrindex = 0; mrindex < pDoc->mrplayers.size(); ++mrindex) {
		auto unique_name = pDoc->mrplayers[mrindex].getUnique();
		std::wstring forced_section = L"";
		auto forced_iter = pDoc->force_sections.find(unique_name);
		if (forced_iter != pDoc->force_sections.end()) {
			forced_section = forced_iter->second;
		}
		addToRegisteredList(RegisteredPlayers,
			mrindex,
			pDoc->mrplayers[mrindex].ws_last,
			pDoc->mrplayers[mrindex].ws_first,
			pDoc->mrplayers[mrindex].nwsrs_rating,
			pDoc->mrplayers[mrindex].ws_id,
			pDoc->mrplayers[mrindex].grade,
			pDoc->mrplayers[mrindex].ws_school_code,
			pDoc->mrplayers[mrindex].ws_school_name,
			pDoc->mrplayers[mrindex].ws_uscf_rating,
			pDoc->mrplayers[mrindex].ws_uscf_id,
			forced_section,
			std::wstring(L""),
			(-mrindex - 1));
	}

	unsigned ppindex;
	for (ppindex = 0; ppindex < post_proc.size(); ++ppindex) {
		auto &ppiter = post_proc[ppindex];
		auto unique_name = ppiter.getUnique();
		std::wstring forced_section = L"";
		auto forced_iter = pDoc->force_sections.find(unique_name);
		if (forced_iter != pDoc->force_sections.end()) {
			forced_section = forced_iter->second;
		}
		std::wstring withdrawn = L"";
		auto with_iter = pDoc->noshows.find(unique_name);
		if (with_iter != pDoc->noshows.end()) {
			withdrawn = L"Yes";
		}
		addToRegisteredList(RegisteredPlayers,
			mrindex++,
			CStringToWString(ppiter.last_name),
			CStringToWString(ppiter.first_name),
			ppiter.rating,
			CStringToWString(ppiter.full_id),
			ppiter.grade,
			CStringToWString(ppiter.school_code),
			CStringToWString(ppiter.school),
			CStringToWString(ppiter.uscf_rating),
			CStringToWString(ppiter.uscf_id),
			forced_section,
			withdrawn,
			ppindex);
	}
}

void ManageRegistrations::OnAnyChange()
{
	if (inProcessingChange) {
		return;
	}
	else {
		inProcessingChange = true;
	}

	PossiblePlayers.SetSelectionMark(-1);
	PossiblePlayers.DeleteAllItems();

	CString cs_last, cs_first, cs_id, cs_school_code, cs_school_name;

	LastNameEdit.GetWindowText(cs_last);
	FirstNameEdit.GetWindowText(cs_first);
	NWSRS_ID_Edit.GetWindowText(cs_id);
	school_code_edit.GetWindowText(cs_school_code);
	school_name_edit.GetWindowText(cs_school_name);
	int grade_sel = grade_combo_box.GetCurSel();

	std::wstring s_last, s_first, s_id, s_school_code, s_school_name;
	s_last = CStringToWString(cs_last);
	s_first = CStringToWString(cs_first);
	s_id = CStringToWString(cs_id);
	s_school_code = CStringToWString(cs_school_code);
	s_school_name = CStringToWString(cs_school_name);

	std::set<std::wstring> code_set;
	if (!s_school_name.empty()) {
		code_set = pDoc->school_codes.getPotentialSet(s_school_name);
		if (code_set.size() == 1) {
			auto first = code_set.begin();
			CString cs_code(first->c_str());
			school_code_edit.SetWindowTextW(cs_code);
		}
		else if (code_set.size() == 0) {
			cs_school_code = L"";
			school_code_edit.SetWindowTextW(cs_school_code);
		}
	}

	int start, end;
	if (s_last.length() == 0) {
		inProcessingChange = false;
		return;
	}

	wchar_t s = toupper(s_last[0]);
	start = ending_index[s - 'A'] + 1;
	end   = ending_index[s - 'A' + 1];

	int entry = 0;

	int i;
	for (i = start; i <= end; ++i) {
		if (insensitiveCompare(pDoc->rated_players[i].last_name, s_last) &&
			findStringIC(pDoc->rated_players[i].first_name, s_first) &&
			findStringIC(pDoc->rated_players[i].getFullId(), s_id) &&
			findStringIC(pDoc->rated_players[i].school_code, s_school_code)) {

			if (grade_sel > 0) {
				wchar_t sel_grade = 'A' + grade_sel - 1;
				if (pDoc->rated_players[i].grade != sel_grade) {
					continue;
				}
			}

			if (!code_set.empty()) {
				if (code_set.find(pDoc->rated_players[i].school_code) == code_set.end()) {
					continue;
				}
			}

			addToPossibleList(PossiblePlayers, 
				entry++,
				pDoc->rated_players[i].last_name,
				pDoc->rated_players[i].first_name,
				pDoc->rated_players[i].nwsrs_rating,
				pDoc->rated_players[i].getFullId(),
				pDoc->rated_players[i].grade,
				pDoc->rated_players[i].school_code,
				pDoc->school_codes.findName(pDoc->rated_players[i].school_code),
				pDoc->rated_players[i].uscf_rating,
				pDoc->rated_players[i].uscf_id,
				i);
		}
	}

	inProcessingChange = false;
}

void ManageRegistrations::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	OnAnyChange();
}


void ManageRegistrations::OnAddNewPlayer()
{
	// TODO: Add your control notification handler code here
	CString cs_last, cs_first, cs_id, cs_school_code, cs_school_name;

	LastNameEdit.GetWindowText(cs_last);
	FirstNameEdit.GetWindowText(cs_first);
	NWSRS_ID_Edit.GetWindowText(cs_id);
	school_code_edit.GetWindowText(cs_school_code);
	school_name_edit.GetWindowText(cs_school_name);
	int grade_sel = grade_combo_box.GetCurSel();

	std::wstring s_last, s_first, s_id, s_school_code, s_school_name;
	s_last = toUpper(CStringToWString(cs_last));
	s_first = toUpper(CStringToWString(cs_first));
	s_id = CStringToWString(cs_id);
	s_school_code = toUpper(CStringToWString(cs_school_code));
	s_school_name = CStringToWString(cs_school_name);

	std::set<std::wstring> code_set;
	if (!s_school_name.empty()) {
		code_set = pDoc->school_codes.getPotentialSet(s_school_name);
		if (code_set.size() == 1) {
			auto first = code_set.begin();
			CString cs_code(first->c_str());
			school_code_edit.SetWindowTextW(cs_code);
		}
		else if (code_set.size() == 0) {
			cs_school_code = L"";
			school_code_edit.SetWindowTextW(cs_school_code);
		}
	}

	wchar_t sel_grade = 'A' + grade_sel - 1;
	unsigned init_rating = (grade_sel - 1) * 100;

	int start, end;
	bool found_possibility = false;
	if (s_last.length() != 0) {
		wchar_t s = toupper(s_last[0]);
		start = ending_index[s - 'A'] + 1;
		end = ending_index[s - 'A' + 1];

		int entry = 0;

		int i;
		for (i = start; i <= end; ++i) {
			if (insensitiveCompare(pDoc->rated_players[i].last_name, s_last) &&
				findStringIC(pDoc->rated_players[i].first_name, s_first) &&
				findStringIC(pDoc->rated_players[i].getFullId(), s_id) &&
				findStringIC(pDoc->rated_players[i].school_code, s_school_code)) {

				if (grade_sel > 0) {
					if (pDoc->rated_players[i].grade != sel_grade) {
						continue;
					}
				}

				if (!code_set.empty()) {
					if (code_set.find(pDoc->rated_players[i].school_code) == code_set.end()) {
						continue;
					}
				}

				found_possibility = true;
			}
		}
	}

	if (found_possibility) {
		MessageBox(_T("Possible matches with existing players.  Please double-click one of them or continue entering complete information for new player."), _T("Error"));
		return;
	}

	if (cs_last.IsEmpty()) {
		MessageBox(_T("Please enter a last name and try again."), _T("Error"));
		return;
	}
	if (cs_first.IsEmpty()) {
		MessageBox(_T("Please enter a first name and try again."), _T("Error"));
		return;
	}
	if (cs_school_code.IsEmpty()) {
		MessageBox(_T("Please enter a school code and try again."), _T("Error"));
		return;
	}
	if (grade_sel <= 0) {
		MessageBox(_T("Please enter the new player's grade and try again."), _T("Error"));
		return;
	}

	unsigned entry = (unsigned)pDoc->mrplayers.size();
	std::wstring new_id = s_school_code + sel_grade;

	pDoc->mrplayers.push_back(MRPlayer(s_last, s_first, new_id, s_school_code, pDoc->school_codes.findName(s_school_code), std::wstring(), init_rating, std::wstring(), std::wstring(), sel_grade));
	unsigned pindex = (unsigned)pDoc->mrplayers.size() - 1;

	addToRegisteredList(RegisteredPlayers,
		entry,
		pDoc->mrplayers[pindex].ws_last,
		pDoc->mrplayers[pindex].ws_first,
		pDoc->mrplayers[pindex].nwsrs_rating,
		pDoc->mrplayers[pindex].ws_id,
		pDoc->mrplayers[pindex].grade,
		pDoc->mrplayers[pindex].ws_school_code,
		pDoc->school_codes.findName(pDoc->mrplayers[pindex].ws_school_code),
//		pDoc->school_codes.findName(pDoc->rated_players[pindex].school_code),
		pDoc->mrplayers[pindex].ws_uscf_rating,
		pDoc->mrplayers[pindex].ws_uscf_id,
		std::wstring(L""),
		std::wstring(L""),
		(-entry - 1));

	entry++;

	LastNameEdit.SetWindowText(L"");
	FirstNameEdit.SetWindowText(L"");
	NWSRS_ID_Edit.SetWindowText(L"");
	school_code_edit.SetWindowText(L"");
	school_name_edit.SetWindowText(L"");
	grade_combo_box.SetCurSel(-1);

	LastNameEdit.SetFocus();
}


void ManageRegistrations::OnConstantContactFileBrowse()
{
	CString cs_school_code, cs_school_name;
	school_code_edit.GetWindowText(cs_school_code);
	school_name_edit.GetWindowText(cs_school_name);

	std::wstring s_school_code, s_school_name;
	s_school_code = CStringToWString(cs_school_code);
	s_school_name = CStringToWString(cs_school_name);

	SchoolSelector ss_dialog(pDoc, s_school_name, s_school_code, s_school_code, L"", L"", L"");

	if (ss_dialog.DoModal() == IDOK) {
		s_school_name = pDoc->school_codes.findName(s_school_code);

		cs_school_code = WStringToCString(s_school_code);
		cs_school_name = WStringToCString(s_school_name);
		school_code_edit.SetWindowTextW(cs_school_code);
		school_name_edit.SetWindowTextW(cs_school_name);
	}
}


void ManageRegistrations::OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	std::wstringstream ss;
//	ss << "OnNMBlkclkList3 subItem = " << pNMItemActivate->iSubItem;
//	MessageBox(CString(ss.str().c_str()), _T("Information"));
	unsigned pindex = (unsigned)PossiblePlayers.GetItemData(pNMItemActivate->iItem);
	unsigned entry = (unsigned)pDoc->mrplayers.size();

	pDoc->mrplayers.push_back(MRPlayer(
		pDoc->rated_players[pindex].last_name,
		pDoc->rated_players[pindex].first_name,
		pDoc->rated_players[pindex].getFullId(),
		pDoc->rated_players[pindex].school_code,
		pDoc->school_codes.findName(pDoc->rated_players[pindex].school_code),
		pDoc->rated_players[pindex].uscf_id,
		pDoc->rated_players[pindex].nwsrs_rating,
		pDoc->rated_players[pindex].uscf_rating,
		pDoc->rated_players[pindex].uscf_exp_date,
		pDoc->rated_players[pindex].grade));

	addToRegisteredList(RegisteredPlayers,
		entry,
		pDoc->rated_players[pindex].last_name,
		pDoc->rated_players[pindex].first_name,
		pDoc->rated_players[pindex].nwsrs_rating,
		pDoc->rated_players[pindex].getFullId(),
		pDoc->rated_players[pindex].grade,
		pDoc->rated_players[pindex].school_code,
		pDoc->school_codes.findName(pDoc->rated_players[pindex].school_code),
		pDoc->rated_players[pindex].uscf_rating,
		pDoc->rated_players[pindex].uscf_id,
		std::wstring(L""),
		std::wstring(L""),
		(-entry - 1));

	entry++;

	LastNameEdit.SetWindowText(L"");
	FirstNameEdit.SetWindowText(L"");
	NWSRS_ID_Edit.SetWindowText(L"");
	school_code_edit.SetWindowText(L"");
	school_name_edit.SetWindowText(L"");
	grade_combo_box.SetCurSel(-1);

	LastNameEdit.SetFocus();

	*pResult = 0;
}


void ManageRegistrations::OnCbnSelchangeCombo2()
{
	OnAnyChange();
}


BOOL ManageRegistrations::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LVCOLUMN lvColumn;
	int nCol;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("Last Name");
	nCol = RegisteredPlayers.InsertColumn(0, &lvColumn);
	nCol = PossiblePlayers.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("First Name");
	RegisteredPlayers.InsertColumn(1, &lvColumn);
	PossiblePlayers.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 70;
	lvColumn.pszText = _T("NWSRS Rating");
	RegisteredPlayers.InsertColumn(2, &lvColumn);
	PossiblePlayers.InsertColumn(2, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("NWSRS ID");
	RegisteredPlayers.InsertColumn(3, &lvColumn);
	PossiblePlayers.InsertColumn(3, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("Grade");
	RegisteredPlayers.InsertColumn(4, &lvColumn);
	PossiblePlayers.InsertColumn(4, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("School");
	RegisteredPlayers.InsertColumn(5, &lvColumn);
	PossiblePlayers.InsertColumn(5, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("School Name");
	RegisteredPlayers.InsertColumn(6, &lvColumn);
	PossiblePlayers.InsertColumn(6, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 70;
	lvColumn.pszText = _T("USCF Rating");
	RegisteredPlayers.InsertColumn(7, &lvColumn);
	PossiblePlayers.InsertColumn(7, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("USCF ID");
	RegisteredPlayers.InsertColumn(8, &lvColumn);
	PossiblePlayers.InsertColumn(8, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("Forced");
	RegisteredPlayers.InsertColumn(9, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("Withdrawn");
	RegisteredPlayers.InsertColumn(10, &lvColumn);

	RegisteredPlayers.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	PossiblePlayers.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (unsigned index = 0; index < 27; ++index) {
		ending_index[index] = -1;
	}

	SetupGradeCombobox(grade_combo_box);

	if (pDoc->school_codes.empty()) {
		MessageBox(_T("Ratings and school code file must be loaded first."), _T("Error"));
		EndDialog(0);
		return TRUE;
	}

	wchar_t cur = 'A' - 1;

#if 0
	std::wifstream infile(pDoc->ratings_file);
	Player p;

	int player_index = 0;

	if (!infile) {
		MessageBox(_T("Ratings and school code file must be loaded first."), _T("Error"));
		EndDialog(0);
		return TRUE;
	}

	while (!infile.eof()) {
		infile >> p;
		if (!infile.eof()) {
			std::wstring ws_last = p.last_name;
			std::wstring ws_first = p.first_name;
			std::wstring ws_id = p.school_code + p.grade + p.id;
			std::wstring ws_school_code = p.school_code;
			std::wstring ws_school_name = pDoc->school_codes.findName(ws_school_code);
			std::wstring ws_uscf_id = p.uscf_id;

			rated_players.push_back(MRPlayer(ws_last, ws_first, ws_id, ws_school_code, ws_school_name, ws_uscf_id, p.nwsrs_rating, p.uscf_rating, p.grade));

			wchar_t start = toupper(ws_last[0]);

			while (cur < start) {
				ending_index[cur - 'A' + 1] = player_index - 1;
				cur += 1;
			}

			++player_index;
		}
	}
	while (cur <= 'Z') {
		ending_index[cur - 'A' + 1] = player_index - 1;
		cur += 1;
	}
#else
	unsigned player_index = 0;
	for(; player_index < pDoc->rated_players.size(); ++player_index) {
		wchar_t start = toupper(pDoc->rated_players[player_index].last_name[0]);

		while (cur < start) {
			ending_index[cur - 'A' + 1] = player_index - 1;
			cur += 1;
		}
	}
	while (cur <= 'Z') {
		ending_index[cur - 'A' + 1] = player_index - 1;
		cur += 1;
	}
#endif

	updateRegistered();

	LastNameEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ManageRegistrations::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	auto removed_item = pNMItemActivate->iItem;
	int removed_index = (int)RegisteredPlayers.GetItemData(removed_item);
	RegisteredPlayers.DeleteItem(removed_item);
	if (removed_index >= 0) {
		pDoc->mrplayers.erase(pDoc->mrplayers.begin() + pNMItemActivate->iItem);
	}
	else {
		int real_index = (-removed_index) - 1;
		pDoc->noshows.insert(real_index);
	}
	*/

	*pResult = 0;
}


void ManageRegistrations::OnForceSection()
{
	int selected_row = RegisteredPlayers.GetSelectionMark();
	if (selected_row >= 0) {
		int force_index = (int)RegisteredPlayers.GetItemData(selected_row);
		int selection = -1;
		ForceSection fs_dialog(pDoc, &selection);
		if (fs_dialog.DoModal() == IDOK) {
			if (force_index >= 0) {
				pDoc->force_sections.insert(std::pair<std::wstring, std::wstring>(post_proc[force_index].getUnique(), CStringToWString(pDoc->sections[selection].name)));
			}
			else {
				int mrindex = -(force_index + 1);
				pDoc->force_sections.insert(std::pair<std::wstring, std::wstring>(pDoc->mrplayers[mrindex].getUnique(), CStringToWString(pDoc->sections[selection].name)));
			}
			pDoc->SetModifiedFlag();
			updateRegistered();
		}
	}
}


void ManageRegistrations::OnBnClickedAutoSection()
{
	int selected_row = RegisteredPlayers.GetSelectionMark();
	if (selected_row >= 0) {
		int force_index = (int)RegisteredPlayers.GetItemData(selected_row);
		if (force_index >= 0) {
			if (pDoc->force_sections.erase(post_proc[force_index].getUnique()) > 0) {
				pDoc->SetModifiedFlag();
				updateRegistered();
			}
		}
		else {
			int mrindex = -(force_index + 1);
			if (pDoc->force_sections.erase(pDoc->mrplayers[mrindex].getUnique()) > 0) {
				pDoc->SetModifiedFlag();
				updateRegistered();
			}
		}
	}
}


void ManageRegistrations::OnBnClickedWithdrawal()
{
	int selected_row = RegisteredPlayers.GetSelectionMark();
	if (selected_row >= 0) {
		int removed_index = (int)RegisteredPlayers.GetItemData(selected_row);
		if (removed_index >= 0) {
			pDoc->noshows.insert(post_proc[removed_index].getUnique());
		}
		else {
			int real_index = (-removed_index) - 1;
			pDoc->mrplayers.erase(pDoc->mrplayers.begin() + real_index);
		}
		pDoc->SetModifiedFlag();
		updateRegistered();
	}
}


void ManageRegistrations::OnBnClickedReenter()
{
	int selected_row = RegisteredPlayers.GetSelectionMark();
	if (selected_row >= 0) {
		int removed_index = (int)RegisteredPlayers.GetItemData(selected_row);
		if (removed_index >= 0) {
			if (pDoc->noshows.erase(post_proc[removed_index].getUnique()) > 0) {
				pDoc->SetModifiedFlag();
				updateRegistered();
			}
		}
	}
}
