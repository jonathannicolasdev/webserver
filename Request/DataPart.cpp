
#include "DataPart.hpp"
/*
\r\n
Content-Disposition: form-data; name="description"\r\n\r\n
This is a description for the uploaded files.\r\n
*/

DataPart::DataPart(std::string datapart_str)
{
    std::string contentMarquer = "\r\n\r\n";

    std::cout << "DataPart CONSTRUCTOR" << std::endl;
    size_t contentStart = datapart_str.find(contentMarquer, 0);
    if (contentStart != std::string::npos)
    {
        _error_bad_form = true;
        _header = datapart_str.substr(0, contentStart);
        _content = datapart_str.substr(contentStart + 4);
        //size_t contentStart = datapart_str.find("\n");
    //   size_t contentStart = datapart_str.find_first_of("\r\n");

        std::cout <<"header : " << _header << std::endl;
        std::cout <<"content : " << _content << std::endl;
    //    exit(0);
        size_t contentEnd = datapart_str.length();
        
        std::cout << "||||||||||||||||" << datapart_str <<","<<contentStart<<","<<contentEnd << std::endl;

       // size_t  name_pos = _header.find("filename=\"") + 10;
        size_t  name_pos = _header.find("name=\"") + 6;
        _name = _header.substr(name_pos, _header.find('\"', name_pos) - name_pos);

        std::cout << "name :      " << _name << std::endl;

        name_pos = _header.find("filename=\"") + 10;
        _filename = _header.substr(name_pos, _header.find('\"', name_pos) - name_pos);

        std::cout << "filename :    " << _filename << std::endl;



    //    DataPart::_content = datapart_str.substr(contentStart,(contentEnd - contentStart)+1);

    //    DataPart::_content = "";
        // DataPart::_ContentDisposition = "";
        // DataPart::_ContentType = "";
        // DataPart::_filename = "";
        // DataPart::_name = _header.substr(_header.find("name="));
        std::cout << "||||||||||||||||" << DataPart::getContent()  << std::endl;
    }
    else
        _error_bad_form = false;
}

DataPart::~DataPart()
{
    std::cout << "DataPart Destructor" << std::endl;
}

std::string DataPart::getContentDisposition() const
{
    return _ContentDisposition;
}
std::string DataPart::getName() const
{
    return _name;
}
std::string DataPart::getFilename() const
{
    return _filename;
}
std::string DataPart::getContentType() const
{
    return _ContentType;
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
