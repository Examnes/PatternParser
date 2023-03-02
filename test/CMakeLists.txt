find_package(Catch2 2 REQUIRED)
add_executable(hparser-unit-tests test_main.cpp)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS hparser/*.cpp)
target_sources(hparser-unit-tests PRIVATE ${TEST_SOURCES})
target_link_libraries(hparser-unit-tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain hparser)
add_custom_target(check COMMAND hparser-unit-tests)

find_program(GCOVR_EXECUTABLE gcovr)
if(HPARSER_COVERAGE AND GCOVR_EXECUTABLE)
    message(STATUS "Измерение покрытия кода тестами включено")
    target_compile_options(hparser-unit-tests PRIVATE --coverage)
    target_link_libraries(hparser-unit-tests PRIVATE gcov)
    add_custom_target(coverage
    COMMAND
        ${GCOVR_EXECUTABLE}
            --root=${PROJECT_SOURCE_DIR}/include/
            --object-directory=${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
        check)
elseif(HPARSER_COVERAGE AND NOT GCOVR_EXECUTABLE)
    set(HPARSER_COVERAGE OFF)
    message(WARNING "Для замеров покрытия кода тестами требуется программа gcovr")
endif()



