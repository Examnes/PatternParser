#if !defined(DRIVER_HH)
#define DRIVER_HH

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "ast.hh"

namespace parse
{
    class Parser;
    class Scanner;
    class location;

    class Driver
    {
    public:
        Driver();
        virtual ~Driver();

        int parse();
        int parse_file(std::string& filename);
        int parse_string(std::string& input);
        void reset();

        void set_result(std::vector<StructureExpression*> result)
        {
            m_result = result;
        }

        void add_type(std::string name)
        {
            m_types.push_back(new TypeExpression(name, m_current_structure));
        }

        bool contains_type(std::string name)
        {
            for (auto type : m_types)
            {
                if (type->name() == name)
                {
                    return true;
                }
            }
            return false;
        }

        void set_current_structure(StructureExpression* structure)
        {
            m_current_structure = structure;
        }

        bool contains_field(std::string name)
        {
            return m_current_structure->has_field(name);
        }

        FieldExpression* get_field(std::string name)
        {
            return m_current_structure->get_field(name);
        }

        TypeExpression* get_type(std::string name)
        {
            for (auto type : m_types)
            {
                if (type->name() == name)
                {
                    return type;
                }
            }
            return nullptr;
        }

        StructureExpression* get_current_structure()
        {
            return m_current_structure;
        }


        StructureExpression* push_type(std::string* name)
        {
            m_types.push_back(new TypeExpression(*name, m_current_structure));
            return m_current_structure;
        }

        std::vector<FieldExpression*>* initalize_current_structure(FieldExpression* field)
        {
            m_current_structure = new StructureExpression();
            m_current_structure->add_field(field);
            return m_current_structure->fields();
        }

        std::vector<FieldExpression*>* add_field(FieldExpression* field)
        {
            m_current_structure->add_field(field);
            return m_current_structure->fields();
        }

        FieldExpression* create_field(std::string* name, TypeExpression* type)
        {
            return new FieldExpression(*name, type);
        }
        
        FieldExpression* create_field(std::string* name, TypeExpression* type, Expression* size)
        {
            return new FieldExpression(*name, type, size);
        }

        FieldExpression* create_field(std::string* name, std::string* type)
        {
            return new FieldExpression(*name, get_type(*type));
        }

        FieldExpression* create_field(std::string* name, std::string* type, Expression* size)
        {
            return new FieldExpression(*name, get_type(*type), size);
        }

        AccessExpression* initalize_access(std::string* fieldname)
        {
            AccessExpression* m_current_access = new AccessExpression(m_current_structure->get_field(*fieldname), m_current_structure);
            return m_current_access;
        }

        AccessExpression* add_selector(Expression* access,std::string* fieldname)
        {
            AccessExpression* a = dynamic_cast<AccessExpression*>(access);
            a->add_access(new SelectExpression(a->get_top_field(), *fieldname));
            return a;
        }

        AccessExpression* add_index(Expression* access,Expression* index)
        {
            AccessExpression* a = dynamic_cast<AccessExpression*>(access);
            AccessExpression* b = dynamic_cast<AccessExpression*>(index);
            a->add_access(new IndexExpression(a->get_top_field(), b));
            return a;
        }

        AccessExpression* create_number(int* number)
        {
            return new AccessExpression(*number);
        }

        AccessExpression* add_index(Expression* access,int* index)
        {
            AccessExpression* a = dynamic_cast<AccessExpression*>(access);
            a->add_access(new IndexExpression(a->get_top_field(), new AccessExpression(*index)));
            return a;
        }

        RequestExpression* create_request(std::string* structure_name)
        {
            m_current_structure = (StructureExpression*)get_type(*structure_name)->structure();
            m_current_request = new RequestExpression(m_current_structure);
            return m_current_request;
        }

        RequestExpression* set_request_access(RequestExpression* request, AccessExpression* access)
        {
            request->set_access(access);
            return request;
        }

        void set_current_request(RequestExpression* request)
        {
            m_current_request = request;
        }

        RequestExpression* execute_request(std::string request_string)
        {
            std::string actual_request = request_string + ";\n";
            parse_string(actual_request);
            return m_current_request;
        }

        std::vector<StructureExpression*> get_result()
        {
            return m_result;
        }

        int _error;

        void print()
        {
            for (auto structure : m_result)
            {
                std::cout << structure->to_string() << std::endl;
            }
        }
    private:

        Parser* m_parser;
        Scanner* m_scanner;
        RequestExpression* m_current_request;
        StructureExpression* m_current_structure;
        std::vector<StructureExpression*> m_result;
        std::vector<TypeExpression*> m_types;
        
        friend class Parser;
        friend class Scanner;
    };
}

#endif // DRIVER_HH
