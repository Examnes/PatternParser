cmake_minimum_required(VERSION 3.13)
project(hparser VERSION 1.0 LANGUAGES CXX)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)
option(HPARSER_TESTING "Включить модульное тестирование" ON)
option(HPARSER_COVERAGE "Включить измерение покрытия кода тестами" OFF)
option(HPARSER_BUILD_SHARED_LIBS "Собирать библиотеку как shared" OFF)
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
)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (HPARSER_BUILD_SHARED_LIBS)
    add_library(hparser SHARED ${SOURCES})
else()
    add_library(hparser ${SOURCES})
endif()

target_include_directories(hparser INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
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
