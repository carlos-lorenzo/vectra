#include <iostream>
#include <linkit/vector3.h>

int main()
{
    auto i = linkit::Vector3(1, 0, 0);
    auto j = linkit::Vector3(0, 1, 0);
    auto k = linkit::Vector3(0, 0, 1);

    linkit::Vector3 v1 = i + j + 2*k;
    std::cout << sizeof(linkit::Vector3) << std::endl;

    
    std::cout << v1.to_string() << std::endl;
}