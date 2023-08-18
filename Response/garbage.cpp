#include <iostream>
#include <string>
#include <fstream>

int main() {
    std::string boundary = "boundary123456789"; // Replace with your actual boundary string

    std::string input; // Read the entire input stream
    char c;
    while (std::cin.get(c)) {
        input += c;
    }

    size_t pos = 0;
    while ((pos = input.find("--" + boundary, pos)) != std::string::npos) {
        pos += boundary.length() + 2; // Move past the boundary and newline

        size_t endPos = input.find("\r\n\r\n", pos);
        if (endPos == std::string::npos) {
            break; // No more valid parts
        }

        std::string headers = input.substr(pos, endPos - pos);
        pos = endPos + 4; // Move past headers and newlines

        endPos = input.find("--" + boundary, pos);
        if (endPos == std::string::npos) {
            break; // No more valid parts
        }

        std::string partContent = input.substr(pos, endPos - pos - 2); // Exclude the final newline
        pos = endPos + boundary.length() + 4; // Move past the part and newlines

        std::cout << "Headers: " << headers << std::endl;
        std::cout << "Content: " << partContent << std::endl;

        // You can further process the headers and content as needed
    }

    return 0;
}


#include <iostream>
#include <vector>
#include <string>

// Function to extract content between boundaries in a multipart/form-data request body
std::vector<std::string> extractContents(const std::string& body, const std::string& boundary) {
    std::vector<std::string> contents;

    size_t startPos = 0;
    size_t endPos;

    while ((endPos = body.find(boundary, startPos)) != std::string::npos) {
        contents.push_back(body.substr(startPos, endPos - startPos));
        startPos = endPos + boundary.length();
    }

    return contents;
}

int main() {
    std::string requestBody = "--boundary123456789\r\n"
                              "Content-Disposition: form-data; name=\"description\"\r\n\r\n"
                         //     "This is a description for the uploaded files.\r\n"
                              "--boundary123456789\r\n"
                              "Content-Disposition: form-data; name=\"file1\"; filename=\"file1.txt\"\r\n"
                              "Content-Type: text/plain\r\n\r\n"
                          //    "...contents of file1.txt...\r\n"
                              "--boundary123456789\r\n"
                              "Content-Disposition: form-data; name=\"file2\"; filename=\"image.jpg\"\r\n"
                              "Content-Type: image/jpeg\r\n\r\n"
                          //    "...contents of image.jpg...\r\n"
                              "--boundary123456789--\r\n";

    std::string boundary = "--boundary123456789";

    std::vector<std::string> extractedContents = extractContents(requestBody, boundary);

    for (size_t i = 0; i < extractedContents.size(); ++i) {
        std::cout << "Part " << i << ":\n" << extractedContents[i] << std::endl;
    }

    return 0;
}


curl -X POST "http://localhost:8080/" \
  -H "Content-Type: multipart/form-data; boundary=boundary123456789" \
  --data-binary @- < body.txt