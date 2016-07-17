#include <vector>
#include <string>
#include <fstream>

std::vector< std::vector<std::string> > load_csv_file(const std::string &filename, bool skip_header = false);
std::vector<std::string> tokenize_csv(const std::string &s);