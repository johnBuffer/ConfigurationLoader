cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME ConfigurationLoader)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE source_files
    "src/*.cpp"
)

set(SOURCES ${source_files})

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
