#include "ConfigBuilder.hpp"

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
    std::vector<std::string> words = split(line.substr(0, line.find('{')), ' ');

    
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
            {
                if (words[i] != "DELETE" && words[i] != "GET" && words[i] != "POST")
                    throw std::runtime_error("Invalid Method Set Up In Location");
                locationConfig.AddAllowMethods(words[i]);
            }
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
            // if (locationConfig.SetMaxBodySize(words[1]) < 0)
            // {
            //     Logger::log(LOG_ERROR, "A location block has invalid max_body_size argument in config.");
            //     locationConfig.SetMaxBodySize(DEFAULT_MAX_BODY_SIZE);
            // }
        }
        if (directiveLocationKey == "allow_delete")
        {
            locationConfig.SetAllowDelete(words[1]);
        }
        if (directiveLocationKey == "allow_download")
        {
            locationConfig.SetAllowDownload(words[1]);
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
        serverConfig.AddLocations(parseLocation(serverConfig, locationBlocks[i]));

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
            /// Allow multiple lines of listen port in config file by appending 
            /// new listen ports to the ServerConfig's listenPort string.
            /// All listen ports in the listenPort string should be seperated by
            /// either a comma, a space or both.
            std::vector<std::string>::iterator  it;
            for (it=(words.begin() + 1); it != words.end(); ++it)
            {
                if (!serverConfig.AddListenPort(*it))
                    throw std::runtime_error("Invalid Port Set Up");
            }
            //serverConfig.SetListenPort(words[1]);
        }
        if (directiveServerKey == "root")
        {
            serverConfig.SetRoot(words[1]);
        }
        if (directiveServerKey == "host")
        {
            serverConfig.SetHostIp(words[1]);
        }
        if (directiveServerKey == "error_page")
        {
            serverConfig.AddError_page(std::atoi(words[1].c_str()), words[2]);
        }
        if (directiveServerKey == "server_name")
        {
            serverConfig.SetServerName(words[1]);
        }
        if (directiveServerKey == "index")
        {
            serverConfig.SetIndexFile(words[1]);
        }
        if (directiveServerKey == "max_body_size")
        {
            serverConfig.SetMaxBodySize(words[1]);
        }
        
    }

    return serverConfig;
}

string ConfigBuilder::readConfigFile(const std::string &filename)
{
    std::string fileContent = "";
    std::ifstream configFile(filename.c_str());

    // The modification made to use direct initialization for the configFile object
    // instead of copy initialization resolves the private copy constructor error.

    if (access(filename.c_str(), F_OK | R_OK) < 0)
    {
        Logger::log(LOG_CRITICAL, std::string("The Server configuration file \"") + filename + "\" does not exist or cannot be read.");
        return ("");
    }
    if (!configFile.is_open())
    {
        throw std::runtime_error("Error opening config file: " + filename);
    }
    std::stringstream buffer;
    buffer << configFile.rdbuf(); // Read file content into buffer
    fileContent = buffer.str();
    configFile.close(); // Close the file
    return fileContent;
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
        std::string configContent = readConfigFile(filename);
        if (configContent.length() == 0)
            return (serverConfigs);
        configContent = ConfigBuilder::cleanComments(configContent);
        configContent = ConfigBuilder::cleanSpaces(configContent);


        std::vector<std::string> serverBlocks = extractServerBlock(configContent);

        for (size_t i = 0; i < serverBlocks.size(); i++)
        {
            serverConfigs.push_back(parseServer(serverBlocks[i]));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return serverConfigs;
}