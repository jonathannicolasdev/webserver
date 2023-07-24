#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cctype> // For isspace function
/*
std::string removeDuplicateSpaces(const std::string &input)
{
    std::string result;
    bool previousSpace = false;

    for (char c : input)
    {
        if (!std::isspace(c))
        {
            result += c;
            previousSpace = false;
        }
        else if (!previousSpace)
        {
            result += c;
            previousSpace = true;
        }
    }
    return (result);
}
*/
/*
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
*/
std::vector<std::string> split(const std::string& input, char delimeter)
{
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimeter)) 
    {
        result.push_back(item);
    }
    return (result);
}
/*
std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}
*/
std::string trim(const std::string &str)
{
    std::string::size_type start = str.find_first_not_of(" \t\n\r\f\v");
    if  (start == std::string::npos)
    {
        return ("");
    }
    std::string::size_type end = str.find_last_not_of(" \t\n\r\f\v");
    return (str.substr(start, end - start + 1));
}
/*
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
*/
ConfigBuilder::ConfigBuilder()
{
}
std::string ConfigBuilder::cleanSpaces(const std::string &content)
{
    std::stringstream buffer;
    std::istringstream iss(content);

    std::string line;
    while (std::getline(iss, line))
    {
        line = trim(line);
        std::string cleanedLine;
        bool previousSpace = false;
        for (std::size_t i = 0; i < line.length(); ++i)
        {
            if (!std::isspace(line[i]))
            {
                cleanedLine += line[i];
                previousSpace = false;
            }
            else if (!previousSpace)
            {
                cleanedLine += line[i];
                previousSpace = true;
            }
        }
        if (!cleanedLine.empty())
        {
            buffer << cleanedLine << "\n";
        }
    }
    return buffer.str();
}
/*
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
        buffer << cleanedLine << "\n";
    }
    return buffer.str();
}
*/
std::string ConfigBuilder::cleanComments(const std::string &content)
{
    std::stringstream buffer;
    std::istringstream iss(content);
    std::string line;

    while (std::getline(iss, line))
    {
        std::string::size_type hasPos = line.find('#');
        if (hasPos != std::string::npos)
        {
            line = line.substr(0, hasPos);
        }
    }
    line = trim(line);
    if (!line.empty())
    {
        buffer << line << "\n";
    }
    return (buffer.str());
}
/*
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
*/
LocationConfig ConfigBuilder::parseLocation(string content)
{
    return LocationConfig();
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
                server_directives += line+"\n";
        }
    }
    locationBlocks.push_back(server_directives);
    return locationBlocks;
}

ServerConfig ConfigBuilder::parseServer(string content)
{
    ServerConfig serverConfig;
    std::vector<LocationConfig> locationConfigs;
    std::vector<std::string> locationBlocks;
    std::string serverDirectives;

    locationBlocks = extractLocationBlock(content);
    serverDirectives = locationBlocks[locationBlocks.size() - 1];
    locationBlocks.pop_back();

    for (int i = 0; i < locationBlocks.size(); i++)
    {
       // std::cout << "LOCATION: " << locationBlocks[i] << "\n";
        locationConfigs.push_back(parseLocation(locationBlocks[i]));
    }

    //std::cout << "SERVER: " << serverDirectives;
    std::istringstream iss(serverDirectives);
    std::string line;
    std::getline(iss, line); //purge first ligne server {
    while (std::getline(iss, line))
    {
        if (line[0]=='}')
            break;
        std::vector<std::string> words = split(line, ' ');
        std:string directiveServerKey = words[0];
        if (directiveServerKey=="listen"){
            serverConfig.SetListenPort(std::atoi(words[1].c_str()));
        }
    }

    return serverConfig;
}

string ConfigBuilder::readConfigFile(const std::string &filename)
{
    std::string fileContent = "";
    std::ifstream configFile(filename);
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
    std::string::size_type pos = content.find("server {", 0);
    std::string::size_type foundPos;

    while ((foundPos = content.find("server {", pos + 8)) != std::string::npos)
    {
        serverBlocks.push_back(content.substr(pos, (foundPos - pos)));
        pos = foundPos + 1;
    }
    serverBlocks.push_back(content.substr(pos));
    return (serverBlocks);
}

/*
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
*/
std::vector<ServerConfig> ConfigBuilder::parseConfigFile(const std::string filename)
{
    std::vector<ServerConfig> serverConfigs;

    try
    {
        string configContent = readConfigFile(filename);
        configContent = ConfigBuilder::cleanComments(configContent);
        configContent = ConfigBuilder::cleanSpaces(configContent);

        std::vector<std::string> serverBlocks = extractServerBlock(configContent);

        for (int i = 0; i < serverBlocks.size(); i++)
        {
            std::cout << "SERVER: " << serverBlocks[i] << "\n";
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