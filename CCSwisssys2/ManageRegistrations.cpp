// ManageRegistrations.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "ManageRegistrations.h"
#include "afxdialogex.h"


// ManageRegistrations dialog

IMPLEMENT_DYNAMIC(ManageRegistrations, CDialogEx)

ManageRegistrations::ManageRegistrations(CCCSwisssys2Doc *doc, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MANAGE_REGISTRATIONS, pParent)
{
	pDoc = doc;
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
	ON_BN_CLICKED(IDC_BUTTON1, &ManageRegistrations::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ManageRegistrations::OnBnClickedButton2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &ManageRegistrations::OnNMDblclkList3)
	ON_CBN_SELCHANGE(IDC_COMBO2, &ManageRegistrations::OnCbnSelchangeCombo2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &ManageRegistrations::OnNMDblclkList2)
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

void addToList(CListCtrl &the_list, 
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

void ManageRegistrations::OnAnyChange()
{
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
	if (s_last.length() == 0) return;

	wchar_t s = toupper(s_last[0]);
	start = ending_index[s - 'A'] + 1;
	end   = ending_index[s - 'A' + 1];

	int entry = 0;

	int i;
	for (i = start; i <= end; ++i) {
		if (insensitiveCompare(rated_players[i].ws_last, s_last) &&
			findStringIC(rated_players[i].ws_first, s_first) &&
			findStringIC(rated_players[i].ws_id, s_id) &&
			findStringIC(rated_players[i].ws_school_code, s_school_code)) {

			if (grade_sel > 0) {
				wchar_t sel_grade = 'A' + grade_sel - 1;
				if (rated_players[i].grade != sel_grade) {
					continue;
				}
			}

			if (!code_set.empty()) {
				if (code_set.find(rated_players[i].ws_school_code) == code_set.end()) {
					continue;
				}
			}

			addToList(PossiblePlayers, 
				entry++,
				rated_players[i].ws_last,
				rated_players[i].ws_first,
				rated_players[i].nwsrs_rating,
				rated_players[i].ws_id,
				rated_players[i].grade,
				rated_players[i].ws_school_code,
				rated_players[i].ws_school_name,
				rated_players[i].ws_uscf_rating,
				rated_players[i].ws_uscf_id,
				i);
		}
	}
}

void ManageRegistrations::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnAnyChange();
}


void ManageRegistrations::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnAnyChange();
}


void ManageRegistrations::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}


void ManageRegistrations::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
}


void ManageRegistrations::OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	std::wstringstream ss;
//	ss << "OnNMBlkclkList3 subItem = " << pNMItemActivate->iSubItem;
//	MessageBox(CString(ss.str().c_str()), _T("Information"));
	unsigned pindex = (unsigned)PossiblePlayers.GetItemData(pNMItemActivate->iItem);
	unsigned entry = pDoc->mrplayers.size();

	pDoc->mrplayers.push_back(rated_players[pindex]);

	addToList(RegisteredPlayers,
		entry,
		rated_players[pindex].ws_last,
		rated_players[pindex].ws_first,
		rated_players[pindex].nwsrs_rating,
		rated_players[pindex].ws_id,
		rated_players[pindex].grade,
		rated_players[pindex].ws_school_code,
		rated_players[pindex].ws_school_name,
		rated_players[pindex].ws_uscf_rating,
		rated_players[pindex].ws_uscf_id,
		entry);

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
	// TODO: Add your control notification handler code here
	OnAnyChange();
}


BOOL ManageRegistrations::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

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

	RegisteredPlayers.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	PossiblePlayers.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (unsigned index = 0; index < 27; ++index) {
		ending_index[index] = -1;
	}

	SetupGradeCombobox(grade_combo_box);

	std::wifstream infile(pDoc->ratings_file);
	Player p;

	int player_index = 0;
	wchar_t cur = 'A' - 1;

	if (!infile) {
		MessageBox(_T("Ratings and school code file must be loaded first."), _T("Error"));
		EndDialog(0);
		return TRUE;
	}

	if (pDoc->school_codes.empty()) {
		MessageBox(_T("Ratings and school code file must be loaded first."), _T("Error"));
		EndDialog(0);
		return TRUE;
	}

	while (!infile.eof()) {
		infile >> p;
		if (!infile.eof()) {
			std::wstring ws_last = p.last_name;
			std::wstring ws_first = p.first_name;
			std::wstring ws_id = p.id;
			std::wstring ws_school_code = p.school_code;
			std::wstring ws_school_name = pDoc->school_codes.findName(ws_school_code);
			std::wstring ws_uscf_id = p.uscf_id;

			rated_players.push_back(MRPlayer(ws_last,ws_first,ws_id,ws_school_code,ws_school_name,ws_uscf_id,p.nwsrs_rating,p.uscf_rating,p.grade));

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

	unsigned mrindex;

	for (mrindex = 0; mrindex < pDoc->mrplayers.size(); ++mrindex) {
		addToList(RegisteredPlayers,
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
			mrindex);
	}

	LastNameEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ManageRegistrations::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
//	unsigned pindex = (unsigned)RegisteredPlayers.GetItemData(pNMItemActivate->iItem);

	RegisteredPlayers.DeleteItem(pNMItemActivate->iItem);
	pDoc->mrplayers.erase(pDoc->mrplayers.begin() + pNMItemActivate->iItem);

	*pResult = 0;
}
