#include "ConfigBuilder.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <fstream>
#include <string>
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
// LocationConfig ConfigBuilder::parseLocation(string content)
// {
//     return LocationConfig();
// }

ServerConfig ConfigBuilder::parseServer(string content)
{
    ServerConfig serverConfig;

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

std::vector<std::string> extractBlock(const std::string &content)
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
/*
std::vector<std::string> ConfigBuilder::extractLocationBlocks(const std::string &content)
{
    std::vector<std::string> locationBlocks;
    std::istringstream iss(content);
    std::string line;
    bool inLocationBlock = false;

    while (std::getline(iss, line))
    {
        // Remove leading and trailing whitespaces from the line
        line = trim(line);

        if (line.empty() || line[0] == '#')
        {
            // Skip empty lines and comments
            continue;
        }
        else if (line.find("location") == 0)
        {
            // Start of location block
            inLocationBlock = true;
            locationBlocks.push_back(line);
        }
        else if (inLocationBlock && line.find("}") != std::string::npos)
        {
            // End of location block
            inLocationBlock = false;
            locationBlocks.back() += "\n" + line; // Add the line with closing curly brace to the last location block
        }
        else if (inLocationBlock)
        {
            // Continue adding lines to the location block
            locationBlocks.back() += "\n" + line;
        }
    }

    return locationBlocks;
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

        std::vector<std::string> serverBlocks = extractBlock(configContent);

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