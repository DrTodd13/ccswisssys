#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"

// DateSelector2 dialog

class DateSelector2 : public CDialogEx
{
	DECLARE_DYNAMIC(DateSelector2)

public:
	DateSelector2(CTime *d, CWnd* pParent = nullptr);   // standard constructor
	virtual ~DateSelector2();

	CTime *m_tournament_date;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOURNAMENT_DATE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_date_picker;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
