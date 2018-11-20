#include <fstream>
#include <sstream>
#include "extlib/dtl/dtl.hpp"
#include "lexer.h"
#include "dirent.h"
#define TESTS_DIRECTORY "../tests/"

void execute_test(const std::string(&filename)) {
    std::string full_filename = (TESTS_DIRECTORY + filename);

    Lexer lexer(full_filename + ".in");

    std::stringstream lexer_out;
    lexer_out << std::setw(5) << "line" << std::setw(5) << "col" << std::setw(16) << "type" << std::setw(23) <<
              "value" << std::setw(16) << "text" << std::endl;
    while (true) {
        try {
            Token t = lexer.get_next();
            lexer_out << t.print();
            if (t.type() == ENDOFFILE) {
                break;
            }
        } catch (LexerException &e) {
            lexer_out << e.what() << std::endl;
        } catch (std::exception &e) {
            lexer_out << "Unknown exception" << std::endl;;
        }
    }

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


