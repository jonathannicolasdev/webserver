void ConfigBuilder::parseConfigFile(const std::string filename)
{
    try
    {
        std::string configContent = readConfigFile(filename);
        configContent = cleanComments(configContent);
        configContent = cleanSpaces(configContent);
        std::cout << "Parsed file:\n" << configContent << std::endl;

        std::istringstream iss(configContent);
        std::string line;
        std::string serverBlockContent;
        bool insideServerBlock = false;

        while (std::getline(iss, line))
        {
            if (line.empty() || line[0] == '#')
            {
                // Skip empty lines and comments
                continue;
            }
            else if (line.find("server {") != std::string::npos)
            {
                // Start of server block
                insideServerBlock = true;
                serverBlockContent += line + "\n";
            }
            else if (line.find("}") != std::string::npos)
            {
                // End of server block
                insideServerBlock = false;
                serverBlockContent += line + "\n";
                // Handle the server block content
                std::cout << "Server block content:\n" << serverBlockContent << std::endl;
                // Clear the server block content for the next block
                serverBlockContent.clear();
            }

            if (insideServerBlock)
            {
                serverBlockContent += line + "\n";
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}