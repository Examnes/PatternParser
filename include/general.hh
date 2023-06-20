#if !defined(AST_GENERAL_HH)
#define AST_GENERAL_HH

#include "types.hh"

class Program : public Expression
{
public:
    Program() = default;
    Program(std::vector<Expression*> types) : m_types(types) {}
    virtual ~Program() = default;
    virtual ExpressionType type() const override { return ExpressionType::Program; }
    virtual std::string to_string() const override 
    { 
        std::stringstream ss;
        ss << "{";
        ss << "\"type\": " << "\"Program\"" << ", ";
        ss << "\"types\": [";
        for (auto type : m_types)
        {
            ss << type->to_string() << ", ";
        }
        ss.seekp(-2, ss.cur);
        ss << "]";
        ss << "}";
        return ss.str(); 
    }
    std::vector<Expression*> types() const { return m_types; }
private:
    std::vector<Expression*> m_types;
};

#endif // AST_GENERAL_HH
