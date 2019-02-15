
// CCSwisssys2View.h : interface of the CCCSwisssys2View class
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class SectionIndex {
public:
	const Section * section;
	int index;

	SectionIndex(const Section *s, int i) : section(s), index(i) {}

	bool operator<(const SectionIndex &other) const {
		if (section->players[index].last_name < other.section->players[other.index].last_name) return true;
		if (section->players[index].last_name > other.section->players[other.index].last_name) return false;
		return section->players[index].first_name < other.section->players[other.index].first_name;
	}
};

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
	bool section_sort[8];
	void reset_section_sort() {
		section_sort[0] = false;
		section_sort[1] = false;
		section_sort[2] = true;
		section_sort[3] = false;
		section_sort[4] = false;
		section_sort[5] = false;
		section_sort[6] = true;
		section_sort[7] = false;
	}
	int last_section_sort;

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
	void refillSections(Sections &s, bool clear=true, int selection_mark=-1);
	unsigned createSectionWorksheet(std::wofstream &normal_log, const std::wstring &output_dir, std::wstring &sec_name, const Section &sec, int subsec, std::vector<SectionIndex> &vsi);
	void createAllCheckinWorksheet(std::wofstream &normal_log, const std::wstring &output_dir, const std::vector<SectionIndex> &vsi);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRatingsFileBrowse();
	afx_msg void OnConstantContactFileBrowse();
	afx_msg void OnEnChangeEdit1();
	CEdit ratings_file_edit;
	CEdit constant_contact_file_edit;
	afx_msg void OnEnChangeEdit2();
	CListCtrl section_list;
	afx_msg void OnAddSection();
	afx_msg void OnDeleteSection();
	afx_msg void OnEditSection();
	afx_msg void OnCreateSections();
	CListCtrl section_players;
	bool first_time;
	afx_msg void OnSectionColumnClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSectionClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreateClubFiles();
	afx_msg void OnSchoolCodesFileBrowse();
	afx_msg void OnEnChangeEdit3();
	CEdit SchoolCodesEdit;
	CSpinButtonCtrl section_spin_button;
	afx_msg void OnSectionReorder(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSectionPlayersColumnSort(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnManageAdditionalRegistrations();
	afx_msg void OnCreateSwisssysTourney();
	afx_msg void OnRestrictFileBrowse();
	CEdit restricted_edit;
	afx_msg void OnDoubleClickSectionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSplitSection();
	afx_msg void OnOptionsSaveschoolcorrections();
	afx_msg void OnUpdateOptionsSaveschoolcorrections(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in CCSwisssys2View.cpp
inline CCCSwisssys2Doc* CCCSwisssys2View::GetDocument() const
   { return reinterpret_cast<CCCSwisssys2Doc*>(m_pDocument); }
#endif
