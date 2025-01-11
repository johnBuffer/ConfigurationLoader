#include "configuration_loader.hpp"


int main()
{
    ConfigurationLoader loader("conf.txt");
    auto const color = loader.getValueAsVector<int32_t>("my_color");

    if (color) {
        for (int32_t v : *color) {
            std::cout << v << std::endl;
        }
    } else {
        std::cout << "Error" << std::endl;
    }

    return 0;
}
