#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>

// Function to decode a Base64 string
std::vector<unsigned char> base64_decode(const std::string& encoded_string) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::vector<unsigned char> decoded_data;
    size_t in_len = encoded_string.size();
    size_t i = 0;

    while (in_len && encoded_string[in_len - 1] == '=')
        in_len--;

    unsigned char char_array_4[4], char_array_3{ 0 };

    while (i < in_len) {
        for (size_t k = 0; k < 4 && i < in_len; ++k) {
            while (i < in_len && !isalnum(encoded_string[i]) && encoded_string[i] != '+' && encoded_string[i] != '/') {
                i++;
            }
            char_array_4[k] = static_cast<unsigned char>(base64_chars.find(encoded_string[i]));
            i++;
        }

        char_array_3 = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        decoded_data.push_back(char_array_3);
        if (char_array_4[2] != 64) {
            char_array_3 = ((char_array_4[1] & 0x0F) << 4) + ((char_array_4[2] & 0x3C) >> 2);
            decoded_data.push_back(char_array_3);
            if (char_array_4[3] != 64) {
                char_array_3 = ((char_array_4[2] & 0x03) << 6) + char_array_4[3];
                decoded_data.push_back(char_array_3);
            }
        }
    }

    return decoded_data;
}

// Find the entry point RVA from the PE header
#ifdef _WIN64  // Targetting 64-bit architecture

DWORD FindEntryPointRVA(const std::vector<unsigned char>& pe_buffer) {
    PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(const_cast<unsigned char*>(pe_buffer.data()));
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Invalid DOS header" << std::endl;
        return 0;
    }

    // Calculate the offset of the NT headers
    DWORD nt_offset = dos_header->e_lfanew;
    if (nt_offset + sizeof(IMAGE_NT_HEADERS64) > pe_buffer.size()) {
        std::cerr << "Invalid NT header offset" << std::endl;
        return 0;
    }

    IMAGE_NT_HEADERS64 nt_headers;
    std::memcpy(&nt_headers, pe_buffer.data() + nt_offset, sizeof(IMAGE_NT_HEADERS64));
    if (nt_headers.Signature != IMAGE_NT_SIGNATURE) {
        std::cerr << "Invalid NT header" << std::endl;
        return 0;
    }

    // Return the entry point RVA
    return nt_headers.OptionalHeader.AddressOfEntryPoint;
}

#else  // Targeting 32-bit architecture

DWORD FindEntryPointRVA(const std::vector<unsigned char>& pe_buffer) {
    PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(const_cast<unsigned char*>(pe_buffer.data()));
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Invalid DOS header" << std::endl;
        return 0;
    }

    // Calculate the offset of the NT headers
    DWORD nt_offset = dos_header->e_lfanew;
    if (nt_offset + sizeof(IMAGE_NT_HEADERS) > pe_buffer.size()) {
        std::cerr << "Invalid NT header offset" << std::endl;
        return 0;
    }

    IMAGE_NT_HEADERS nt_headers;
    std::memcpy(&nt_headers, pe_buffer.data() + nt_offset, sizeof(IMAGE_NT_HEADERS));
    if (nt_headers.Signature != IMAGE_NT_SIGNATURE) {
        std::cerr << "Invalid NT header" << std::endl;
        return 0;
    }

    // Return the entry point RVA
    return nt_headers.OptionalHeader.AddressOfEntryPoint;
}

#endif

// Find files with the naming convention "part_#.b64"
std::vector<std::string> findPartFiles(int num_parts) {
    std::vector<std::string> filenames;
    for (int i = 0; i < num_parts; ++i) {
        std::string filename = "part_" + std::to_string(i) + ".b64";
        std::ifstream file(filename);
        if (file) {
            filenames.push_back(filename);
            file.close();
        }
        else {
            std::cerr << "Error opening file: " << filename << std::endl;
        }
    }
    return filenames;
}

int main() {
    int num_parts;
    std::cout << "Enter the number of parts: ";
    std::cin >> num_parts;

    // Find filenames matching the naming convention
    std::vector<std::string> filenames = findPartFiles(num_parts);

    // Concatenate separate files into a single executable buffer
    std::vector<unsigned char> single_executable;
    for (const auto& filename : filenames) {
        std::ifstream file(filename, std::ios::binary);
        if (file) {
            std::string encoded_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            std::vector<unsigned char> decoded_data = base64_decode(encoded_data);
            single_executable.insert(single_executable.end(), decoded_data.begin(), decoded_data.end());
            file.close();
        }
        else {
            std::cerr << "Error opening file: " << filename << std::endl;
        }
    }

    // Find the entry point RVA of the executable
    DWORD entry_point_rva = FindEntryPointRVA(single_executable);
    if (entry_point_rva == 0) {
        std::cerr << "Failed to find entry point RVA" << std::endl;
        return 1;
    }

    // Calculate the entry point address
    LPVOID entry_point_address = reinterpret_cast<LPVOID>(single_executable.data() + entry_point_rva);

    // Call the entry point function
    using FunctionPtr = void(*)();
    FunctionPtr entry_point = reinterpret_cast<FunctionPtr>(entry_point_address);
    entry_point();

    return 0;
}
