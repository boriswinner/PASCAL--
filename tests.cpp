#include <fstream>
#include <sstream>
#include "extlib/dtl/dtl.hpp"
#include "lexer_executor.h"
#include "dirent.h"
#define TESTS_DIRECTORY "../tests/"

void execute_test(const std::string(&filename)) {
    std::string full_filename = (TESTS_DIRECTORY + filename);
    std::stringstream lexer_out;

    test_lexer(full_filename + ".in", lexer_out);
    std::ifstream out(full_filename + ".out");
    std::stringstream ssout;
    ssout << out.rdbuf();
    std::__cxx11::string lexer_out_str = lexer_out.str();
    lexer_out_str.erase(remove_if(lexer_out_str.begin(), lexer_out_str.end(), isspace), lexer_out_str.end());
    std::__cxx11::string ssout_str = ssout.str();
    ssout_str.erase(remove_if(ssout_str.begin(), ssout_str.end(), isspace), ssout_str.end());
    dtl::Diff<char, std::__cxx11::string> d(lexer_out_str, ssout_str);
    d.compose();
    if (d.getEditDistance() == 0) {
        std::cout << std::setw(8) << filename << " PASSED" << std::endl;
    } else {
        std::cout << std::setw(8) << filename << " FAILED" << std::endl;
        std::cout << "PROGRAM OUTPUT" << std::endl;
        std::cout << lexer_out.str() << std::endl;
        std::cout << "TEST OUTPUT" << std::endl;
        std::cout << ssout.str() << std::endl;
    }
}

int main() {
    DIR *d;
    char *p1, *p2;
    int ret;
    struct dirent *dir;
    d = opendir(TESTS_DIRECTORY);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            p1 = strtok(dir->d_name, ".");
            p2 = strtok(NULL, ".");
            if (p2 != NULL) {
                ret = strcmp(p2, "in");
                if (ret == 0) {
                    execute_test(p1);
                }
            }

        }
        closedir(d);
    } else {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
}


