#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "ConfigBuilder.hpp"

class Server {
private:
    int port;
    std::string host;
    std::string root;
    std::string defaultIndex;

public:
    Server(int port, const std::string& host, const std::string& root, const std::string& defaultIndex)
        : port(port), host(host), root(root), defaultIndex(defaultIndex) {}

    void startListening() {
        // Server startup logic
        std::cout << "Server listening on " << host << ":" << port << std::endl;
    }
};

class ServerParser {
private:
    std::string configFile;
    int port;
    std::string host;
    std::string root;
    std::string defaultIndex;

public:
    ServerParser(const std::string& filename) : configFile(filename) {}

    void parse() {
        std::ifstream file(configFile);
        if (!file) {
            std::cerr << "Failed to open file: " << configFile << std::endl;
            return;
        }

        std::string line;
        std::stringstream ss;

        while (std::getline(file, line)) {
            ss.str(line);
            std::string token;
            ss >> token;

            if (token == "server") {
                resetConfig();
            }
            else if (token == "listen") {
                ss >> port;
            }
            else if (token == "host") {
                ss >> host;
            }
            else if (token == "root") {
                ss >> root;
            }
            else if (token == "index") {
                ss >> defaultIndex;
            }

            ss.clear();
        }
    }

    void createServer() const {
        Server server(port, host, root, defaultIndex);
        server.startListening();
    }

private:
    void resetConfig() {
        port = 0;
        host = "";
        root = "";
        defaultIndex = "";
    }
};

int main() {
    ConfigBuilder configBuilder = ConfigBuilder();

    configBuilder.parseConfigFile("config.txt");

    return 0;
}