#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"
#include "afxcmn.h"


// SchoolSelector dialog

class SchoolSelector : public CDialogEx
{
	DECLARE_DYNAMIC(SchoolSelector)

public:
	SchoolSelector(CCCSwisssys2Doc *doc, const std::wstring &school, std::wstring &out_code, CWnd* pParent = NULL);   // standard constructor
	virtual ~SchoolSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	CCCSwisssys2Doc *pDoc;
	std::wstring m_school;
	std::wstring &m_out_code;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit cedit_input_school;
	CEdit cedit_code;
	CEdit cedit_name;
	CEdit cedit_city;
	CComboBox ccombo_type;
	CComboBox ccombo_state;
	CListCtrl clistctrl_allcodes;
	afx_msg void OnDoubleClickSchoolList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
