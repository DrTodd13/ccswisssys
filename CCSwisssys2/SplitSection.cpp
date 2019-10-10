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
	DDX_Control(pDX, IDC_AUTO_RESECTION, auto_resection);
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
			int num_in_section = (int)sec->players.size();
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
		int num_in_section = (int)sec->players.size();
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

CString tocstring(int x) {
	std::wstringstream new_name;
	new_name << x;
	return WStringToCString(new_name.str());
}

/*
 * num_split = 0 for quads
 *             2+ for even split
 */
std::vector<Section> do_split(const Section *sec, int num_split, HWND hwnd) {
	bool make_quads = false;
	bool make_equal = false;
	bool quad_swiss = false;
	int sections_left = 0;
	int int_num_split = 0;

	if (num_split == 0) {
		make_quads = true;
	}
	else if (num_split >= 2) {
		make_equal = true;
		int_num_split = num_split;
	}
	else {
		ASSERT(0);
	}
	int players_left = (int)sec->players.size();
	int new_num_sections = 0;

	if (make_quads) {
		// This works for both even and odd sections.
		// If there is between a full quad plus one to full quads plus 3
		// then this integer division rounds down because those extra
		// players are absorbed into a 5+ swiss section.
		sections_left = players_left / 4;
		quad_swiss = ((players_left % 4) != 0 ? true : false);
	}
	else if (make_equal) {
		sections_left = int_num_split;
	}
	else {
		ASSERT(0);
	}

	new_num_sections = sections_left;

	std::vector<SectionPlayerInfo> working_copy = sec->players;
	std::sort(working_copy.begin(), working_copy.end(), customSplitLess);
	int next_to_use = 0;
	std::vector<Section> new_sections;
	int cur_sec = 0;
	int cur_upper_limit = sec->upper_rating_limit;
	int cur_board_num = _ttoi(sec->starting_board_number);

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

		newSection.time_control = sec->time_control;

		if (make_quads) {
			newSection.num_rounds = 3;
			if (sections_left > 1 || !quad_swiss) {
				newSection.sec_type = ROUND_ROBIN;
			}
			else {
				newSection.sec_type = SWISS;
			}
		}
		else {
			newSection.sec_type = sec->sec_type;
			newSection.num_rounds = sec->num_rounds;
		}

		newSection.upper_grade_limit = sec->upper_grade_limit;
		newSection.uscf_required = sec->uscf_required;
		std::wstringstream new_name;
		new_name << CStringToWString(sec->name) << L"-" << cur_sec;
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
		
		if (cur_board_num != 0) {
			newSection.starting_board_number = tocstring(cur_board_num);
			cur_board_num = newSection.getLastBoardNumber() + 1;
		}

		cur_upper_limit--;
		new_sections.push_back(newSection);
	}

	if (new_sections.size() != new_num_sections) {
//		MessageBox(hwnd, _T("Split section algorithm failed to split into the requested number of sections."), _T("ERROR"));
		ASSERT(0);
	}
	return new_sections;
}

void SplitSection::OnBnClickedOk()
{
	int ns_param = 0;
	if (QuadButton.GetCheck()) {
		ns_param = 0;
		// All quad sections will be 3 rounds.
		sec->num_rounds = 3;
	}
	else if (EqualButton.GetCheck()) {
		ns_param = int_num_split;
	}

	int auto_resec = auto_resection.GetCheck();
	std::vector<Section> new_sections = do_split(sec, ns_param, GetSafeHwnd());

#if 0
	bool make_quads = false;
	bool make_equal = false;
	bool quad_swiss = false;
	int sections_left = 0;

	int players_left = (int)sec->players.size();
	int new_num_sections = 0;

	if (QuadButton.GetCheck()) {
		make_quads = true;

		// This works for both even and odd sections.
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
#endif

	unsigned next_key = -1;
	if (auto_resec) {
		next_key = doc->parent_sections.last_key() + 1;
		sec->num_split = ns_param;
		doc->parent_sections[next_key] = *sec;
	}

	doc->sections.erase(doc->sections.begin() + index);

	unsigned i;
	for (i = 0; i < new_sections.size(); ++i) {
		if (auto_resec) {
			new_sections[i].parent_section = next_key;
		}
		doc->sections.push_back(new_sections[i]);
	}

	CDialog::OnOK();
}
