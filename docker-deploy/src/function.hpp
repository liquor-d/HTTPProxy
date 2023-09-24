#include <vector>
#include <string>

std::vector<char> str_to_charvec(const std::string & str) {
    std::vector<char> charvec;
    for (auto it = str.begin(); it != str.end(); it++) {
        charvec.push_back(*it);
    }
    return charvec;
}