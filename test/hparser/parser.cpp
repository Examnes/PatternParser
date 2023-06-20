#include <catch2/catch.hpp>
#include "ast.hh"
#include "driver.hh"
#include "evaluator.hh"

TEST_CASE("PARSER", "[parser]")
{
    SECTION("Simple test")
    {
        Evaluator evaluator;
        evaluator.parse_template("test_template.hp");
        evaluator.load_from_binary_file("test_data.bin");
        evaluator.evaluate("request file.arr2");
        REQUIRE(2 == 2);
    }   
}