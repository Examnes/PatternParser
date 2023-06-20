#if !defined(EVALUATOR_HH)
#define EVALUATOR_HH

#include <vector>
#include <string>
#include <fstream>
#include "driver.hh"


class RequestResult
{
public:
    RequestResult() = default;
    RequestResult(std::string name, std::string type, std::string value, size_t size) : 
                    m_name(name), 
                    m_type(type), 
                    m_value(value), 
                    m_size(size) {}
    std::string name() const { return m_name; }
    std::string type() const { return m_type; }
    std::string value() const { return m_value; }
    size_t size() const { return m_size; }
    std::string m_name;
    std::string m_type;
    std::string m_value;
    size_t m_size;
    size_t m_array_size;
private:
    
};

class Evaluator
{
public:
    Evaluator(): m_driver() {}

    void load_from_binary_file(std::string filename)
    {
        std::ifstream file(filename);
        if (file.is_open())
        {
            file.seekg(0, std::ios::end);
            std::streampos length = file.tellg();
            file.seekg(0, std::ios::beg);
            m_data.resize(length);
            file.read(&m_data[0], length);
            file.close();
        }else
        {
            throw std::runtime_error("Could not open file " + filename);
        }
    }

    void load_from_hex_file(std::string filename)
    {
        std::ifstream file(filename);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                for (size_t i = 0; i < line.length(); i += 2)
                {
                    std::string byteString = line.substr(i, 2);
                    char byte = (char) strtol(byteString.c_str(), NULL, 16);
                    m_data.push_back(byte);
                }
            }
            file.close();
        }else
        {
            throw std::runtime_error("Could not open file " + filename);
        }
    }

    void parse_template(std::string filename)
    {
        m_driver.parse_file(filename);
    }

    RequestResult evaluate(std::string request)
    {
        RequestExpression* requestExpression = m_driver.execute_request(request);
        long offset = get_data_location(requestExpression->m_access, 0);
        FieldExpression* accessed_field = (FieldExpression*)requestExpression->m_access->get_top_field();
        TypeExpression* type = (TypeExpression*)accessed_field->field_type();
        RequestResult result = RequestResult();
        result.m_name = requestExpression->m_access->get_top_field()->name();
        result.m_type = type->name();
        //result.m_value = get_data_by_size(offset, result.m_size);
        result.m_array_size = 1;
        if (accessed_field->is_array())
        {
            AccessExpression* size_access = (AccessExpression*)accessed_field->m_size;
            if (size_access->is_index())
            {
                result.m_array_size = size_access->m_index;
            }
            else
            {
                long elements_count_location = get_data_location(size_access, 0);
                result.m_array_size = get_data_by_size(elements_count_location, 
                    get_primitive_size((TypeExpression*)size_access->get_top_field()->field_type()));
            }
        }
        result.m_size = get_primitive_size(type) * result.m_array_size;
        if (result.m_size == 0)
        {
            for (size_t i = 0; i < result.m_array_size; i++)
            {
                result.m_size += get_size((StructureExpression*)type->structure(), offset + result.m_size);
            }
        }
        return result;
    }

    long get_data_location(AccessExpression* access, long base_offset)
    {
        long offset = base_offset;
        FieldExpression* accessed_field = (FieldExpression*)access->m_field;
        StructureExpression* structure = (StructureExpression*)access->m_structure;
        for (auto field : structure->m_fields)
        {
            if (accessed_field->name() == field->name())
            {
                break;
            }
            TypeExpression* type = (TypeExpression*)field->field_type();
            int elements_count = 1;
            if (field->is_array())
            {
                AccessExpression* size_access = (AccessExpression*)field->m_size;
                if (size_access->is_index())
                {
                    elements_count = size_access->m_index;
                }
                else
                {
                    long elements_count_location = get_data_location(size_access, base_offset);
                    int index_size = get_primitive_size((TypeExpression*)size_access->get_top_field()->field_type());
                    // get slice
                    elements_count = get_data_by_size(elements_count_location, index_size);
                }
            }               

            for (int i = 0; i < elements_count; i++)
            {
                if (type->is_primitive())
                {
                    offset += get_primitive_size(type);
                }
                else
                {
                    StructureExpression* structure = (StructureExpression*)type->structure();
                    offset += get_size(structure, offset);
                }
            }
        }

        FieldExpression* current_field = accessed_field;
        for (auto chain_link : access->m_chain)
        {
           FieldAccessExpression* field_access = (FieldAccessExpression*)chain_link;
           if (field_access->is_index())
           {
                IndexExpression* index = (IndexExpression*)field_access;
                AccessExpression* index_access = (AccessExpression*)index->m_index;
                long index_value = 0;
                if (index_access->is_index())
                {
                    index_value = index_access->m_index;
                }
                else
                {
                    long index_location = get_data_location(index_access, base_offset);
                    int index_size = get_primitive_size((TypeExpression*)index_access->get_top_field()->field_type());
                    // get slice
                    index_value = get_data_by_size(index_location, index_size);
                }

                for (int i = 0; i < index_value; i++)
                {
                    TypeExpression* type = (TypeExpression*)current_field->field_type();
                    if (type->is_primitive())
                    {
                        offset += get_primitive_size(type);
                    }
                    else
                    {
                        StructureExpression* structure = (StructureExpression*)type->structure();
                        offset += get_size(structure, offset);
                    }
                }
           }
           else if (field_access->is_select())
           {
                SelectExpression* select = (SelectExpression*)field_access;
                FieldExpression* selected_field = (FieldExpression*)select->m_field;
                TypeExpression* type = (TypeExpression*)current_field->field_type();
                for (auto field : ((StructureExpression*)type->structure())->m_fields)
                {
                    if (field->name() == selected_field->name())
                    {
                        break;
                    }
                    long elements_count = 1;
                    if (field->is_array())
                    {
                        AccessExpression* size_access = (AccessExpression*)field->m_size;
                        if (size_access->is_index())
                        {
                            elements_count = size_access->m_index;
                        }
                        else
                        {
                            long elements_count_location = get_data_location(size_access, offset);
                            int index_size = get_primitive_size((TypeExpression*)size_access->get_top_field()->field_type());
                            // get slice
                            elements_count = get_data_by_size(elements_count_location, index_size);
                        }
                    }               

                    for (int i = 0; i < elements_count; i++)
                    {
                        if (((TypeExpression*)field->field_type())->is_primitive())
                        {
                            offset += get_primitive_size(((TypeExpression*)field->field_type()));
                        }
                        else
                        {
                            StructureExpression* structure = (StructureExpression*)((TypeExpression*)field->field_type())->structure();
                            offset += get_size(structure, offset);
                        }
                    }
                }
                current_field = selected_field;
           }
        }
        return offset;
    }

    long get_size(StructureExpression* structure, long current_offset)
    {
        long size = 0;
        for (auto field : structure->m_fields)
        {
            TypeExpression* type = (TypeExpression*)field->field_type();
            int elements_count = 1;
            if (field->is_array())
            {
                AccessExpression* size_access = (AccessExpression*)field->m_size;
                if (size_access->is_index())
                {
                    elements_count = size_access->m_index;
                }
                else
                {
                    long elements_count_location = get_data_location(size_access, current_offset);
                    int index_size = get_primitive_size((TypeExpression*)size_access->get_top_field()->field_type());
                    // get slice
                    elements_count = get_data_by_size(elements_count_location, index_size);
                }
            }

            for (int i = 0; i < elements_count; i++)
            {
                if (type->is_primitive())
                {
                    size += get_primitive_size(type);
                }
                else
                {
                    StructureExpression* structure = (StructureExpression*)type->structure();
                    size += get_size(structure, current_offset + size);
                }
            }
        }
        return size;
    }

    typedef char data_type;
    std::vector<data_type> m_data;
    parse::Driver m_driver;

private:

    int get_primitive_size(TypeExpression* te)
    {
        std::string type = te->name();
        if (type == "int")
        {
            return 4;
        }else if (type == "short")
        {
            return 2;
        }else if (type == "char")
        {
            return 1;
        }else if (type == "long")
        {
            return 8;
        }else if (type == "float")
        {
            return 4;
        }else if (type == "double")
        {
            return 8;
        }

        return 0;
    }

    int get_data_by_size(long offset, int size)
    {
        int value = 0;
        for (int i = 0; i < size; i++)
        {
            value += m_data[offset + i] << (8 * i);
        }

        return value;
    }
    
};

#endif // EVALUATOR_HH
