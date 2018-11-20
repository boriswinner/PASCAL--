#pragma once
#include "lexer.h"

void test_lexer(std::string filename, std::stringstream &out) {
    Lexer lexer(filename);
    out << std::setw(5) << "line" << std::setw(5) << "col" << std::setw(16) << "type" << std::setw(23) << "value" <<
           std::setw(16) << "text" << std::endl;
    while (true) {
        try {
            Token t = lexer.get_next();
            out << t.print();
            if (t.type() == ENDOFFILE) {
                break;
            }
        } catch (LexerException &e) {
            out << e.what() << std::endl;
        } catch (std::exception &e) {
            out << "Unknown exception" << std::endl;;
        }
    }
}