#pragma once
#include "afxwin.h"
#include "CCSwisssys2Doc.h"
#include "afxcmn.h"


// ManageRegistrations dialog

std::vector<SectionPlayerInfo> process_cc_file(HWND hWnd, CCCSwisssys2Doc *pDoc, bool &error_condition, bool &warning_condition, bool &info_condition, std::wofstream &normal_log);

class ManageRegistrations : public CDialogEx
{
	DECLARE_DYNAMIC(ManageRegistrations)

public:
	ManageRegistrations(CCCSwisssys2Doc *doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~ManageRegistrations();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGE_REGISTRATIONS };
#endif

protected:
	CCCSwisssys2Doc *pDoc;
	int ending_index[27];
	bool inProcessingChange;

	void OnAnyChange();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit LastNameEdit;
	afx_msg void OnEnChangeEdit1();
	CEdit FirstNameEdit;
	afx_msg void OnEnChangeEdit2();
	CEdit NWSRS_ID_Edit;
	afx_msg void OnEnChangeEdit3();
	CListCtrl RegisteredPlayers;
	CListCtrl PossiblePlayers;
	CEdit school_code_edit;
	afx_msg void OnEnChangeEdit4();
	CEdit school_name_edit;
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnAddNewPlayer();
	afx_msg void OnConstantContactFileBrowse();
	afx_msg void OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox grade_combo_box;
	afx_msg void OnCbnSelchangeCombo2();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnForceSection();
};
