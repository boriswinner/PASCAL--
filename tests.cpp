#include <fstream>
#include <sstream>
#include "extlib/dtl/dtl.hpp"
#include "lexer.h"
#define NUMBER_OF_TESTS 10

int main(){
    for (int i = 1; i <= NUMBER_OF_TESTS; ++i){
        std::string filename("../tests/test"+std::to_string(i));

        Lexer lexer(filename+".in");

        std::stringstream lexer_out;
        while (true){
            try{
                Token t = lexer.get_next();
                if (t.type_ == token_types::ENDOFFILE){
                    break;
                }
                lexer_out << t.print();
            } catch (LexerException & e){
                lexer_out << e.what() << std::endl;
            } catch(std::exception& e){
                std::cout << "Unknown exception" << std::endl;;
            }
        }

        std::ifstream out(filename+".out");
        std::stringstream ssout;
        ssout << out.rdbuf();
        dtl::Diff <char, std::string> d(lexer_out.str(), ssout.str());
        d.compose();
        if (d.getEditDistance() == 0){
            std::cout << "TEST" << std::setw(4) << std::to_string(i) <<" PASSED" << std::endl;
        } else{
            std::cout << "TEST" << std::setw(4) << std::to_string(i) <<" FAILED" << std::endl;
        }
// editDistance
//        std::cout << "editDistance:" << d.getEditDistance() << std::endl;

        // Longest Common Subsequence
//        std::vector< char > lcs_v = d.getLcsVec();
//        std::string lcs_s(lcs_v.begin(), lcs_v.end());
//        std::cout << "LCS:" << std::endl << lcs_s << std::endl;

        // Shortest Edit Script
//        std::cout << "SES" << std::endl;
//        d.printSES();
    }
}
