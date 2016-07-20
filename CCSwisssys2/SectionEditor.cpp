// SectionEditor.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "SectionEditor.h"
#include "afxdialogex.h"
#include <sstream>


// SectionEditor dialog

IMPLEMENT_DYNAMIC(SectionEditor, CDialog)

SectionEditor::SectionEditor(Section *s, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent), m_s(s)
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

BOOL SectionEditor::OnInitDialog() {
	CDialog::OnInitDialog();

	section_name_edit.SetWindowText(m_s->name);
	SetupGradeCombobox(min_grade_combo);
	SetupGradeCombobox(max_grade_combo);
	min_grade_combo.SetCurSel(0);
	max_grade_combo.SetCurSel(0);

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
}


BEGIN_MESSAGE_MAP(SectionEditor, CDialog)
	ON_BN_CLICKED(IDOK, &SectionEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SectionEditor::OnBnClickedCancel)
END_MESSAGE_MAP()


// SectionEditor message handlers

bool isNumeric(const CString &cs) {
	if (cs.SpanIncluding(_T("0123456789")) == cs) return true;
	else return false;
}

void SectionEditor::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString new_section_name, new_min_rating, new_max_rating;
	int min_grade_sel, max_grade_sel, new_section_type_sel;

	section_name_edit.GetWindowText(new_section_name);
	min_rating_edit.GetWindowText(new_min_rating);
	max_rating_edit.GetWindowText(new_max_rating);
	min_grade_sel = min_grade_combo.GetCurSel();
	max_grade_sel = max_grade_combo.GetCurSel();
	new_section_type_sel = section_type_combo.GetCurSel();
	int new_min_int, new_max_int;
	
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
	m_s->name = new_section_name;
	m_s->lower_rating_limit = new_min_int;
	m_s->upper_rating_limit = new_max_int;
	if (min_grade_sel == 0) {
		m_s->lower_grade_limit = 'A';
	}
	else {
		m_s->lower_grade_limit = 'A' + min_grade_sel - 1;
	}
	if (max_grade_sel == 0) {
		m_s->upper_grade_limit = 'M';
	}
	else {
		m_s->upper_grade_limit = 'A' + max_grade_sel - 1;
	}
	m_s->sec_type = SECTION_TYPE(new_section_type_sel);

	CDialog::OnOK();
}


void SectionEditor::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
