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

TEST_CASE("Strings"){
    std::string filename_in = "test02.txt";
    std::string filename_out = "test02_out.txt";
    std::ifstream file(filename_out);
    std::vector<std::string> answers;
    std::string s;
    while (getline(file, s)){
        answers.push_back(s);
    }
    Lexer lexer(filename_in);
    int i = 0;
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        REQUIRE(t.text_ == answers[i]);
        REQUIRE(t.type_ == token_types::LITERAL);
        ++i;
    }
}

TEST_CASE("Identificators"){
    std::string filename_in = "test03.txt";
    std::string filename_out = "test03.txt";
    std::ifstream file(filename_out);
    std::vector<std::string> answers;
    std::string s;
    while (getline(file, s)){
        answers.push_back(s);
    }
    Lexer lexer(filename_in);
    int i = 0;
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        REQUIRE(t.text_ == answers[i]);
        REQUIRE(t.type_ == token_types::IDENTIFICATOR);
        ++i;
    }
}

TEST_CASE("Keywords"){
    std::string filename_in = "test04.txt";
    std::string filename_out = "test04.txt";
    std::ifstream file(filename_out);
    std::vector<std::string> answers;
    std::string s;
    while (getline(file, s)){
        answers.push_back(s);
    }
    Lexer lexer(filename_in);
    int i = 0;
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        REQUIRE(t.text_ == answers[i]);
        REQUIRE(t.type_ == token_types::KEYWORD);
        ++i;
    }
}

TEST_CASE("Relops"){
    std::string filename_in = "test05.txt";
    std::string filename_out = "test05.txt";
    std::ifstream file(filename_out);
    std::vector<std::string> answers;
    std::string s;
    while (getline(file, s)){
        answers.push_back(s);
    }
    Lexer lexer(filename_in);
    int i = 0;
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        REQUIRE(t.text_ == answers[i]);
        REQUIRE(t.type_ == token_types::OPERATOR);
        ++i;
    }
}