#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

// Function to calculate PSNR between two images
double getPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
    cv::Mat s1;
    cv::absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);      // convert to float
    s1 = s1.mul(s1);               // |I1 - I2|^2

    cv::Scalar s = cv::sum(s1);    // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if (sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

// Function to calculate SSIM between two images
cv::Scalar getSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;

    cv::Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    cv::Mat I2_2 = I2.mul(I2);        // I2^2
    cv::Mat I1_2 = I1.mul(I1);        // I1^2
    cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    cv::Mat mu1, mu2;   //
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

    ///////////////////////////////// FORMULA ////////////////////////////////
    cv::Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);        // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);        // t1 = ((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    cv::Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    cv::Scalar mssim = mean(ssim_map); // mssim = average of ssim map
    return mssim;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./video_compare <video1> <video2>" << std::endl;
        return -1;
    }

    cv::VideoCapture cap1(argv[1]);
    cv::VideoCapture cap2(argv[2]);

    if (!cap1.isOpened() || !cap2.isOpened())
    {
        std::cout << "Error opening video streams" << std::endl;
        return -1;
    }

    cv::Mat frame1, frame2;
    double totalPSNR = 0;
    cv::Scalar totalSSIM(0, 0, 0, 0);
    int frameCount = 0;

    // Open CSV file for writing
    std::ofstream csvFile("output.csv");
    csvFile << "Frame,PSNR,SSIM" << std::endl;

    while (true)
    {
        bool ret1 = cap1.read(frame1);
        bool ret2 = cap2.read(frame2);

        if (!ret1 || !ret2)
            break;

        double psnr = getPSNR(frame1, frame2);
        cv::Scalar ssim = getSSIM(frame1, frame2);

        totalPSNR += psnr;
        totalSSIM += ssim;
        frameCount++;

        // Save frames as images
        /*std::string frame1Path = "frame1_" + std::to_string(frameCount) + ".png";*/
        /*std::string frame2Path = "frame2_" + std::to_string(frameCount) + ".png";*/
        /*cv::imwrite(frame1Path, frame1);*/
        /*cv::imwrite(frame2Path, frame2);*/

        // Write PSNR and SSIM to CSV file
        csvFile << frameCount << "," << psnr << "," << ssim[0] << std::endl;

        /*std::cout << "Frame " << frameCount << " - PSNR: " << psnr << " SSIM: "*/
        /*          << ssim[0] << ", " << std::endl;*/
    }

    totalPSNR /= frameCount;
    totalSSIM /= frameCount;

    std::cout << "Average PSNR: " << totalPSNR << std::endl;
    std::cout << "Average SSIM: "
              << totalSSIM[0] << ", " << totalSSIM[1] << ", " << totalSSIM[2] << std::endl;

    // Close the CSV file
    csvFile.close();

    cap1.release();
    cap2.release();

    return 0;
}

