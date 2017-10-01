// ForceSection.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "ForceSection.h"
#include "afxdialogex.h"


// ForceSection dialog

IMPLEMENT_DYNAMIC(ForceSection, CDialogEx)

ForceSection::ForceSection(CCCSwisssys2Doc *doc, int *selected_section, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FORCE_SECTION, pParent), selection(selected_section)
{
	pDoc = doc;
}

ForceSection::~ForceSection()
{
}

void ForceSection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, SectionNames);
}


BEGIN_MESSAGE_MAP(ForceSection, CDialogEx)
	ON_BN_CLICKED(IDOK, &ForceSection::OnBnClickedOk)
END_MESSAGE_MAP()


// ForceSection message handlers


BOOL ForceSection::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto siter = pDoc->sections.begin(); siter != pDoc->sections.end(); ++siter) {
		SectionNames.AddString(siter->name);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ForceSection::OnBnClickedOk()
{
	auto sec_sel = this->SectionNames.GetCurSel();
	*selection = sec_sel;
	CDialogEx::OnOK();
}
