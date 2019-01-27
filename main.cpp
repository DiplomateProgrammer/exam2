#include <iostream>
#include "unique_ptr.h"
using namespace std;
struct test
{
    test(int a, int b, int c): a(a), b(b), c(c) {}
    int a, b, c;
};

int main()
{
    my::unique_ptr<test> xyz = my::make_unique<test>(1, 2, 3);
    int c = xyz.get()->c;
    cout << c << "\n";
    xyz.reset(new test(4, 5, 6), std::default_delete<test>());
    cout << xyz.get()->a << "\n";
}
