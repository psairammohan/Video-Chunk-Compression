#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

// Function to read frame positions from a file
vector<int> readFramePositions(const string& filename) {
    vector<int> framePositions;
    ifstream inputFile(filename);
    int framePos;
    while (inputFile >> framePos) {
        framePositions.push_back(framePos);
    }
    inputFile.close();
    return framePositions;
}

// Function to create video chunks based on frame positions
void createVideoChunks(const string& videoFile, const vector<int>& framePositions) {
    VideoCapture cap(videoFile);

    if (!cap.isOpened()) {
        cout << "Error opening video file: " << videoFile << endl;
        return;
    }

    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);

    int chunkCount = 1;
    for (size_t i = 0; i < framePositions.size() - 1; ++i) {
        int startFrame = framePositions[i];
        int endFrame = framePositions[i + 1];

        cap.set(CAP_PROP_POS_FRAMES, startFrame);

        string outputFilename = "chunk_" + to_string(chunkCount++) + ".mkv";
        VideoWriter writer(outputFilename, VideoWriter::fourcc('F', 'F', 'V', '1'), fps, Size(frameWidth, frameHeight));

        for (int f = startFrame; f < endFrame; ++f) {
            Mat frame;
            cap >> frame;
            if (frame.empty())
                break;
            writer.write(frame);
        }
        writer.release();
    }
    cap.release();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: ./VideoChunkCreator <video_file>" << endl;
        return -1;
    }

    string videoFile = argv[1];
    string framePositionsFile = "/mnt/data/scene_changes.txt"; // Update the path to your file if necessary

    vector<int> framePositions = readFramePositions(framePositionsFile);
    if (framePositions.empty()) {
        cout << "No frame positions read from file: " << framePositionsFile << endl;
        return -1;
    }

    createVideoChunks(videoFile, framePositions);

    cout << "Video chunk creation completed." << endl;
    return 0;
}

