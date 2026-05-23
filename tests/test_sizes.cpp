#include <iostream>
#include "core/CanvasNode.hpp"
#include "core/RectNode.hpp"

using namespace vectma;

int main() {
    std::cout << "Size of CanvasNode: " << sizeof(CanvasNode) << std::endl;
    std::cout << "Size of RectNode: " << sizeof(RectNode) << std::endl;
    std::cout << "Size of LWWProperty<double>: " << sizeof(LWWProperty<double>) << std::endl;
    std::cout << "Size of LamportTimestamp: " << sizeof(LamportTimestamp) << std::endl;
    return 0;
}
