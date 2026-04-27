#include "CannyEdgeDetector.h"
#include <cmath>

CannyEdgeDetector::CannyEdgeDetector() {}

cv::Mat CannyEdgeDetector::apply(const cv::Mat& inputImage, int lowThreshold, int highThreshold, cv::Mat& outGradX, cv::Mat& outGradY) {
    // 1. Convert to Grayscale
    cv::Mat gray;
    if (inputImage.channels() == 3) cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    else gray = inputImage.clone();

    // 2. Gaussian Blur (Noise Reduction)
    cv::Mat blurred = applyGaussianBlur(gray);

    // 3. Sobel Gradients (Edge Intensity & Direction)
    cv::Mat magnitude, angle;
    calculateGradients(blurred, magnitude, angle, outGradX, outGradY);

    // 4. Non-Maximum Suppression (Edge Thinning)
    cv::Mat suppressed = nonMaximumSuppression(magnitude, angle);

    // 5. Double Threshold & Hysteresis (Edge Linking)
    return doubleThresholdAndHysteresis(suppressed, lowThreshold, highThreshold);
}

cv::Mat CannyEdgeDetector::applyGaussianBlur(const cv::Mat& input) {
    float kernel[5][5] = {
        {2, 4, 5, 4, 2},
        {4, 9, 12, 9, 4},
        {5, 12, 15, 12, 5},
        {4, 9, 12, 9, 4},
        {2, 4, 5, 4, 2}
    };
    cv::Mat output = cv::Mat::zeros(input.size(), CV_32F);
    cv::Mat padded;
    cv::copyMakeBorder(input, padded, 2, 2, 2, 2, cv::BORDER_REPLICATE);

    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            float sum = 0.0;
            for (int ki = -2; ki <= 2; ki++) {
                for (int kj = -2; kj <= 2; kj++) {
                    sum += padded.at<uchar>(i + ki + 2, j + kj + 2) * kernel[ki + 2][kj + 2];
                }
            }
            output.at<float>(i, j) = sum / 159.0f;
        }
    }
    return output;
}

void CannyEdgeDetector::calculateGradients(const cv::Mat& input, cv::Mat& magnitude, cv::Mat& angle, cv::Mat& gradX, cv::Mat& gradY) {
    magnitude = cv::Mat::zeros(input.size(), CV_32F);
    angle = cv::Mat::zeros(input.size(), CV_32F);
    gradX = cv::Mat::zeros(input.size(), CV_32F);
    gradY = cv::Mat::zeros(input.size(), CV_32F);

    int Kx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Ky[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
    cv::Mat padded;
    cv::copyMakeBorder(input, padded, 1, 1, 1, 1, cv::BORDER_REPLICATE);

    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            float gx = 0, gy = 0;
            for (int ki = -1; ki <= 1; ki++) {
                for (int kj = -1; kj <= 1; kj++) {
                    float pixel = padded.at<float>(i + ki + 1, j + kj + 1);
                    gx += pixel * Kx[ki + 1][kj + 1];
                    gy += pixel * Ky[ki + 1][kj + 1];
                }
            }
            gradX.at<float>(i, j) = gx;
            gradY.at<float>(i, j) = gy;
            magnitude.at<float>(i, j) = std::sqrt(gx * gx + gy * gy);

            float theta = std::atan2(gy, gx) * 180.0f / CV_PI;
            if (theta < 0) theta += 180.0f;
            angle.at<float>(i, j) = theta;
        }
    }
}

cv::Mat CannyEdgeDetector::nonMaximumSuppression(const cv::Mat& magnitude, const cv::Mat& angle) {
    cv::Mat output = cv::Mat::zeros(magnitude.size(), CV_32F);
    for (int i = 1; i < magnitude.rows - 1; i++) {
        for (int j = 1; j < magnitude.cols - 1; j++) {
            float q = 255.0f, r = 255.0f, currentAngle = angle.at<float>(i, j);

            if ((currentAngle >= 0 && currentAngle < 22.5) || (currentAngle >= 157.5 && currentAngle <= 180)) {
                q = magnitude.at<float>(i, j + 1); r = magnitude.at<float>(i, j - 1);
            } else if (currentAngle >= 22.5 && currentAngle < 67.5) {
                q = magnitude.at<float>(i + 1, j - 1); r = magnitude.at<float>(i - 1, j + 1);
            } else if (currentAngle >= 67.5 && currentAngle < 112.5) {
                q = magnitude.at<float>(i + 1, j); r = magnitude.at<float>(i - 1, j);
            } else if (currentAngle >= 112.5 && currentAngle < 157.5) {
                q = magnitude.at<float>(i - 1, j - 1); r = magnitude.at<float>(i + 1, j + 1);
            }

            if (magnitude.at<float>(i, j) >= q && magnitude.at<float>(i, j) >= r) {
                output.at<float>(i, j) = magnitude.at<float>(i, j);
            } else {
                output.at<float>(i, j) = 0.0f;
            }
        }
    }
    return output;
}

cv::Mat CannyEdgeDetector::doubleThresholdAndHysteresis(const cv::Mat& image, int lowThresh, int highThresh) {
    cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            float val = image.at<float>(i, j);
            if (val >= highThresh) result.at<uchar>(i, j) = 255;
            else if (val >= lowThresh && val < highThresh) result.at<uchar>(i, j) = 75;
        }
    }

    for (int i = 1; i < result.rows - 1; i++) {
        for (int j = 1; j < result.cols - 1; j++) {
            if (result.at<uchar>(i, j) == 75) {
                bool connected = false;
                for (int ki = -1; ki <= 1; ki++) {
                    for (int kj = -1; kj <= 1; kj++) {
                        if (result.at<uchar>(i + ki, j + kj) == 255) { connected = true; break; }
                    }
                }
                result.at<uchar>(i, j) = connected ? 255 : 0;
            }
        }
    }

    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            if (result.at<uchar>(i, j) == 75) result.at<uchar>(i, j) = 0;
        }
    }
    return result;
}
