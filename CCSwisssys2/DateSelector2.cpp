// DateSelector2.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "DateSelector2.h"
#include "afxdialogex.h"


// DateSelector2 dialog

IMPLEMENT_DYNAMIC(DateSelector2, CDialogEx)

DateSelector2::DateSelector2(CTime *d, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOURNAMENT_DATE, pParent), m_tournament_date(d)
{

}

DateSelector2::~DateSelector2()
{
}

void DateSelector2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date_picker);
}


BEGIN_MESSAGE_MAP(DateSelector2, CDialogEx)
	ON_BN_CLICKED(IDOK, &DateSelector2::OnBnClickedOk)
END_MESSAGE_MAP()


// DateSelector2 message handlers


BOOL DateSelector2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->m_date_picker.SetTime(m_tournament_date);
	return FALSE;
}


void DateSelector2::OnBnClickedOk()
{
	m_date_picker.GetTime(*m_tournament_date);
	CDialogEx::OnOK();
}
