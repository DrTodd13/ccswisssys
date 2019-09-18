#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"

// SplitSection dialog

class SplitSection : public CDialog
//	class SplitSection : public CDialogEx
{
	DECLARE_DYNAMIC(SplitSection)

public:
	SplitSection(Section *s, CCCSwisssys2Doc* d, int i, CWnd* pParent = NULL);   // standard constructor
	virtual ~SplitSection();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPLIT_SECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void set_num_split();

	DECLARE_MESSAGE_MAP()
public:
	CEdit num_split;
	int int_num_split;
	int index;
	Section *sec;
	CCCSwisssys2Doc *doc;
	afx_msg void OnEnChangeNumSplit();
	afx_msg void OnDeltaposSplitSpin(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CButton QuadButton;
	CButton EqualButton;
	CButton auto_resection;
};

std::vector<Section> do_split(const Section *sec, int num_split, HWND hwnd);