#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"
#include <sstream>

// SectionEditor dialog

class SectionEditor : public CDialog
{
	DECLARE_DYNAMIC(SectionEditor)

public:
	SectionEditor(Section *s, CWnd* pParent = NULL);   // standard constructor
	virtual ~SectionEditor();

	Section *m_s;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit section_name_edit;
	CEdit min_rating_edit;
	CEdit max_rating_edit;
	CComboBox section_type_combo;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox min_grade_combo;
	CComboBox max_grade_combo;
};
