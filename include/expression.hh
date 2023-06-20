#if !defined(EXPRESSION_H)
#define EXPRESSION_H

#include <string>

enum class ExpressionType
{
    None = 0,
    Structure,
    Type,
    Field,
    Access,
    FieldAccess,
    Select,
    Index,
    Program,
    Request
};

class Expression
{
public:
    virtual ~Expression() = default;
    virtual ExpressionType type() const = 0;
    virtual std::string to_string() const = 0;
};

#endif // EXPRESSION_H
