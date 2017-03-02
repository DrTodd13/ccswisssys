
// CCSwisssys2View.h : interface of the CCCSwisssys2View class
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CCCSwisssys2View : public CFormView
{
protected: // create from serialization only
	CCCSwisssys2View();
	DECLARE_DYNCREATE(CCCSwisssys2View)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_CCSWISSSYS2_FORM };
#endif

// Attributes
public:
	CCCSwisssys2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CCCSwisssys2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void refillSections(Sections &s);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();
	CEdit ratings_file_edit;
	CEdit constant_contact_file_edit;
	afx_msg void OnEnChangeEdit2();
	CListCtrl section_list;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	CListCtrl section_players;
	bool first_time;
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnEnChangeEdit3();
	CEdit SchoolCodesEdit;
	CSpinButtonCtrl section_spin_button;
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
};

#ifndef _DEBUG  // debug version in CCSwisssys2View.cpp
inline CCCSwisssys2Doc* CCCSwisssys2View::GetDocument() const
   { return reinterpret_cast<CCCSwisssys2Doc*>(m_pDocument); }
#endif

