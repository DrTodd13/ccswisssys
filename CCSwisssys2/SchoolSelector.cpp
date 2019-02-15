// SchoolSelector.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "SchoolSelector.h"
#include "afxdialogex.h"
#include "helper.h"


// SchoolSelector dialog

IMPLEMENT_DYNAMIC(SchoolSelector, CDialogEx)

SchoolSelector::SchoolSelector(CCCSwisssys2Doc *doc, const std::wstring &school, const std::wstring &in_code, std::wstring &out_code, const std::wstring &name, const std::wstring &grade, const std::wstring &key, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCHOOL_EDITOR, pParent), m_out_code(out_code)
{
	pDoc = doc;
	m_school = school;
	m_possible = in_code;
	m_name = name;
	m_grade = grade;
	last_sort = 0;
	m_unique_key = key;
}

SchoolSelector::~SchoolSelector()
{
}

void SchoolSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, cedit_input_school);
	DDX_Control(pDX, IDC_EDIT5, cedit_possible);
	DDX_Control(pDX, IDC_EDIT2, cedit_code);
	DDX_Control(pDX, IDC_EDIT3, cedit_name);
	DDX_Control(pDX, IDC_EDIT4, cedit_city);
	DDX_Control(pDX, IDC_SE_NAME, cedit_player_name);
	DDX_Control(pDX, IDC_SE_GRADE, cedit_player_grade);
	DDX_Control(pDX, IDC_COMBO2, ccombo_type);
	DDX_Control(pDX, IDC_COMBO1, ccombo_state);
	DDX_Control(pDX, IDC_LIST1, clistctrl_allcodes);
}


BEGIN_MESSAGE_MAP(SchoolSelector, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &SchoolSelector::OnDoubleClickSchoolList)
	ON_BN_CLICKED(IDOK, &SchoolSelector::OnBnClickedOk)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &SchoolSelector::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// SchoolSelector message handlers

void addToList(CListCtrl &the_list,
	int entry,
	std::wstring &code,
	std::wstring &name,
	std::wstring &type,
	std::wstring &city,
	std::wstring &state,
	int itemData) {

	LVITEM lvItem;
	int nItem;

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = entry;
	lvItem.iSubItem = 0;
	lvItem.pszText = &code[0];
	nItem = the_list.InsertItem(&lvItem);

	the_list.SetItemText(nItem, 1, &name[0]);
	the_list.SetItemText(nItem, 2, &type[0]);
	the_list.SetItemText(nItem, 3, &city[0]);
	the_list.SetItemText(nItem, 4, &state[0]);
	the_list.SetItemData(nItem, (DWORD_PTR)itemData);
}

void SetupStateCombobox(CComboBox &box) {
	box.AddString(_T("OR"));
	box.AddString(_T("WA"));
	box.AddString(_T("ID"));
	box.AddString(_T("BC"));
	box.SetCurSel(0);
}

void SetupTypeCombobox(CComboBox &box) {
	box.AddString(_T("Elementary"));
	box.AddString(_T("Middle"));
	box.AddString(_T("High"));
	box.AddString(_T("Elementary/Middle"));
	box.AddString(_T("Middle/High"));
	box.AddString(_T("Elementary/Middle/High"));
	box.SetCurSel(0);
}

void SetTopIndex(CListCtrl& ctl, int topindex) {
	CRect re;
	int	i;

	ctl.EnsureVisible(topindex, FALSE);
	i = ctl.GetTopIndex(); 
	ctl.GetItemRect(i, re, LVIR_BOUNDS);
	ctl.Scroll(CSize(0, (topindex - i) * re.Height()));
}

BOOL SchoolSelector::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	LVCOLUMN lvColumn;
	int nCol;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 40;
	lvColumn.pszText = _T("Code");
	nCol = clistctrl_allcodes.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 200;
	lvColumn.pszText = _T("Name");
	clistctrl_allcodes.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = _T("Type");
	clistctrl_allcodes.InsertColumn(2, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = _T("City");
	clistctrl_allcodes.InsertColumn(3, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 40;
	lvColumn.pszText = _T("State");
	clistctrl_allcodes.InsertColumn(4, &lvColumn);

	clistctrl_allcodes.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (unsigned ssindex = 0; ssindex < pDoc->school_codes.size(); ++ssindex) {
		addToList(clistctrl_allcodes,
			ssindex,
			pDoc->school_codes[ssindex].getSchoolCode(),
			pDoc->school_codes[ssindex].getSchoolName(),
			pDoc->school_codes[ssindex].getSchoolType(),
			pDoc->school_codes[ssindex].getSchoolCity(),
			pDoc->school_codes[ssindex].getSchoolState(),
			ssindex);
	}

	cedit_input_school.SetWindowTextW(WStringToCString(m_school));
	cedit_possible.SetWindowTextW(WStringToCString(m_possible));
	cedit_player_name.SetWindowTextW(WStringToCString(m_name));
	cedit_player_grade.SetWindowTextW(WStringToCString(m_grade));
	SetupStateCombobox(ccombo_state);
	SetupTypeCombobox(ccombo_type);

	auto first_char = toUpper(m_school)[0];
	size_t i = pDoc->school_codes.size();
	auto upper_possible = toUpper(m_possible);

	if (m_possible.length() == 3) {
		for (i=0; i < pDoc->school_codes.size(); ++i) {
			if (upper_possible == toUpper(pDoc->school_codes[i].getSchoolCode()))
			{
				break;
			}
		}
	}

	if (i == pDoc->school_codes.size()) {
		for (i = 0; i < pDoc->school_codes.size(); ++i) {
			if (first_char == toUpper(pDoc->school_codes[i].getSchoolCode())[0])
			{
				break;
			}
		}
	}

	if (i != pDoc->school_codes.size()) {
		SetTopIndex(clistctrl_allcodes, (int)i);
//		clistctrl_allcodes.EnsureVisible(i, FALSE);
	}
	return TRUE;
}


void SchoolSelector::OnDoubleClickSchoolList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	unsigned pindex = (unsigned)clistctrl_allcodes.GetItemData(pNMItemActivate->iItem);

	m_out_code = pDoc->school_codes[pindex].getSchoolCode();
	if (m_unique_key != L"") {
		auto cfiter = pDoc->get_corrected_iter(m_unique_key);
		cfiter->second.school_code = m_out_code;
	}

	*pResult = 0;

	EndDialog(IDOK);
}


void SchoolSelector::OnBnClickedOk()
{
	CString code, name, city;
	cedit_code.GetWindowText(code);
	cedit_name.GetWindowText(name);
	cedit_city.GetWindowText(city);
	int type_sel = ccombo_type.GetCurSel();
	int state_sel = ccombo_state.GetCurSel();

	std::wstring ws_code = CStringToWString(code);
	std::wstring ws_name = CStringToWString(name);
	std::wstring ws_city = CStringToWString(city);
	std::wstring ws_state, ws_type;

	switch (state_sel) {
	case 0:
		ws_state = L"OR";
		break;
	case 1:
		ws_state = L"WA";
		break;
	case 2:
		ws_state = L"ID";
		break;
	case 3:
		ws_state = L"BC";
		break;
	}

	switch (type_sel) {
	case 0:
		ws_type = L"ES";
		break;
	case 1:
		ws_type = L"MS";
		break;
	case 2:
		ws_type = L"HS";
		break;
	case 3:
		ws_type = L"ES/MS";
		break;
	case 4:
		ws_type = L"MS/HS";
		break;
	case 5:
		ws_type = L"ES/MS/HS";
		break;
	}
	
	if (ws_code.length() != 3) {
		MessageBox(_T("School code must be exactly 3 characters long."), _T("Error"));
		return;
	}

	if (pDoc->school_codes.find(ws_code) != -1) {
		MessageBox(_T("The new school code is not unique.  Please consult the above list to choose a unique code."), _T("Error"));
		return;
	}

	pDoc->school_codes.addSchool(AllCodesEntry(ws_code, ws_name, ws_type, ws_city, ws_state));
	m_out_code = ws_code;
	CorrectedFields cf;
	cf.school_code = m_out_code;
	if (m_unique_key != L"") {
		pDoc->saved_school_corrections.insert(std::pair<std::wstring, CorrectedFields>(m_unique_key, cf));
	}
	CDialogEx::OnOK();
}

class se_sort_info {
public:
	int column;
	AllCodes *all_codes;
	std::wstring school;
	se_sort_info(int c, AllCodes *ac, const std::wstring &s) : column(c), all_codes(ac), school(s) {}
};

int CALLBACK se_school_compare(LPARAM l1, LPARAM l2, LPARAM lsort) {
	int p1 = (int)l1;
	int p2 = (int)l2;
	se_sort_info *sort_info = (se_sort_info*)lsort;
	switch (sort_info->column) {
	case 0:
		return listCompareItem((*sort_info->all_codes)[p1].getSchoolCode(), (*sort_info->all_codes)[p2].getSchoolCode());
	case 1:
		return listCompareItem((*sort_info->all_codes)[p1].getSchoolName(), (*sort_info->all_codes)[p2].getSchoolName());
	case 2:
		return listCompareItemFloat(
			       multiWordScore(sort_info->school, ((*sort_info->all_codes)[p1].getSchoolNameUpperNoSchool())),
			       multiWordScore(sort_info->school, ((*sort_info->all_codes)[p2].getSchoolNameUpperNoSchool())));
	default:
		return 0;
	}
}

void SchoolSelector::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	int column = pNMLV->iSubItem;
	if (column >= 0 && column <= 1) {
		int new_column;
		if (column == 0) {
			new_column = 0;
		}
		else {
			switch (last_sort) {
			case 0:
				new_column = 1;
				break;
			case 1:
				if (m_school != L"") {
					new_column = 2;
				}
				else {
					new_column = 1;
				}
				break;
			case 2:
				new_column = 1;
				break;
			}
		}
		if (new_column != last_sort) {
			last_sort = new_column;
		}
		else {
			return;
		}
	}
	se_sort_info sort_info(last_sort, &pDoc->school_codes, toUpper(m_school));
	clistctrl_allcodes.SortItems(se_school_compare, (LPARAM)&sort_info);
}
