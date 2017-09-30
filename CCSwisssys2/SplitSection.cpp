// SplitSection.cpp : implementation file
//

#include "stdafx.h"
#include "CCSwisssys2.h"
#include "SplitSection.h"
#include "afxdialogex.h"


// SplitSection dialog

IMPLEMENT_DYNAMIC(SplitSection, CDialog)

SplitSection::SplitSection(Section *s, CCCSwisssys2Doc* d, int i, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SPLIT_SECTION, pParent)
{
	int_num_split = 2;
	sec = s;
	doc = d;
	index = i;
}

SplitSection::~SplitSection()
{
}

void SplitSection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NUM_SPLIT, num_split);
	DDX_Control(pDX, IDC_RADIO_MAKE_QUADS, QuadButton);
	DDX_Control(pDX, IDC_RADIO_EQUAL, EqualButton);
}


BEGIN_MESSAGE_MAP(SplitSection, CDialog)
	ON_EN_CHANGE(IDC_NUM_SPLIT, &SplitSection::OnEnChangeNumSplit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPLIT_SPIN, &SplitSection::OnDeltaposSplitSpin)
	ON_BN_CLICKED(IDOK, &SplitSection::OnBnClickedOk)
END_MESSAGE_MAP()


// SplitSection message handlers


void SplitSection::OnEnChangeNumSplit()
{
	CString new_split;
	num_split.GetWindowText(new_split);
	std::wstring new_split_wstr(new_split);
	if (isNumeric(new_split_wstr)) {
		int int_new_split = _ttoi(new_split_wstr.c_str());
		if (int_new_split >= 2) {
			int num_in_section = sec->players.size();
			double est_section_size = num_in_section / (int_new_split + 1.0);
			if (est_section_size < 2.0) {
				MessageBox(_T("Splitting into this many sections would cause average section size to be less than 2."), _T("Information"), MB_OK);
			}
			else {
				int_num_split = int_new_split;
			}
		}
		else {
			set_num_split();
		}
	}
	else {
		set_num_split();
	}
}


void SplitSection::OnDeltaposSplitSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	if (pNMUpDown->iDelta == -1) {
		int num_in_section = sec->players.size();
		double est_section_size = num_in_section / (int_num_split + 1.0);
		if (est_section_size < 2.0) {
			MessageBox(_T("Splitting into this many sections would cause average section size to be less than 2."), _T("Information"), MB_OK);
		}
		else {
			int_num_split++;
		}
	}
	else if (pNMUpDown->iDelta == 1) {
		if (int_num_split >= 3) {
			int_num_split--;
		}
	}
	else {
		MessageBox(_T("spin control idelta was not -1 or 1"), _T("ERROR"));
		return;
	}

	set_num_split();
}

void SplitSection::set_num_split() {
	std::wstringstream ss;
	ss << int_num_split;
	num_split.SetWindowText(CString(ss.str().c_str()));
}

BOOL SplitSection::OnInitDialog()
{
	CDialog::OnInitDialog();

	set_num_split();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

struct {
	bool operator()(SectionPlayerInfo &a, SectionPlayerInfo &b) const
	{
		return a.rating > b.rating;
	}
} customSplitLess;

void SplitSection::OnBnClickedOk()
{
	bool make_quads = false;
	bool make_equal = false;
	bool quad_swiss = false;
	int sections_left = 0;

	int players_left = sec->players.size();
	int new_num_sections = 0;

	if (QuadButton.GetCheck()) {
		make_quads = true;

		// This works for both even and off sections.
		// If there is between a full quad plus one to full quads plus 3
		// then this integer division rounds down because those extra
		// players are absorbed into a 5+ swiss section.
		sections_left = players_left / 4;
		quad_swiss = ((players_left % 4) != 0 ? true : false);
	}
	else if (EqualButton.GetCheck()) {
		make_equal = true;

		sections_left = int_num_split;
	}

	new_num_sections = sections_left;

	if (!make_quads && !make_equal) return;

	std::vector<SectionPlayerInfo> working_copy = sec->players;
	std::sort(working_copy.begin(), working_copy.end(), customSplitLess);
	int next_to_use = 0;
	std::vector<Section> new_sections;
	int cur_sec = 0;
	int cur_upper_limit = sec->upper_rating_limit;

	for (; sections_left > 0; --sections_left) {
		cur_sec++;
		int approx_size = players_left;

		if (sections_left > 1) {
			if (make_quads) {
				approx_size = 4;
			}
			else {
				approx_size = players_left / sections_left;
				if (approx_size % 2 == 1) {
					approx_size--;
				}
			}
		}

		players_left -= approx_size;

		Section newSection;
		newSection.lower_grade_limit = sec->lower_grade_limit;

		if (make_quads) {
			if (sections_left > 1 || !quad_swiss) {
				newSection.sec_type = ROUND_ROBIN;
			}
			else {
				newSection.sec_type = SWISS;
			}
		}
		else {
			newSection.sec_type = sec->sec_type;
		}

		newSection.upper_grade_limit = sec->upper_grade_limit;
		newSection.uscf_required = sec->uscf_required;
		std::wstringstream new_name;
		new_name << CStringToWString(sec->name) << L"." << cur_sec;
		newSection.name = WStringToCString(new_name.str());
		newSection.upper_rating_limit = cur_upper_limit;

		int limit = next_to_use + approx_size;
		for (; next_to_use < limit; next_to_use++) {
			newSection.players.push_back(working_copy[next_to_use]);
			cur_upper_limit = working_copy[next_to_use].rating;
		}

		if (sections_left > 1) {
			newSection.lower_rating_limit = cur_upper_limit;
		}
		else {
			newSection.lower_rating_limit = sec->lower_rating_limit;
		}
		cur_upper_limit--;
		new_sections.push_back(newSection);
	}

	if (new_sections.size() != new_num_sections) {
		MessageBox(_T("Split section algorithm failed to split into the requested number of sections."), _T("ERROR"));
		return;
	}

	doc->sections.erase(doc->sections.begin() + index);

	unsigned i;
	for (i = 0; i < new_sections.size(); ++i) {
		doc->sections.push_back(new_sections[i]);
	}

	CDialog::OnOK();
}
