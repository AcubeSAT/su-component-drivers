cmake_minimum_required(VERSION 3.14)
project(Basic_On_Board_Software_Project_Template)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/inc")
# Change the executable's name. It should match the project's name.
add_executable(Basic_On_Board_Software_Project_Template
        src/main.cpp
        src/SampleClass.cpp)

IF (EXISTS "${PROJECT_SOURCE_DIR}/lib/Catch2/CMakeLists.txt")
    # Gather all the .cpp files corresponding to tests
    file(GLOB test_main_SRC "test/*.cpp")
    file(GLOB test_SRC "test/**/*.cpp")

    add_subdirectory(lib/Catch2)
    add_executable(tests ${test_main_SRC} ${test_SRC})
    target_link_libraries(tests Catch2::Catch2)
ENDIF ()