#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>

// Function to encode binary data to Base64
std::string base64_encode(const std::vector<unsigned char>& data) {
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encoded_data;
    size_t i = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (i < data.size()) {
        char_array_3[0] = data[i++];
        char_array_3[1] = data[i++];
        char_array_3[2] = data[i++];

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (int j = 0; j < 4; j++)
            encoded_data += base64_chars[char_array_4[j]];
    }

    return encoded_data;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_exe_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];

    // Open the input file
    std::ifstream input(input_file, std::ios::binary);
    if (!input) {
        std::cerr << "Error opening input file: " << input_file << std::endl;
        return 1;
    }

    // Get the file size
    input.seekg(0, std::ios::end);
    std::streampos file_size = input.tellg();
    input.seekg(0, std::ios::beg);

    // Read the file content
    std::vector<unsigned char> file_content(file_size);
    input.read(reinterpret_cast<char*>(file_content.data()), file_size);
    input.close();

    // Split the file into even parts
    const size_t num_parts = 5; // Change this to adjust the number of parts
    const size_t part_size = file_size / num_parts;

    for (size_t i = 0; i < num_parts; ++i) {
        size_t start_pos = i * part_size;
        size_t end_pos = (i == num_parts - 1) ? static_cast<size_t>(file_size) : (start_pos + part_size);

        std::vector<unsigned char> part_data(file_content.begin() + start_pos, file_content.begin() + end_pos);
        std::string encoded_data = base64_encode(part_data);

        // Write encoded data to a file
        std::ofstream output("part_" + std::to_string(i) + ".b64");
        output << encoded_data;
        output.close();
    }

    std::cout << "Splitting and encoding completed successfully." << std::endl;

    return 0;
}
