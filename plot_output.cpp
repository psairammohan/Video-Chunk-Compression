#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <gnuplot-iostream.h>

using namespace std;

int main() {
    // Open the CSV file
    ifstream file("output.csv");
    string line;

    // Skip the header row
    getline(file, line);

    // Initialize vectors to store the data
    vector<int> frames;
    vector<double> ssim;

    // Read the data from the CSV file
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<string> row;

        while (getline(ss, value, ',')) {
            row.push_back(value);
        }

        frames.push_back(stoi(row[0]));
        ssim.push_back(stod(row[2]));
    }

    file.close();

    // Create a Gnuplot object
    Gnuplot gp;

    // Set the output to a PNG file
    gp << "set terminal pngcairo size 800,600\n";
    gp << "set output 'frame_vs_ssim.png'\n";

    // Plot frame vs SSIM
    gp << "set title 'Frame vs SSIM'\n";
    gp << "set xlabel 'Frame'\n";
    gp << "set ylabel 'SSIM'\n";
    gp << "plot '-' with lines title 'SSIM'\n";
    gp.send1d(boost::make_tuple(frames, ssim));

    return 0;
}

