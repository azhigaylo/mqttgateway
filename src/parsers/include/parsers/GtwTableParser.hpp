#pragma once

#include <string>


namespace Parsers
{

class CGtwTableParser final
{
    public:
        explicit CGtwTableParser(const std::string& table_path);
        ~CGtwTableParser();

    private:

        CGtwTableParser(const CGtwTableParser&) = delete;
        CGtwTableParser& operator=(const CGtwTableParser&) = delete;

};

} //namespace Parsers

