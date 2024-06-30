#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

using namespace cv;
using namespace std;

double calculateHistogramDifference(const Mat& frame1, const Mat& frame2) {
    Mat hsvFrame1, hsvFrame2;
    cvtColor(frame1, hsvFrame1, COLOR_BGR2HSV);
    cvtColor(frame2, hsvFrame2, COLOR_BGR2HSV);

    int hBins = 50, sBins = 60;
    int histSize[] = { hBins, sBins };
    float hRanges[] = { 0, 180 };
    float sRanges[] = { 0, 256 };
    const float* ranges[] = { hRanges, sRanges };
    int channels[] = { 0, 1 };

    Mat histFrame1, histFrame2;
    calcHist(&hsvFrame1, 1, channels, Mat(), histFrame1, 2, histSize, ranges, true, false);
    normalize(histFrame1, histFrame1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&hsvFrame2, 1, channels, Mat(), histFrame2, 2, histSize, ranges, true, false);
    normalize(histFrame2, histFrame2, 0, 1, NORM_MINMAX, -1, Mat());

    double diff = compareHist(histFrame1, histFrame2, HISTCMP_BHATTACHARYYA);
    return diff;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <video_file> <output_file>" << endl;
        return -1;
    }

    string videoFile = argv[1];
    string outputFile = argv[2];
    VideoCapture cap(videoFile);
    if (!cap.isOpened()) {
        cout << "Error opening video file: " << videoFile << endl;
        return -1;
    }

    Mat prevFrame, currFrame;
    vector<int> sceneChanges;
    double threshold = 0.3;  // Adjust the threshold based on your needs
    int framePos = 0;

    sceneChanges.push_back(0);
    
    while (true) {
        if (!cap.read(currFrame)) {
            break;
        }

        if (framePos > 0) {
            double diff = calculateHistogramDifference(prevFrame, currFrame);
            if (diff > threshold) {
                sceneChanges.push_back(framePos);
            }
        }

        currFrame.copyTo(prevFrame);
        framePos++;
    }

    int total_frames = cap.get(CAP_PROP_FRAME_COUNT);
    sceneChanges.push_back(total_frames);

    cap.release();

    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cout << "Error opening output file: " << outputFile << endl;
        return -1;
    }

    for (int frame : sceneChanges) {
        outFile << frame << endl;
    }
    outFile.close();

    cout << "Scene changes detected and saved to " << outputFile << endl;

    return 0;
}

