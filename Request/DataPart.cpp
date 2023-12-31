
#include "DataPart.hpp"

DataPart::DataPart(const std::string& datapart_str)
{
	std::string contentMarquer = "\r\n\r\n";

	size_t contentStart = datapart_str.find(contentMarquer, 0);
	if (contentStart != std::string::npos)
	{
		_error_bad_form = true;
		_header = datapart_str.substr(0, contentStart);
		_content = datapart_str.substr(contentStart + 4);
		size_t name_pos = _header.find("name=\"") + 6;
		_name = _header.substr(name_pos, _header.find('\"', name_pos) - name_pos);
		if (_header.find("filename=\"") != std::string::npos)
		{
			name_pos = _header.find("filename=\"") + 10;
			_filename = _header.substr(name_pos, _header.find('\"', name_pos) - name_pos);
		}
		else
			_filename="";
	}
	else
		_error_bad_form = false;
}

DataPart::~DataPart()
{
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

std::ostream &operator<<(std::ostream &os, const DataPart &data)
{
	std::cout << "DataPart :" << std::endl;
	std::cout << "name :        " << data.getName() << std::endl;
	std::cout << "filename :    " << data.getFilename() << std::endl;
	std::cout << "ContentType : " << data.getContentType() << std::endl;
	if (data.getContent().empty())
		std::cout << "Content :     " << "[...]" << std::endl;
	else
		std::cout << "Content :     " << "[EMPTY]" << std::endl;
	return (os);
}
