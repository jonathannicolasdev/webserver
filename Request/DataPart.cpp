
#include "DataPart.hpp"
/*
\r\n
Content-Disposition: form-data; name="description"\r\n\r\n
This is a description for the uploaded files.\r\n
*/

DataPart::DataPart(std::string datapart_str)
{
    std::string contentMarquer="\r\n";

    size_t contentStart = datapart_str.find(contentMarquer, 0);

    size_t contentEnd = datapart_str.length();
    
    std::cout << "||||||||||||||||" << datapart_str <<","<<contentStart<<","<<contentEnd << std::endl;

    DataPart::_content = datapart_str.substr(contentStart,(contentEnd-contentStart)+1);

    DataPart::_content = "";
    DataPart::_ContentDisposition = "";
    DataPart::_ContentType = "";
    DataPart::_filename = "";
    DataPart::_name = "";
    std::cout << "||||||||||||||||" << DataPart::getContent()  << std::endl;
}

DataPart::~DataPart()
{
    std::cout << "DataPart Destructor" << std::endl;
}

std::string DataPart::getContentDisposition() const
{
    return "";
}
std::string DataPart::getName() const
{
    return "";
}
std::string DataPart::getFilename() const
{
    return "";
}
std::string DataPart::getContentType() const
{
    return "";
}
std::string DataPart::getContent() const
{
    return _content;
}

std::ostream&   operator<<(std::ostream& os, const DataPart& data)
{
    std::cout << "DataPart :" << std::endl;
    std::cout << "name :        " << data.getName() << std::endl;
    std::cout << "filename :    " << data.getFilename() << std::endl;
    std::cout << "ContentType : " << data.getContentType() << std::endl;
    std::cout << "Content :     " <<  data.getContent() << std::endl;
    return (os);
}
