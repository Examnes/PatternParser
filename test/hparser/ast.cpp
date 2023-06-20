#include <catch2/catch.hpp>
#include "ast.hh"

TEST_CASE("AST", "[ast]")
{
    SECTION("Type")
    {
        TypeExpression* type = new TypeExpression("int");
        REQUIRE(type->name() == "int");
        REQUIRE(type->is_primitive());
        REQUIRE(type->structure() == nullptr);
        REQUIRE(type->to_string() == "{\"type\": \"Type\", \"name\": int}");
    }

    SECTION("Structure")
    {
        TypeExpression* type = new TypeExpression("int");
        FieldExpression* field = new FieldExpression("x", type);
        StructureExpression* structure = new StructureExpression();
        structure->add_field(field);
        REQUIRE((*structure->fields()).size() == 1);
        REQUIRE((*structure->fields())[0] == field);
        REQUIRE(structure->to_string() == "{\"type\": \"Structure\", \"fields\": [{\"type\": \"Field\", \"name\": x, \"field_type\": {\"type\": \"Type\", \"name\": int}}]}");
        TypeExpression* non_primitive_type = new TypeExpression("struct", structure);
        REQUIRE(non_primitive_type->name() == "struct");
        REQUIRE(!non_primitive_type->is_primitive());
        REQUIRE(non_primitive_type->structure() == structure);
        REQUIRE(non_primitive_type->to_string() == "{\"type\": \"Type\", \"name\": struct, \"structure\": {\"type\": \"Structure\", \"fields\": [{\"type\": \"Field\", \"name\": x, \"field_type\": {\"type\": \"Type\", \"name\": int}}]}}");
    }

    SECTION("Field")
    {
        TypeExpression* type = new TypeExpression("int");
        FieldExpression* field = new FieldExpression("x", type);
        REQUIRE(field->name() == "x");
        REQUIRE(field->field_type() == type);
        REQUIRE(field->to_string() == "{\"type\": \"Field\", \"name\": x, \"field_type\": {\"type\": \"Type\", \"name\": int}}");
    }

    SECTION("Access")
    {
        StructureExpression* structure = new StructureExpression();
        TypeExpression* type_int = new TypeExpression("int");
        FieldExpression* field_x = new FieldExpression("x", type_int);
        structure->add_field(field_x);
        AccessExpression* access = new AccessExpression(field_x, structure);
        REQUIRE(!access->is_index());
        REQUIRE(access->is_numeric());
        REQUIRE(access->get_top_field() == field_x);

        TypeExpression* type_struct = new TypeExpression("getx", structure);
        FieldExpression* field_getx = new FieldExpression("getx", type_struct);
        StructureExpression* structure2 = new StructureExpression();
        structure2->add_field(field_getx);
        AccessExpression* access2 = new AccessExpression(field_getx, structure2);
        SelectExpression* select = new SelectExpression(field_getx, "x");
        access2->add_access(select);
        REQUIRE(!access2->is_index());
        REQUIRE(access2->is_numeric());
        REQUIRE(access2->get_top_field() == field_x);
        Access* test_access = access2->get_access();
        REQUIRE(test_access->field() == field_getx);
        REQUIRE(test_access->chain().size() == 1);
        REQUIRE(test_access->chain()[0] == select);
    }

    SECTION("Complex struct")
    {
        StructureExpression* structure = new StructureExpression();
        FieldExpression* field_x = new FieldExpression("x", TypeExpression::get_primitive_type("int"));
        structure->add_field(field_x);
        AccessExpression* access = new AccessExpression(field_x, structure);
        FieldExpression* field_arr = new FieldExpression("arr", TypeExpression::get_primitive_type("int"), access);
        structure->add_field(field_arr);
        TypeExpression* type_struct = new TypeExpression("size", structure);
        FieldExpression* field_size = new FieldExpression("size", type_struct);
        StructureExpression* structure2 = new StructureExpression();
        structure2->add_field(field_size);
        AccessExpression* access2 = new AccessExpression(field_size, structure2);
        SelectExpression* select = new SelectExpression(access2->get_top_field(), "arr");
        access2->add_access(select);
        AccessExpression* access3 = new AccessExpression(0);
        IndexExpression* index = new IndexExpression(access2->get_top_field(), access3);
        access2->add_access(index);
        FieldExpression* int_arr = new FieldExpression("arr", TypeExpression::get_primitive_type("int"),access2);
        structure2->add_field(int_arr);


        REQUIRE((*structure2->fields()).size() == 2);
        REQUIRE((*structure2->fields())[0] == field_size);
        REQUIRE((*structure2->fields())[1] == int_arr);

        REQUIRE(access2->is_numeric());
        REQUIRE(access3->is_index());
        REQUIRE(access3->is_numeric());
        REQUIRE(access2->get_top_field() == field_arr);
        REQUIRE(access2->get_access()->chain().size() == 2);
        REQUIRE(access2->get_access()->chain()[0] == select);
        REQUIRE(access2->get_access()->chain()[1] == index);
        REQUIRE(access2->get_access()->field() == field_size);
    }
}