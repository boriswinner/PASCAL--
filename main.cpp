#include <iostream>
#include "lexer_executor.h"
using namespace std;

int main() {
    std::ofstream out("out.txt");
    std::stringstream ss;
    test_lexer("input.txt", ss);
    out << ss.str();
    return 0;
}

