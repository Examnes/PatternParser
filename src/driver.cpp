#include "driver.hh"
#include "parser.hpp"
#include "scanner.hh"
#include <fstream>
#include <sstream>


namespace parse
{
    Driver::Driver():
        m_scanner(new Scanner()),
        m_parser(new Parser(*this))
    {
        m_types.push_back(new TypeExpression("int"));
        m_types.push_back(new TypeExpression("char"));
        m_types.push_back(new TypeExpression("short"));
        m_types.push_back(new TypeExpression("long"));
        m_types.push_back(new TypeExpression("float"));
        m_types.push_back(new TypeExpression("double"));
        _error = 0;
    }

    Driver::~Driver()
    {
        delete m_parser;
        delete m_scanner;
    }

    void Driver::reset()
    {
        _error = 0;
    }
    
    int Driver::parse()
    {
        m_scanner->switch_streams(&std::cin, &std::cerr);
        m_parser->parse();
        return _error;
    }

    int Driver::parse_file(std::string& filename)
    {
        std::ifstream in(filename, std::ifstream::in);
        if (!in.good())
        {
            return 1;
        }
        m_scanner->switch_streams(&in, &std::cerr);
        m_parser->parse();
        in.close();
        return _error;
    }

    int Driver::parse_string(std::string& input)
    {
        std::istringstream iss(input);
        m_scanner->switch_streams(&iss, &std::cerr);
        m_parser->parse();
        return _error;
    }
}