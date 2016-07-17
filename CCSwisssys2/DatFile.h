#include <string>
#include <iostream>
#include <cstdlib>

std::string trim(const std::string& str,
	const std::string& whitespace = " \t");

class Player {
public:
    std::string last_name;
    std::string first_name;
    std::string school_code;
    char grade;
    std::string id;
    unsigned nwsrs_rating;
    unsigned num_games;
    unsigned games_ytd;
    unsigned highest_rating_this_year;
    unsigned lowest_rating;
    unsigned start_rating;
    unsigned kirks_code;
    std::string last_action_date;
    char rating_basis;
    std::string uscf_last_name; 
    std::string uscf_first_name; 
    std::string uscf_id;
    std::string uscf_exp_date;
    unsigned uscf_rating;
    std::string uscf_prov_codes;
    std::string uscf_rating_date;
    std::string state;

    unsigned calc_highest_rating;

    friend std::ostream& operator<<(std::ostream &os, Player &p);
    friend std::istream& operator>>(std::istream &in, Player &p);

    unsigned get_higher_rating(void) const {
        return nwsrs_rating > uscf_rating ? nwsrs_rating : uscf_rating;
    }

    std::string getFullId(void) const {
        return school_code + grade + id;
    }

    bool isAdult(void) const {
        return grade == 'N';
    }

    bool isUSCFProvOrExpired(void) const {
        return uscf_prov_codes == "E" || uscf_prov_codes == "P";
    }

    Player(void) : grade('Z'), nwsrs_rating(0), num_games(0), games_ytd(0), highest_rating_this_year(0), lowest_rating(0), start_rating(0), kirks_code(0), rating_basis(' '), uscf_rating(0), calc_highest_rating(0) {}
};

std::string read_fixed(std::istream &in, unsigned length);
char read_fixed_char(std::istream &in, unsigned length);
unsigned read_fixed_unsigned(std::istream &in, unsigned length);
std::istream& operator >> (std::istream &in, Player &p);
std::ostream& operator<<(std::ostream &os, Player &p);
