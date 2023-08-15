#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cctype> // For isspace function

std::string removeDuplicateSpaces(const std::string &input)
{
    std::string result;
    bool previousSpace = false;

    for (size_t i = 0; i < input.length(); ++i)
    {
        if (!isspace(input[i]))
        {
            // If the current character is not a space, append it to the result string
            result += input[i];
            previousSpace = false;
        }
        else
        {
            // If the current character is a space
            if (!previousSpace)
            {
                // If the previous character was not a space, append it to the result string
                result += input[i];
            }
            previousSpace = true;
        }
    }
    return result;
}

std::vector<std::string> split(const std::string &input, char delimiter)
{
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter))
    {
        result.push_back(token);
    }

    return result;
}

std::string trim(const std::string &str)
{
    // Find the first non-whitespace character
    size_t start = str.find_first_not_of(" \t\n\r\f\v");

    // Find the last non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r\f\v");

    if (start == std::string::npos)
    {
        // String contains only whitespace characters
        return "";
    }
    // Extract the trimmed substring
    return str.substr(start, end - start + 1);
}

ConfigBuilder::ConfigBuilder()
{
}

string ConfigBuilder::cleanSpaces(const std::string &content)
{
    string line, cleanedLine;
    std::stringstream buffer;
    std::istringstream iss(content);

    while (std::getline(iss, line))
    {
        cleanedLine = trim(line); // netoyer vraiment la ligne
        cleanedLine = removeDuplicateSpaces(cleanedLine);
        if (cleanedLine.empty())
            continue;
        if (cleanedLine[cleanedLine.length() - 1] == ';')
            cleanedLine = cleanedLine.substr(0, cleanedLine.length() - 1);
        buffer << cleanedLine << "\n";
    }
    return buffer.str();
}

string ConfigBuilder::cleanComments(const std::string &content)
{
    string line, cleanedLine;
    std::stringstream buffer;
    std::istringstream iss(content);

    while (std::getline(iss, line))
    {
        if (line[0] == '#')
            continue;
        size_t hashPos = line.find('#');
        if (hashPos != std::string::npos)
        {
            // If the '#' character is found, extract the substring before it
            cleanedLine = line.substr(0, hashPos);
        }
        else
            cleanedLine = line;
        buffer << cleanedLine << "\n";
    }
    return buffer.str();
}
LocationConfig ConfigBuilder::parseLocation(ServerConfig& srv_cfg, const std::string& content)
{
    LocationConfig locationConfig(srv_cfg);

    std::istringstream iss(content);
    std::string line;
    std::getline(iss, line); // purge first ligne server {
    std::vector<std::string> words = split(line, ' ');
    locationConfig.SetPath(words[1]);
    while (std::getline(iss, line))
    {
        if (line[0] == '}')
            break;
        words = split(line, ' ');
        std::string directiveLocationKey = words[0];

        if (directiveLocationKey == "index")
        {
            locationConfig.SetIndexFile(words[1]);
        }
        if (directiveLocationKey == "root")
        {
            locationConfig.SetRoot(words[1]);
        }
        if (directiveLocationKey == "allow_methods")
        {
            for (size_t i = 1; i < words.size(); i++)
                locationConfig.AddAllowMethods(words[i]);
        }
        if (directiveLocationKey == "return")
        {
            locationConfig.SetReturnPath(words[1]);
        }
        if (directiveLocationKey == "autoindex")
        {
            locationConfig.SetAutoIndex(words[1]);
        }
        if (directiveLocationKey == "max_body_size")
        {
            locationConfig.SetMaxBodySize(words[1]);
        }
        if (directiveLocationKey == "allow_delete")
        {
            locationConfig.SetAllowDelete(words[1]);
        }        
        if (directiveLocationKey == "upload")
        {
            locationConfig.SetUpload(words[1]);
        }          
        if (directiveLocationKey == "cgi_path")
        {
            for (size_t i = 1; i < words.size(); i++)
                locationConfig.AddCgiPath(words[i]);
        }
        if (directiveLocationKey == "cgi_ext")
        {
            for (size_t i = 1; i < words.size(); i++)
                locationConfig.AddCgiExt(words[i]);
        }
    }
    return locationConfig;
}

std::vector<std::string> extractLocationBlock(const std::string &content)
{
    std::vector<std::string> locationBlocks;
    std::istringstream iss(content);
    std::string line, currentLocation, server_directives = "";
    bool inLocationBlock = false;

    while (std::getline(iss, line))
    {
        if (inLocationBlock)
        {
            currentLocation += "\n" + line; // Add the line with closing curly brace to the last location block
            if (line[line.length() - 1] == '}')
            {
                // End of location block
                inLocationBlock = false;
                locationBlocks.push_back(currentLocation);
            }
        }
        else
        {
            if (line.substr(0, 8) == "location" && line[line.length() - 1] == '{')
            {
                // Start of location block
                inLocationBlock = true;
                currentLocation = line;
            }
            else
                server_directives += line + "\n";
        }
    }
    locationBlocks.push_back(server_directives);
    return locationBlocks;
}

ServerConfig ConfigBuilder::parseServer(string content)
{
    ServerConfig serverConfig;
    std::vector<std::string> locationBlocks;
    std::string serverDirectives;

    locationBlocks = extractLocationBlock(content);
    serverDirectives = locationBlocks[locationBlocks.size() - 1];
    locationBlocks.pop_back();

    for (size_t i = 0; i < locationBlocks.size(); i++)
    {
        // std::cout << "LOCATION: " << locationBlocks[i] << "\n";
        serverConfig.AddLocations(parseLocation(serverConfig, locationBlocks[i]));
    }

    // std::cout << "SERVER: " << serverDirectives;
    std::istringstream iss(serverDirectives);
    std::string line;
    std::getline(iss, line); // purge first ligne server {
    while (std::getline(iss, line))
    {
        if (line[0] == '}')
            break;
        std::vector<std::string> words = split(line, ' ');
        std::string directiveServerKey = words[0];

        if (directiveServerKey == "listen")
        {
            serverConfig.SetListenPort(words[1]);
            // std::cout << serverConfig.GetListenPort() << ": port\n";
        }
        if (directiveServerKey == "root")
        {
            serverConfig.SetRoot(words[1]);
            // std::cout << serverConfig.GetRoot() << ": root\n";
        }
        if (directiveServerKey == "host")
        {
            serverConfig.SetHostIp(words[1]);
            // std::cout << serverConfig.GetHostIp() << ": host IP\n";
        }
        if (directiveServerKey == "error_page")
        {
            serverConfig.AddError_page(std::atoi(words[1].c_str()), words[2]);
            // std::cout << serverConfig.GetError_pages()[404] << ": error_page\n";
        }
        if (directiveServerKey == "server_name")
        {
            serverConfig.SetServerName(words[1]);
            // std::cout << serverConfig.GetServerName() << ": serverName\n";
        }
        if (directiveServerKey == "index")
        {
            serverConfig.SetIndexFile(words[1]);
            // std::cout << serverConfig.GetServerName() << ": serverName\n";
        }
        if (directiveServerKey == "max_body_size")
        {
            serverConfig.SetMaxBodySize(words[1]);
            // std::cout << serverConfig.GetServerName() << ": serverName\n";
        }
        
    }

    return serverConfig;
}

string ConfigBuilder::readConfigFile(const std::string &filename)
{
    std::string fileContent = "";
    std::ifstream configFile(filename.c_str());
    // std::ifstream configFile = std::ifstream(filename);
    // The modification made to use direct initialization for the configFile object
    // instead of copy initialization resolves the private copy constructor error.
    if (!configFile.is_open())
    {
        throw std::runtime_error("Error opening config file: " + filename);
    }
    std::stringstream buffer;
    buffer << configFile.rdbuf(); // Read file content into buffer
    fileContent = buffer.str();
    configFile.close(); // Close the file
    return fileContent;
    // std::string line;

    // if (!configFile.is_open())
    // {
    //     std::cerr << "Error opening config file: " << filename << std::endl;
    //     return;
    // }

    // while (std::getline(configFile, line))
    // {

    // }
}

std::vector<std::string> extractServerBlock(const std::string &content)
{
    std::vector<std::string> serverBlocks;

    size_t pos = content.find("server {", 0);
    size_t foundPos;

    while ((foundPos = content.find("server {", pos + 8)) != std::string::npos)
    {
        serverBlocks.push_back(content.substr(pos, (foundPos - pos)));
        pos = foundPos; // Move to the next position to search from
    }
    serverBlocks.push_back(content.substr(pos, (content.length() - pos)));

    return serverBlocks;
}

std::vector<ServerConfig> ConfigBuilder::parseConfigFile(const std::string& filename)
{
    std::vector<ServerConfig> serverConfigs;

    try
    {
        string configContent = readConfigFile(filename);
        configContent = ConfigBuilder::cleanComments(configContent);
        configContent = ConfigBuilder::cleanSpaces(configContent);

        std::vector<std::string> serverBlocks = extractServerBlock(configContent);

        for (size_t i = 0; i < serverBlocks.size(); i++)
        {
            // std::cout << "SERVER: " << serverBlocks[i] << "\n";
            serverConfigs.push_back(parseServer(serverBlocks[i]));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return serverConfigs;

    /*
        // Remove leading and trailing whitespaces from the line
        line = trim(line);

        if (line.empty() || line[0] == '#')
        {
            // Skip empty lines and comments
            continue;
        }
        else if (line.find("server {") != std::string::npos)
        {
            // Start of server block
            server = Server();
            continue;
        }
        else if (line.find("}") != std::string::npos)
        {


        // Parse directives within server or location block
        std::istringstream iss(line);
        std::string directive, value;
        std::getline(iss, directive, ' ');
        std::getline(iss, value);

        if (!inLocationBlock)
        {
            // Directives within server block
            if (directive == "listen")
            {
                int port = std::stoi(value);
                setListenPort(port);
            }
            else if (directive == "server_name")
            {
                setServerName(value);
            }
            else if (directive == "host")
            {
                setHost(value);
            }
            else if (directive == "root")
            {
                setRoot(value);
            }
            else if (directive == "index")
            {
                setIndexFile(value);
            }
        }
        else
        {
            // Directives within location block
            if (directive == "location")
            {
                currentLocation.setPath(value);
            }
            else if (directive == "allow_methods")
            {
                std::vector<std::string> methods = split(value, ' ');
                currentLocation.setAllowedMethods(methods);
            }
            else if (directive == "autoindex")
            {
                bool autoindex = (value == "on");
                currentLocation.setAutoindex(autoindex);
            }
            // Add more directives as needed
        }
        configFile.close();
    }*/
}