#include <string>
#include <iostream>
#include <cstdlib>

std::wstring trim(const std::wstring& str, const std::wstring& whitespace = L" \t");

class Player {
public:
    std::wstring last_name;
    std::wstring first_name;
    std::wstring school_code;
    wchar_t grade;
    std::wstring id;
    unsigned nwsrs_rating;
    unsigned num_games;
    unsigned games_ytd;
    unsigned highest_rating_this_year;
    unsigned lowest_rating;
    unsigned start_rating;
    unsigned kirks_code;
    std::wstring last_action_date;
    wchar_t rating_basis;
    std::wstring uscf_last_name; 
    std::wstring uscf_first_name; 
    std::wstring uscf_id;
    std::wstring uscf_exp_date;
	std::wstring uscf_rating;
	//unsigned uscf_rating;
	std::wstring uscf_prov_codes;
    std::wstring uscf_rating_date;
    std::wstring state;

    unsigned calc_highest_rating;

    friend std::wostream& operator<<(std::wostream &os, Player &p);
    friend std::wistream& operator>>(std::wistream &in, Player &p);

	unsigned getUscfRating() const {
		if (uscf_rating == L"") {
			return 0;
		}
		else {
			return _ttoi(uscf_rating.c_str());
		}
	}

    unsigned get_higher_rating(void) const {
        return nwsrs_rating > getUscfRating() ? nwsrs_rating : getUscfRating();
    }

    std::wstring getFullId(void) const {
        return school_code + grade + id;
    }

    bool isAdult(void) const {
        return grade == 'N';
    }

    bool isUSCFProvOrExpired(void) const {
        return uscf_prov_codes == L"E" || uscf_prov_codes == L"P";
    }

    Player(void) : grade(L'Z'), nwsrs_rating(0), num_games(0), games_ytd(0), highest_rating_this_year(0), lowest_rating(0), start_rating(0), kirks_code(0), rating_basis(L' '), uscf_rating(L""), calc_highest_rating(0) {}
};

std::wstring read_fixed(std::wistream &in, unsigned length);
wchar_t read_fixed_char(std::wistream &in, unsigned length);
unsigned read_fixed_unsigned(std::wistream &in, unsigned length);
std::wistream& operator >> (std::wistream &in, Player &p);
std::wostream& operator<<(std::wostream &os, Player &p);
