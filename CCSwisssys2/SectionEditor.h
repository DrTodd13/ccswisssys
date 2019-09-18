#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"
#include <sstream>

// SectionEditor dialog

class SectionEditor : public CDialog
{
	DECLARE_DYNAMIC(SectionEditor)

public:
	SectionEditor(Section *s, CCCSwisssys2Doc* d, bool &check, CWnd* pParent = NULL);   // standard constructor
	virtual ~SectionEditor();

	Section *m_s;
	bool &needs_check;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECTION_EDITOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit section_name_edit;
	CEdit min_rating_edit;
	CEdit max_rating_edit;
	CComboBox section_type_combo;
	CEdit num_subsections_edit;
	CCCSwisssys2Doc *pDoc;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox min_grade_combo;
	CComboBox max_grade_combo;
	CComboBox num_subsections_combo;
	afx_msg void OnEnChangeSubsections();
	CButton uscf_required;
	CComboBox num_pairings_computers;
	CEdit time_control_edit;
	CEdit board_number_edit;
	CEdit playing_room_edit;
	CComboBox num_rounds_combobox;
	CButton AutoResection;
	afx_msg void OnBnClickedAutoResecCheck();
};
