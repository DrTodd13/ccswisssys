#include <vector>
#include <string>
#include <fstream>

std::vector< std::vector<std::wstring> > load_csvw_file(const std::wstring &filename, bool skip_header = false);
std::vector<std::wstring> tokenize_csv(const std::wstring &s);