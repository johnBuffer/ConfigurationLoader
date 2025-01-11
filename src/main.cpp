#include "configuration_loader.hpp"


int main()
{
    ConfigurationLoader loader("conf.txt");
    auto const color = loader.getValueAsArray<float, 3>("my_color");

    if (color) {
        for (float v : *color) {
            std::cout << '"' << v << '"' << std::endl;
        }
    } else {
        std::cout << "Error" << std::endl;
    }

    int array[3];
    loader.tryReadValueIntoArray("my_color", array);
    loader.tryReadValueIntoArray<3>("my_color", array);

    return 0;
}
