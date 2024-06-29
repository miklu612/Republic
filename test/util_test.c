#include"../src/util.h"



int main() {
    assert(is_digit('0') == true);
    assert(is_digit('1') == true);
    assert(is_digit('2') == true);
    assert(is_digit('3') == true);
    assert(is_digit('4') == true);
    assert(is_digit('5') == true);
    assert(is_digit('6') == true);
    assert(is_digit('7') == true);
    assert(is_digit('8') == true);
    assert(is_digit('9') == true);
    assert(is_digit('a') == false);
    return 0;
}
