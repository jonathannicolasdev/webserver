
#ifndef DATAPART_HPP
#define DATAPART_HPP

#include <string>

#include "Logger.hpp"

class DataPart
{
private:
    std::string _ContentDisposition;
    std::string _name;
    std::string _filename;
    std::string _ContentType;
    std::string _content;

public:
    DataPart(std::string datapart_str);
    ~DataPart(void);

    std::string getContentDisposition() const;
    std::string getName() const;
    std::string getFilename() const;
    std::string getContentType() const;
    std::string getContent() const;
};

#endif
