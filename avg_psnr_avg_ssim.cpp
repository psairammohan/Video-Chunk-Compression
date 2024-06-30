#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric> // For std::accumulate

using namespace std;

struct FrameData {
    int chunk;
    int frame;
    double psnr;
    double ssim;
};

vector<FrameData> readCSV(const string& filename) {
    ifstream file(filename);
    vector<FrameData> data;
    string line;

    // Skip the header
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        FrameData frameData;

        getline(ss, token, ',');
        frameData.chunk = stoi(token);

        getline(ss, token, ',');
        frameData.frame = stoi(token);

        getline(ss, token, ',');
        frameData.psnr = stod(token);

        getline(ss, token, ',');
        frameData.ssim = stod(token);

        data.push_back(frameData);
    }

    return data;
}

double calculateAveragePSNR(const vector<FrameData>& data) {
    double totalPSNR = accumulate(data.begin(), data.end(), 0.0, 
                                  [](double sum, const FrameData& fd) {
                                      return sum + fd.psnr;
                                  });
    return totalPSNR / data.size();
}

double calculateAverageSSIM(const vector<FrameData>& data) {
    double totalSSIM = accumulate(data.begin(), data.end(), 0.0, 
                                  [](double sum, const FrameData& fd) {
                                      return sum + fd.ssim;
                                  });
    return totalSSIM / data.size();
}

int main() {
    string filename = "frame_metrics.csv";
    vector<FrameData> data = readCSV(filename);

    double averagePSNR = calculateAveragePSNR(data);
    double averageSSIM = calculateAverageSSIM(data);

    cout << "Average PSNR: " << averagePSNR << " dB" << endl;
    cout << "Average SSIM: " << averageSSIM << endl;

    return 0;
}

