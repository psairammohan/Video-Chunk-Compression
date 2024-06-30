#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <fstream> // For file handling

// Function to calculate PSNR for two frames
double getPSNR(const cv::Mat& I1, const cv::Mat& I2) {
    cv::Mat s1;
    cv::absdiff(I1, I2, s1);           // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    cv::Scalar s = cv::sum(s1);        // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if (sse <= 1e-10) { // for small values return zero
        return 0;
    } else {
        double mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

// Function to calculate SSIM for two frames
cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2) {
    const double C1 = 6.5025, C2 = 58.5225;
    int d = CV_32F;

    cv::Mat I1, I2;
    i1.convertTo(I1, d);
    i2.convertTo(I2, d);

    cv::Mat I2_2 = I2.mul(I2);
    cv::Mat I1_2 = I1.mul(I1);
    cv::Mat I1_I2 = I1.mul(I2);

    cv::Mat mu1, mu2;
    cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

    cv::Mat mu1_2 = mu1.mul(mu1);
    cv::Mat mu2_2 = mu2.mul(mu2);
    cv::Mat mu1_mu2 = mu1.mul(mu2);

    cv::Mat sigma1_2, sigma2_2, sigma12;

    cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    cv::Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);

    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);
    cv::Scalar mssim = cv::mean(ssim_map);
    return mssim;
}

int main() {
    std::string chunkPrefix = "chunk_";
    std::string outputChunkPrefix = "output_chunk_";
    std::string inputExtension = ".mkv";
    std::string outputExtension = ".webm";
    int i = 1;
    std::ofstream chunkOutputFile("chunk_metrics_all.csv"); // Open CSV file for chunk metrics
    std::ofstream frameOutputFile("frame_metrics.csv");

    if (!chunkOutputFile.is_open()) {
        std::cerr << "Failed to open CSV file for writing chunk metrics!" << std::endl;
        return -1;
    }

    // Write headers to chunk metrics CSV file
    chunkOutputFile << "Chunk,Average PSNR (dB),Average SSIM" << std::endl;
    
    if (!frameOutputFile.is_open()) {
        std::cerr << "Failed to open CSV file for writing chunk metrics!" << std::endl;
        return -1;
    }

    // Write headers to chunk metrics CSV file
    frameOutputFile<< "Chunk,Frame,PSNR (dB),SSIM" << std::endl;

    int total_frames = 0;
    while (std::filesystem::exists(chunkPrefix + std::to_string(i) + inputExtension)) {
        std::string inputChunk = chunkPrefix + std::to_string(i) + inputExtension;
        std::string outputChunk = outputChunkPrefix + std::to_string(i) + outputExtension;

        // Open the input and output video chunks
        cv::VideoCapture capInput(inputChunk);
        cv::VideoCapture capOutput(outputChunk);

        if (!capInput.isOpened() || !capOutput.isOpened()) {
            std::cerr << "Could not open the video files!" << std::endl;
            return -1;
        }

        // Get the frame dimensions from the input chunk
        int width = static_cast<int>(capInput.get(cv::CAP_PROP_FRAME_WIDTH));
        int height = static_cast<int>(capInput.get(cv::CAP_PROP_FRAME_HEIGHT));

        cv::Mat inputFrame, outputFrame;
        int frameCount = 0;
        double totalPSNR = 0;
        double totalSSIM = 0;

        while (true) {
            capInput >> inputFrame;
            capOutput >> outputFrame;

            if (inputFrame.empty() || outputFrame.empty()) {
                break;
            }

            // Calculate PSNR and SSIM for the current frame pair
            double psnr = getPSNR(inputFrame, outputFrame);
            cv::Scalar ssim = getMSSIM(inputFrame, outputFrame);

            frameOutputFile << i << "," << frameCount + total_frames << "," << psnr << "," << ssim.val[0] << std::endl;

            // Accumulate total PSNR and SSIM for chunk metrics
            totalPSNR += psnr;
            totalSSIM += ssim.val[0];

            // Increment frame count
            frameCount++;
        }

        if (frameCount > 0) {
            // Calculate average PSNR and SSIM for chunk
            double avgPSNR = totalPSNR / frameCount;
            double avgSSIM = totalSSIM / frameCount;

            // Write chunk metrics to chunk metrics CSV file
            chunkOutputFile << i << "," << avgPSNR << "," << avgSSIM << std::endl;

            std::cout << "Chunk " << i << " Average PSNR: " << avgPSNR << " dB" << std::endl;
            std::cout << "Chunk " << i << " Average SSIM: " << avgSSIM << std::endl;
            total_frames += frameCount;
        }

        ++i;
    }

    // Close chunk metrics CSV file
    chunkOutputFile.close();
    frameOutputFile.close();
    return 0;
}

