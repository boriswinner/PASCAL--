#include <fstream>
#include <sstream>
#include "extlib/dtl/dtl.hpp"
#include "lexer.h"
#define NUMBER_OF_TESTS 8

int main(){
    for (int i = 1; i <= NUMBER_OF_TESTS; ++i){
        std::string filename("../tests/test"+std::to_string(i));

        Lexer lexer(filename+".in");

        std::stringstream lexer_out;
        lexer_out << std::setw(5) << "line" << std::setw(5) << "col" << std::setw(16) << "type" << std::setw(23) << "value" << std::setw(16)
            << "text" << std::endl;
        while (true){
            try{
                Token t = lexer.get_next();
                lexer_out << t.print();
                if (t.type() == token_types::ENDOFFILE){
                    break;
                }
            } catch (LexerException & e){
                lexer_out << e.what() << std::endl;
            } catch(std::exception& e){
                lexer_out << "Unknown exception" << std::endl;;
            }
        }

        std::ifstream out(filename+".out");
        std::stringstream ssout;
        ssout << out.rdbuf();
        std::string lexer_out_str = lexer_out.str();
        lexer_out_str.erase(remove_if(lexer_out_str.begin(), lexer_out_str.end(), isspace), lexer_out_str.end());
        std::string ssout_str = ssout.str();
        ssout_str.erase(remove_if(ssout_str.begin(), ssout_str.end(), isspace), ssout_str.end());
        dtl::Diff <char, std::string> d(lexer_out_str, ssout_str);
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
