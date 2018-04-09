#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"
#include "afxcmn.h"


// SchoolSelector dialog

class SchoolSelector : public CDialogEx
{
	DECLARE_DYNAMIC(SchoolSelector)

public:
	SchoolSelector(CCCSwisssys2Doc *doc, const std::wstring &school, const std::wstring &in_code, std::wstring &out_code, const std::wstring &name, const std::wstring &grade, CWnd* pParent = NULL);   // standard constructor
	virtual ~SchoolSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	CCCSwisssys2Doc *pDoc;
	std::wstring m_school;
	std::wstring m_possible;
	std::wstring m_name;
	std::wstring m_grade;
	std::wstring &m_out_code;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit cedit_input_school;
	CEdit cedit_possible;
	CEdit cedit_code;
	CEdit cedit_name;
	CEdit cedit_city;
	CEdit cedit_player_name;
	CEdit cedit_player_grade;
	CComboBox ccombo_type;
	CComboBox ccombo_state;
	CListCtrl clistctrl_allcodes;

	int last_sort;

	afx_msg void OnDoubleClickSchoolList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
