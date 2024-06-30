#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    // Search for all files matching the patterns
    vector<String> webmFiles, mkvFiles;

    // Pattern for webm files: output_chunk_*.webm
    glob("output_chunk_*.webm", webmFiles, false);

    // Pattern for mkv files: chunk_*.mkv
    glob("chunk_*.mkv", mkvFiles, false);

    // File path for the specific video file
    string inputFilePath = "../120s_tears_of_steel_1080p.webm";
    
    int totalFrames_webm = 0;

    // Process webm files
    for (const auto& file : webmFiles) {
        VideoCapture cap(file);
        if (!cap.isOpened()) {
            cerr << "Error opening video file: " << file << endl;
            continue;
        }
        int frameCount = cap.get(CAP_PROP_FRAME_COUNT);
        totalFrames_webm += frameCount;
    }
    
    cout << "Total Frames in .webm video files = " << totalFrames_webm << endl;

    int totalFrames_mkv = 0;
    // Process mkv files
    for (const auto& file : mkvFiles) {
        VideoCapture cap(file);
        if (!cap.isOpened()) {
            cerr << "Error opening video file: " << file << endl;
            continue;
        }
        int frameCount = cap.get(CAP_PROP_FRAME_COUNT);
        totalFrames_mkv += frameCount;
    }

    cout << "Total frames in .mkv video files = " << totalFrames_mkv << endl;

    // Process the specific input.mp4 file
    VideoCapture inputCap(inputFilePath);
    if (!inputCap.isOpened()) {
        cerr << "Error opening video file: " << inputFilePath << endl;
    } else {
        int inputFrameCount = inputCap.get(CAP_PROP_FRAME_COUNT);
        cout << "Total Frames in input.mp4 video file :" << inputFrameCount << endl;
    }

    return 0;
}

