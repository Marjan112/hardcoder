#include <iostream>
#include <fstream>
#include <vector>

std::string replace_dot_with_underscore(std::string filename) {
    for(size_t i = 0; i < filename.size(); ++i) if(filename[i] == '.') filename[i] = '_';
    return filename;
}

std::string just_lowercase_everything(std::string str) {
    for(auto& ch : str) ch = tolower(ch);
    return str;
}

int main(int argc, char** argv) {
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " <option> <inputfile>\n";
        std::cout << "Options:\n";
        std::cout << "\tc - Hardcode in C\n";
        std::cout << "\tcxx - Hardcode in C++\n";
        return 1;
    }

    const std::string option = just_lowercase_everything(argv[1]);

    if(option != "c" && option != "cxx") {
        std::cerr << "Invalid option \"" << option << "\"\n";
        return 1;
    }

    const std::string input_filename = argv[2];
    std::ifstream input_file(input_filename, std::ios::binary | std::ios::ate);
    if(input_file.fail()) {
        std::cerr << "Could not open file \"" << input_filename << "\"\n";
        return 1;
    }

    size_t input_file_size = input_file.tellg();
    input_file.seekg(0, std::ios_base::beg);

    std::vector<unsigned char> bytes;
    bytes.resize(input_file_size);
    input_file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    std::string identifier = replace_dot_with_underscore(input_filename);

    std::ofstream output_file(input_filename + ".h");
    output_file << "#ifndef " << identifier << "_H\n";
    output_file << "#define " << identifier << "_H\n";

    if(option == "c") output_file << "const unsigned char " << identifier << "[] = {\n";
    else output_file << "constexpr unsigned char " << identifier << "[] = {\n";
    
    for(size_t i = 0; i < input_file_size; ++i) {
        output_file << "0x" << std::hex << static_cast<int>(bytes[i]);
        if(i != input_file_size - 1) output_file << ", ";
        if((i + 1) % 12 == 0) output_file << "\n";
    }

    output_file << "\n};\n\n" << std::dec;

    if(option == "c") output_file << "const unsigned int " << identifier << "_size = " << input_file_size << ";\n";
    else output_file << "constexpr unsigned int " << identifier << "_size = " << input_file_size << ";\n";
    
    output_file << "#endif\n";
    return 0;
}