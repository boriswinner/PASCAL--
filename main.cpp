#include <iostream>
#include "lexer.h"

using namespace std;


int main() {
    Lexer lexer("input.txt");
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        t.print();
    }

    return 0;
}