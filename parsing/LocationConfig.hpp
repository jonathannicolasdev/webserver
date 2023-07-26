#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <iostream>
#include <vector>

class LocationConfig
{
public:
	LocationConfig();
	void print();

	std::string 		GetPath() const;
	void 				SetPath(const std::string& path);

	std::string 		GetIndexFile() const;
	void				SetIndexFile(const std::string& indexFile);
	
	std::string 		GetRoot() const;
	void				SetRoot(const std::string& root);
	
	std::vector<std::string> GetAllowMethods() const;
	void				AddAllowMethods(const std::string& method);
	const std::string&	GetReturnPath() const;
	void 				SetReturnPath(const std::string& returnPath);
	std::string 		GetAutoIndex() const;
	void 				SetAutoIndex(const std::string& autoIndex);
	std::string 		GetMaxBodySize() const;
	void 				SetMaxBodySize(const std::string& maxBodySize);
	std::string 		GetAllowDelete() const;
	void 				SetAllowDelete(const std::string& allowDelete);
	std::string 		GetUpload() const;
	void 				SetUpload(const std::string& upload);

	void 						AddCgiPath(const std::string& cgiPath);
	std::vector<std::string>	GetCgiPaths() const;
	std::vector<std::string> 	GetCgiExts() const;
	void 						AddCgiExt(const std::string& cgiExt);

private:
	std::string path;
	std::vector<std::string> allowMethods;
	std::string autoIndex;
	std::string indexFile;
	std::string returnPath;
	std::string root;
	std::vector<std::string> cgiPaths;
	std::vector<std::string> cgiExts;
	std::string maxBodySize; // max_body_size
	std::string upload;
	std::string allowDelete; // allow_delete on;

	// limit_except, auth
};

#endif