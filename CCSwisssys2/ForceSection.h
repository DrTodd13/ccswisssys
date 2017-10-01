#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"

// ForceSection dialog

class ForceSection : public CDialogEx
{
	DECLARE_DYNAMIC(ForceSection)

public:
	ForceSection(CCCSwisssys2Doc *doc, int *selected_section, CWnd* pParent = NULL);   // standard constructor
	virtual ~ForceSection();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORCE_SECTION };
#endif

protected:
	CCCSwisssys2Doc *pDoc;
	int *selection;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox SectionNames;
	afx_msg void OnBnClickedOk();
};
