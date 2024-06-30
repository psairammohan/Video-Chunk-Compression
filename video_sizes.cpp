#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int main() {
    std::uintmax_t totalWebmSizeBytes = 0;
    std::uintmax_t totalMkvSizeBytes = 0;
    
    try {
        for (const auto& entry : fs::directory_iterator(".")) { // Current directory
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.rfind("output_chunk_", 0) == 0 && entry.path().extension() == ".webm") {
                    totalWebmSizeBytes += entry.file_size();
                } else if (filename.rfind("chunk_", 0) == 0 && entry.path().extension() == ".mkv") {
                    totalMkvSizeBytes += entry.file_size();
                }
            }
        }

        double totalWebmSizeMB = totalWebmSizeBytes / (1024.0 * 1024.0);
        double totalMkvSizeMB = totalMkvSizeBytes / (1024.0 * 1024.0);

        std::cout << "Total size of files starting with 'output_chunk_' and ending with '.webm': " 
                  << totalWebmSizeMB << " MB" << std::endl;
        std::cout << "Total size of files starting with 'chunk_' and ending with '.mkv': " 
                  << totalMkvSizeMB << " MB" << std::endl;

        if (totalMkvSizeBytes > 0) {
            double compressionRatio = totalWebmSizeBytes / static_cast<double>(totalMkvSizeBytes);
            std::cout << "Compression ratio (totalWebmSize / totalMkvSize): " << compressionRatio << std::endl;
        } else {
            std::cout << "No .mkv files found, cannot calculate compression ratio." << std::endl;
        }
        
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

