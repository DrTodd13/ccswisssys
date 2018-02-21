// SectionEditor.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "SectionEditor.h"
#include "afxdialogex.h"
#include <sstream>


// SectionEditor dialog

IMPLEMENT_DYNAMIC(SectionEditor, CDialog)

SectionEditor::SectionEditor(Section *s, bool &check, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SECTION_EDITOR, pParent), m_s(s), needs_check(check)
{
}

void SetupGradeCombobox(CComboBox &box) {
	box.AddString(_T("None"));
	box.AddString(_T("A - Kindergarten"));
	box.AddString(_T("B - 1st Grade"));
	box.AddString(_T("C - 2nd Grade"));
	box.AddString(_T("D - 3rd Grade"));
	box.AddString(_T("E - 4th Grade"));
	box.AddString(_T("F - 5th Grade"));
	box.AddString(_T("G - 6th Grade"));
	box.AddString(_T("H - 7th Grade"));
	box.AddString(_T("I - 8th Grade"));
	box.AddString(_T("J - 9th Grade"));
	box.AddString(_T("K - 10th Grade"));
	box.AddString(_T("L - 11th Grade"));
	box.AddString(_T("M - 12th Grade"));
	box.AddString(_T("N - Adult"));
}

void SetupSubsectionCombobox(CComboBox &box) {
	box.AddString(_T("1"));
	box.AddString(_T("2"));
	box.AddString(_T("3"));
	box.AddString(_T("4"));
	box.AddString(_T("5"));
	box.AddString(_T("6"));
	box.AddString(_T("7"));
	box.AddString(_T("8"));
	box.AddString(_T("9"));
	box.AddString(_T("10"));
}

void SetupPairingComputerCombobox(CComboBox &box) {
	box.AddString(_T("1"));
	box.AddString(_T("2"));
	box.AddString(_T("3"));
	box.AddString(_T("4"));
	box.AddString(_T("5"));
}

BOOL SectionEditor::OnInitDialog() {
	CDialog::OnInitDialog();

	section_name_edit.SetWindowText(m_s->name);
	SetupGradeCombobox(min_grade_combo);
	SetupGradeCombobox(max_grade_combo);
	SetupSubsectionCombobox(num_subsections_combo);
	SetupPairingComputerCombobox(num_pairings_computers);
	min_grade_combo.SetCurSel(0);
	max_grade_combo.SetCurSel(0);
	num_subsections_combo.SetCurSel(m_s->num_subsections - 1);
	uscf_required.SetCheck(m_s->uscf_required ? BST_CHECKED : BST_UNCHECKED);
	num_pairings_computers.SetCurSel(m_s->which_computer - 1);
	//	section_name_edit.SetWindowText(m_s->subsections);

	if (m_s->usedRatings()) {
		std::wstringstream ss;
		ss << m_s->lower_rating_limit;
		min_rating_edit.SetWindowText(CString(ss.str().c_str()));
		ss.str(L"");
		ss << m_s->upper_rating_limit;
		max_rating_edit.SetWindowText(CString(ss.str().c_str()));
	}
	if (m_s->usedGrade()) {
		min_grade_combo.SetCurSel(m_s->lower_grade_limit - 'A' + 1);
		max_grade_combo.SetCurSel(m_s->upper_grade_limit - 'A' + 1);
	}
	section_type_combo.AddString(_T("Swiss"));
	section_type_combo.AddString(_T("Round Robin"));
	section_type_combo.SetCurSel(m_s->sec_type);

	return FALSE;
}

SectionEditor::~SectionEditor()
{
}

void SectionEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, section_name_edit);
	DDX_Control(pDX, IDC_EDIT2, min_rating_edit);
	DDX_Control(pDX, IDC_EDIT3, max_rating_edit);
	DDX_Control(pDX, IDC_COMBO1, section_type_combo);
	DDX_Control(pDX, IDC_COMBO2, min_grade_combo);
	DDX_Control(pDX, IDC_COMBO3, max_grade_combo);
	DDX_Control(pDX, IDC_COMBO4, num_subsections_combo);
	DDX_Control(pDX, IDC_CHECK1, uscf_required);
	DDX_Control(pDX, IDC_PAIRING_COMBO, num_pairings_computers);
}


BEGIN_MESSAGE_MAP(SectionEditor, CDialog)
	ON_BN_CLICKED(IDOK, &SectionEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SectionEditor::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT4, &SectionEditor::OnEnChangeSubsections)
END_MESSAGE_MAP()


// SectionEditor message handlers

bool isNumeric(const CString &cs) {
	if (cs.SpanIncluding(_T("0123456789")) == cs) return true;
	else return false;
}

template<class T>
void set_changed(T &old, const T& newvalue, bool &changed) {
	if (old != newvalue) {
		old = newvalue;
		changed = true;
	}
}

void SectionEditor::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString new_section_name, new_min_rating, new_max_rating;
	int min_grade_sel, max_grade_sel, new_section_type_sel;
	unsigned num_subsections_sel, which_computer_sel;
	bool name_change = false, other_change = false;

	section_name_edit.GetWindowText(new_section_name);
	min_rating_edit.GetWindowText(new_min_rating);
	max_rating_edit.GetWindowText(new_max_rating);
	min_grade_sel = min_grade_combo.GetCurSel();
	max_grade_sel = max_grade_combo.GetCurSel();
	num_subsections_sel = num_subsections_combo.GetCurSel() + 1;
	which_computer_sel = num_pairings_computers.GetCurSel() + 1;
	new_section_type_sel = section_type_combo.GetCurSel();
	int new_min_int, new_max_int;
	int uscf_required_int = uscf_required.GetCheck();
	
	if (new_section_name.GetLength() == 0) {
		MessageBox(_T("Section name should not be empty."), _T("Section Editor Error"));
		return;
	}
	if (min_grade_sel == 0 && max_grade_sel != 0) {
		MessageBox(_T("If maximum grade is specified then minimum must be specified also"), _T("Section Editor Error"));
		return;
	}
	if (min_grade_sel != 0 && max_grade_sel == 0) {
		MessageBox(_T("If minumum grade is specified then maximum must be specified also"), _T("Section Editor Error"));
		return;
	}
	if (max_grade_sel < min_grade_sel) {
		MessageBox(_T("Maximum grade must be larger than minimum grade."), _T("Section Editor Error"));
		return;
	}

	if (new_min_rating.GetLength() == 0) {
		new_min_int = 0;
	} else if (isNumeric(new_min_rating)) {
		new_min_int = _ttoi(new_min_rating);
	}
	else {
		MessageBox(_T("Minimum rating is not numeric."), _T("Section Editor Error"));
		return;
	}

	if (new_max_rating.GetLength() == 0) {
		new_max_int = 3000;
	}
	else if (isNumeric(new_max_rating)) {
		new_max_int = _ttoi(new_max_rating);
	}
	else {
		MessageBox(_T("Maximum rating is not numeric."), _T("Section Editor Error"));
		return;
	}
	if (new_min_int > new_max_int) {
		MessageBox(_T("Maximum rating is less than the minimum rating."), _T("Section Editor Error"));
		return;
	}
	if (new_min_int < 0 || new_min_int > 3000) {
		MessageBox(_T("Minimum rating range is 0-3000."), _T("Section Editor Error"));
		return;
	}
	if (new_max_int < 0 || new_max_int > 3000) {
		MessageBox(_T("Minimum rating range is 0-3000."), _T("Section Editor Error"));
		return;
	}
	set_changed(m_s->name, new_section_name, name_change);
	set_changed(m_s->lower_rating_limit, new_min_int, other_change);
	set_changed(m_s->upper_rating_limit, new_max_int, other_change);
	if (min_grade_sel == 0) {
		set_changed(m_s->lower_grade_limit, wchar_t('A'), other_change);
	}
	else {
		set_changed(m_s->lower_grade_limit, wchar_t('A' + min_grade_sel - 1), other_change);
	}
	if (max_grade_sel == 0) {
		set_changed(m_s->upper_grade_limit, wchar_t('N'), other_change);
	}
	else {
		set_changed(m_s->upper_grade_limit, wchar_t('A' + max_grade_sel - 1), other_change);
	}
	set_changed(m_s->sec_type, SECTION_TYPE(new_section_type_sel), other_change);
	set_changed(m_s->num_subsections, num_subsections_sel, other_change);
	set_changed(m_s->which_computer, which_computer_sel, name_change);
	set_changed(m_s->uscf_required, uscf_required_int == BST_CHECKED ? true : false, other_change);

	if (other_change || name_change) {
		needs_check = other_change;
		CDialog::OnOK();
	}
	else {
		CDialog::OnCancel();
	}
}


void SectionEditor::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void SectionEditor::OnEnChangeSubsections()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
