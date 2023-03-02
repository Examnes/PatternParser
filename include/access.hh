#if !defined(AST_ACCESS_HH)
#define AST_ACCESS_HH

#include "expression.hh"
#include "types.hh"

class FieldAccessExpression : public Expression // base class
{
public:
    FieldAccessExpression() = default;
    virtual ~FieldAccessExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::FieldAccess; }
    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "type: " << "FieldAccess" << ", ";
        ss << "field: " << get_field()->to_string();
        ss << "}";
        return ss.str(); 
    }
    virtual FieldExpression* get_field() const = 0;
    virtual bool is_index() const { return false; }
    virtual bool is_select() const { return false; }
};

class Access
{
public:
    Access() = default;
    Access(FieldExpression* field, std::vector<Expression*> chain) : m_field(field), m_chain(chain) {}
    virtual ~Access() = default;
    FieldExpression* field() const { return m_field; }
    std::vector<Expression*> chain() const { return m_chain; }
private:
    FieldExpression* m_field;
    std::vector<Expression*> m_chain;
};

class AccessExpression : public Expression
{
public:
    AccessExpression() = default;
    AccessExpression(Expression* field, Expression* structure) : m_field(field), m_structure(structure) {}
    AccessExpression(int index) : m_index(index) {}
    virtual ~AccessExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Access; }

    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "type: " << "Access" << ", ";
        if (m_field)
        {
            ss << "field: " << m_field->to_string();
        }
        else
        {
            ss << "index: " << m_index;
        }
        ss << "}";
        return ss.str(); 
    }

    void add_access(Expression* accessExpression)
    {
        m_chain.push_back(accessExpression);
    }

    FieldExpression* get_top_field()
    {
        if (m_chain.size() == 0)
        {
            if (m_field)
            {
                return dynamic_cast<FieldExpression*>(m_field);
            }
            throw std::runtime_error("Trying to get access to a number");
        }else
        {
            return dynamic_cast<FieldExpression*>(dynamic_cast<FieldAccessExpression*>(m_chain[0])->get_field());
        }
    }

    bool is_numeric()
    {
        if (is_index())
        {
            return true;
        }
        FieldExpression* field = get_top_field();
        TypeExpression* type = dynamic_cast<TypeExpression*>(field->field_type());
        return type->is_primitive();
    }

    bool is_index()
    {
        return m_index != -1;
    }

    Access* get_access()
    {
        if (is_index())
        {
            throw std::runtime_error("Trying to get access to a number");
        }
        return new Access(dynamic_cast<FieldExpression*>(m_field), m_chain);
    }


private:
    Expression* m_field = nullptr;
    Expression* m_structure = nullptr;
    long m_index = -1;
    std::vector<Expression*> m_chain;
};

class SelectExpression : public FieldAccessExpression
{
public:
    SelectExpression() = default;
    SelectExpression(FieldExpression* field, std::string selected_field)
    {
        TypeExpression* type = dynamic_cast<TypeExpression*>(field->field_type());
        if (type->is_primitive())
        {
            throw std::runtime_error("Trying to select a field from a primitive type");
        }
        StructureExpression* structure = dynamic_cast<StructureExpression*>(type->structure());
        for (auto field_ref : structure->fields())
        {
            FieldExpression* field = dynamic_cast<FieldExpression*>(field_ref);
            if (field->name() == selected_field)
            {
                m_field = field;
                return;
            }
        }
        throw std::runtime_error("Trying to select a field that doesn't exist");
    }
    virtual ~SelectExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Select; }

    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "type: " << "Select" << ", ";
        ss << "field: " << m_field->to_string();
        ss << "}";
        return ss.str(); 
    }
    virtual FieldExpression* get_field() const override { return m_field; }
    virtual bool is_select() const override { return true; }
private:
    FieldExpression* m_field;
};

class IndexExpression : public FieldAccessExpression
{
public:
    IndexExpression() = default;
    IndexExpression(FieldExpression* field, AccessExpression* index)
    {
        TypeExpression* type = dynamic_cast<TypeExpression*>(field->field_type());
        type->is_primitive();
        if (field->is_array())
        {
            m_field = field;
        }
        else
        {
            throw std::runtime_error("Trying to index a non-array field");
        }
        if (index->is_numeric())
        {
            m_index = index;
        }
        else
        {
            throw std::runtime_error("Trying to index with a non-numeric value");
        }
    }
    virtual ~IndexExpression() = default;
    virtual ExpressionType type() const override { return ExpressionType::Index; }

    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "type: " << "Index" << ", ";
        ss << "field: " << m_field->to_string() << ", ";
        ss << "index: " << m_index->to_string();
        ss << "}";
        return ss.str();
    }
    virtual FieldExpression* get_field() const override { return m_field; }
    virtual bool is_index() const override { return true; }
private:
    FieldExpression* m_field;
    AccessExpression* m_index;
};


#endif // AST_ACCESS_HH
