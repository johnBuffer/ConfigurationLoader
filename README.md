# Configuration Loader

Very simple configuration file parser written in C++.

# Installation

This project is a header only lib depending on the standard lib.
Just place the `include/configuration_loader.hpp` file at the desired place in your project and include it.

# Usage

## The configuration file
First create a **configuration file**.
The format is very simple and straightforward, here is an example
```ini
# This line is a comment, it will be ignored
this_is_a_key = this_is_a_value

# Here are some example of valid numerical values
value_int_1 = 1
value_int_2 = -3
value_float_1 = 1
value_float_2 = 1.234
value_float_3 = 1.f
value_bool = 1

# Sequences are supported
sequence_1 = 1, 2, 3, 4 # A sequence is a some coma separated values
sequence_2 = 1, 2, LOL # This will not parse, the values have to be of convertible to same type
sequence_3 = 1, 2.0, 3.3 #
```

## Load the file
Then, in the code we need create a loader and read the file
```cpp
cload::ConfigurationLoader loader;
loader.loadFromFile("conf.txt");
```
or directly
```cpp
cload::ConfigurationLoader loader("conf.txt");
```
We can check that the file has been correctly loaded
```cpp
// Ensure the file has correctly been loaded
if (!loader) {
    std::cout << "The file could not be loaded" << std::endl;
}
```

## Access the values
We can now access the **values** using **keys**.

Several ways are possible
```cpp
// Load a value
auto const int_1 = loader.tryGetValueAs<int32_t>("value_int_1");
if (int_1) {
    std::cout << *int_1 << std::endl; // 1
}
auto const int_2 = loader.tryGetValueAs<int32_t>("value_int_2");
if (int_2) {
    std::cout << *int_2 << std::endl; // -3
}

auto const uint_1 = loader.tryGetValueAs<uint32_t>("value_int_2");
if (uint_1) {
    std::cout << *uint_1 << std::endl;
} else {
    std::cout << "parse error" << std::endl; // Parse error, -3 is not supported by uint32_t
}

auto const float_1 = loader.tryGetValueAs<float>("value_float_3");
if (float_1) {
    std::cout << *float_1 << std::endl; // 1
}
```

All functions return `std::optional`, so it is easy to check that the value has been corretly loaded and parsed.

If the optional has no value, it can mean 3 things
 - The **key** was not found in the configuration
 - The **value** could not be parsed into the specified type
 - The **value** does not fit into the specified type (too big or too small)

It is possible to load values directly into existing variables.

In this case, the functions returns a **boolean** that tells if the operation was successful or not
```cpp
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
```

## Sequences
It is also possible to associate a **sequence of values** with a **key**

```cpp
// Load sequence as an array of int
auto const sequence_1 = loader.tryGetSequenceAsArray<int, 3>("sequence_3");
if (sequence_1) {
    std::cout << sequence_1->size() << std::endl; // 3
}

auto const sequence_2 = loader.tryGetSequenceAsArray<int, 3>("sequence_1");
if (sequence_2) {
    std::cout << sequence_2->size() << std::endl; // 3, only the first 3 elements of the sequence are loaded
}

auto const sequence_3 = loader.tryGetSequence<int>("sequence_1");
if (sequence_3) {
    std::cout << sequence_3->size() << std::endl; // 4, all elements are loaded
}
```

As for **values**, **sequences** can be loaded directly into variables
```cpp
// Read into array
float vec3[3];
if (!loader.tryReadSequenceIntoArray("sequence_3", vec3)) {
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
if (!loader.tryReadSequenceIntoArray<3>("sequence_3", &a_vec.x)) {
    std::cout << "Could not read the value" << std::endl;
} else {
    std::cout << a_vec.x << " " << a_vec.y << " " << a_vec.z << std::endl; // 1 2 3.3
}
```
