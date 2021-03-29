#include "tak.h"

#include <iostream>

int main()
{
    Position position(6);
    std::cout << position.print();

    position.place(Place(0, Stone::WhiteFlat));
    std::cout << position.print();
    return 0;
}
