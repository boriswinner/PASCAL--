#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "lexer.h"
#include "vector"

TEST_CASE ("Numbers"){
    std::string filename = "test01.txt";
    std::ifstream file(filename);
    std::vector<std::string> answers;
    std::string s;
    while (getline(file, s)){
        answers.push_back(s);
    }
    Lexer lexer(filename);
    int i = 0;
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        REQUIRE(t.text_ == answers[i]);
        REQUIRE(t.type_ == token_types::NUMBER);
        ++i;
    }
}