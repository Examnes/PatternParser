cmake_minimum_required(VERSION 3.13)
project(hparser VERSION 1.0 LANGUAGES CXX)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)
option(HPARSER_TESTING "Включить модульное тестирование" ON)
option(HPARSER_COVERAGE "Включить измерение покрытия кода тестами" OFF)
option(HPARSER_BUILD_SHARED_LIBS "Собирать библиотеку как shared" OFF)

find_package(BISON)
find_package(FLEX)
# turn debug on
set(BISON_DEBUG TRUE)
set(BISON_VERBOSE TRUE)

add_compile_options(
    -Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
    
    -Wno-old-style-cast
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-reorder
    -Wno-unused-variable
    -Wno-unused-parameter
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
bison_target(Parser bnf/parse.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(Scanner bnf/scan.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
add_flex_bison_dependency(Scanner Parser)

set(SOURCES ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})


if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (HPARSER_BUILD_SHARED_LIBS)
    add_library(hparser SHARED ${SOURCES})
else()
    add_library(hparser ${SOURCES})
endif()

target_include_directories(hparser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(hparser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_features(hparser INTERFACE cxx_std_17)

add_library(hparser::hparser ALIAS hparser)

install(DIRECTORY include/ DESTINATION include)

install(TARGETS hparser EXPORT hparserConfig)

install(EXPORT hparserConfig DESTINATION share/hparser/cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/hparserConfigVersion.cmake"
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

install(FILES
    "${PROJECT_BINARY_DIR}/hparserConfigVersion.cmake"
    DESTINATION
        share/hparser/cmake
)

if(NOT HPARSER_TESTING)
    message(STATUS "Тестирование проекта отключено")
elseif(IS_SUBPROJECT)
    message(STATUS "Тестирование проекта отключено для подпроекта")
else()
    add_subdirectory(test)
endif()

if(NOT IS_SUBPROJECT)
    add_subdirectory(doc)
endif()
