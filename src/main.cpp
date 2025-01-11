#include "configuration_loader.hpp"


int main()
{
    ConfigurationLoader loader("conf.txt");
    auto const color = loader.getValueAsVector<float>("my_color");

    if (color) {
        for (float v : *color) {
            std::cout << v << std::endl;
        }
    } else {
        std::cout << "Error" << std::endl;
    }

    return 0;
}
