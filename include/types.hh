#if !defined(AST_TYPES_HH)
#define AST_TYPES_HH

#include "expression.hh"
#include <vector>
#include <sstream>

class FieldExpression : public Expression
{
public:
    FieldExpression()
    {
        m_name = "";
        m_type = nullptr;
        m_size = nullptr;
    }
    FieldExpression(std::string name, Expression* type) : m_name(name), m_type(type), m_size(nullptr) {}
    FieldExpression(std::string name, Expression* type, Expression* size) : m_name(name), m_type(type), m_size(size) {}
    virtual ~FieldExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Field; }
    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "\"type\": " << "\"Field\"" << ", ";
        ss << "\"name\": " << m_name << ", ";
        ss << "\"field_type\": " << m_type->to_string();
        if (m_size != nullptr)
        {
            ss << ", ";
            ss << "\"size\": " << m_size->to_string();
        }
        ss << "}";
        return ss.str(); 
    }
    std::string name() const { return m_name; }
    Expression* field_type() const { return m_type; }
    Expression* size() const { return m_size; }
    bool is_array() const { return m_size != nullptr; }

    std::string m_name;
    Expression* m_type;
    Expression* m_size;
private:
};

class StructureExpression : public Expression
{
public:
    StructureExpression() = default;
    StructureExpression(std::vector<FieldExpression*> fields) : m_fields(fields) {}
    virtual ~StructureExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Structure; }
    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "\"type\": " << "\"Structure\"" << ", ";
        ss << "\"fields\": [";
        for (auto field : m_fields)
        {
            ss << field->to_string() << ", ";
        }
        ss.seekp(-2, ss.cur);
        ss << "]";
        ss << "}";
        return ss.str(); 
    }
    void add_field(FieldExpression* field)
    {
        std::string name = dynamic_cast<FieldExpression*>(field)->name();
        for (auto f : m_fields)
        {
            if (dynamic_cast<FieldExpression*>(f)->name() == name)
            {
                throw std::runtime_error("Field " + name + " already exists in structure");
            }
        }
        m_fields.push_back(field);
    }
    std::vector<FieldExpression*>* fields() { return &m_fields; }

    bool has_field(std::string name) const
    {
        for (auto field : m_fields)
        {
            if (dynamic_cast<FieldExpression*>(field)->name() == name)
            {
                return true;
            }
        }
        return false;
    }

    FieldExpression* get_field(std::string name) const
    {
        for (auto field : m_fields)
        {
            if (dynamic_cast<FieldExpression*>(field)->name() == name)
            {
                return field;
            }
        }
        throw std::runtime_error("Field " + name + " does not exist in structure");
    }
    std::vector<FieldExpression*> m_fields;
private:
    
};

class TypeExpression : public Expression
{
public:
    TypeExpression() = default;
    TypeExpression(std::string name){ m_name = name; m_structure = nullptr; }
    TypeExpression(std::string name, Expression* structure) : m_name(name), m_structure(structure) {}
    virtual ~TypeExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Type; }
    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "\"type\": " << "\"Type\"" << ", ";
        ss << "\"name\": " << m_name;
        if (m_structure)
            ss << ", " << "\"structure\": " << m_structure->to_string();
        ss << "}";
        return ss.str(); 
    }
    std::string name() const { return m_name; }
    Expression* structure() const { return m_structure; }
    bool is_primitive() const { return m_structure == nullptr; }
    static TypeExpression* get_primitive_type(std::string name)
    {
        if (name == "int")
        {
            return new TypeExpression("int");
        }
        else if (name == "char")
        {
            return new TypeExpression("char");
        }
        else if (name == "short")
        {
            return new TypeExpression("short");
        }
        else if (name == "long")
        {
            return new TypeExpression("long");
        }
        else if (name == "float")
        {
            return new TypeExpression("float");
        }
        else if (name == "double")
        {
            return new TypeExpression("double");
        }
        else 
        {
            throw std::runtime_error("Unknown primitive type " + name);
        }
    }
private:
    std::string m_name;
    Expression* m_structure;
};



#endif // AST_TYPES_HH
