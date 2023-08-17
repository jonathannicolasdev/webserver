
#include "DataPart.hpp"

DataPart::DataPart(std::string datapart_str)
{
    (void) datapart_str;
    DataPart::_content="";
    DataPart::_ContentDisposition="";
    DataPart::_ContentType="";
    DataPart::_filename="";
    DataPart::_name="";
	std::cout << "DataPart default constructor" << std::endl;
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
        return "";
    }

