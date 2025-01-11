#include "../include/configuration_loader.hpp"


int main()
{
    cload::ConfigurationLoader loader("conf.txt");

    // Load a value
    auto const int_1 = loader.getValueAs<int32_t>("value_int_1");
    if (int_1) {
        std::cout << *int_1 << std::endl; // 1
    }

    auto const int_2 = loader.getValueAs<int32_t>("value_int_2");
    if (int_2) {
        std::cout << *int_2 << std::endl; // -3
    }

    auto const uint_1 = loader.getValueAs<uint32_t>("value_int_2");
    if (uint_1) {
        std::cout << *uint_1 << std::endl;
    } else {
        std::cout << "parse error" << std::endl; // Parse error, -3 is not supported by uint32_t
    }

    auto const float_1 = loader.getValueAs<float>("value_float_3");
    if (float_1) {
        std::cout << *float_1 << std::endl; // 1
    }

    // Load sequence as an array of int
    auto const sequence_1 = loader.getValueAsArray<int, 3>("sequence_3");
    if (sequence_1) {
        std::cout << sequence_1->size() << std::endl; // 3
    }

    auto const sequence_2 = loader.getValueAsArray<int, 3>("sequence_1");
    if (sequence_2) {
        std::cout << sequence_2->size() << std::endl; // 3, only the first 3 elements of the sequence are loaded
    }

    auto const sequence_3 = loader.getValueAsVector<int>("sequence_1");
    if (sequence_3) {
        std::cout << sequence_3->size() << std::endl; // 4, all elements are loaded
    }

    // Load values directly into existing variables
    float float_2;
    if (!loader.tryReadValueInto("value_float_2", &float_2)) {
        std::cout << "Could not read the value" << std::endl;
    } else {
        std::cout << float_2 << std::endl;
    }

    int32_t int_4;
    if (!loader.tryReadValueInto("value_float_2", &int_4)) {
        std::cout << "Could not read the value" << std::endl;
    } else {
        std::cout << int_4 << std::endl; // 1, the value is truncated
    }

    bool bool_1;
    if (!loader.tryReadValueInto("value_bool", &bool_1)) {
        std::cout << "Could not read the value" << std::endl;
    } else {
        std::cout << bool_1 << std::endl; // 1
    }

    bool bool_2;
    if (!loader.tryReadValueInto("value_int_2", &bool_2)) {
        std::cout << "Could not read the value" << std::endl; // -3 is not supported by bool
    } else {
        std::cout << bool_2 << std::endl;
    }

    // Read into array
    float vec3[3];
    if (!loader.tryReadValueIntoArray("sequence_3", vec3)) {
        std::cout << "Could not read the value" << std::endl;
    } else {
        std::cout << vec3[0] << " " << vec3[1] << " " << vec3[2] << std::endl; // 1 2 3.3
    }

    // A bit hacky but it works
    struct Vec3
    {
        float x, y, z;
    };

    Vec3 a_vec;
    if (!loader.tryReadValueIntoArray<3>("sequence_3", &a_vec.x)) {
        std::cout << "Could not read the value" << std::endl;
    } else {
        std::cout << a_vec.x << " " << a_vec.y << " " << a_vec.z << std::endl; // 1 2 3.3
    }

    return 0;
}
