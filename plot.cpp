#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <gnuplot-iostream.h>

using namespace std;

int main() {
    // Open the CSV file
    ifstream file("frame_metrics.csv");
    if (!file.is_open()) {
        cerr << "Error: Could not open the file 'frame_metrics.csv'." << endl;
        return 1;
    }

    string line;

    // Skip the header row
    if (!getline(file, line)) {
        cerr << "Error: Could not read the header row." << endl;
        return 1;
    }

    // Initialize vectors to store the data
    vector<int> frames;
    vector<double> psnr, ssim;

    // Read the data from the CSV file
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<string> row;

        while (getline(ss, value, ',')) {
            row.push_back(value);
        }

        // Validate the row size
        if (row.size() != 4) {
            cerr << "Error: Incorrect data format in row: " << line << endl;
            return 1;
        }

        try {
            // We ignore the "Chunk" column (row[0])
            frames.push_back(stoi(row[1])); // Frame
            psnr.push_back(stod(row[2])); // PSNR (dB)
            ssim.push_back(stod(row[3])); // SSIM
        } catch (const invalid_argument& e) {
            cerr << "Error: Invalid data in row: " << line << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "Error: Data out of range in row: " << line << endl;
            return 1;
        }
    }

    file.close();

    // Create a Gnuplot object
    Gnuplot gp;

    // Plot frame vs PSNR
    gp << "set title 'Frame vs PSNR'\n";
    gp << "set xlabel 'Frame'\n";
    gp << "set ylabel 'PSNR (dB)'\n";
    gp << "plot '-' with lines title 'PSNR'\n";
    gp.send1d(boost::make_tuple(frames, psnr));

    // Plot frame vs SSIM
    gp << "set title 'Frame vs SSIM'\n";
    gp << "set xlabel 'Frame'\n";
    gp << "set ylabel 'SSIM'\n";
    gp << "plot '-' with lines title 'SSIM'\n";
    gp.send1d(boost::make_tuple(frames, ssim));

    return 0;
}

