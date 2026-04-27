#ifndef CANNYEDGEDETECTOR_H
#define CANNYEDGEDETECTOR_H

#include <opencv2/opencv.hpp>

class CannyEdgeDetector {
public:
    CannyEdgeDetector();
    // الدالة الرئيسية اللي بتشغل الـ 5 خطوات
    cv::Mat apply(const cv::Mat& inputImage, int lowThreshold, int highThreshold, cv::Mat& outGradX, cv::Mat& outGradY);

private:
    cv::Mat applyGaussianBlur(const cv::Mat& input);
    void calculateGradients(const cv::Mat& input, cv::Mat& magnitude, cv::Mat& angle, cv::Mat& gradX, cv::Mat& gradY);
    cv::Mat nonMaximumSuppression(const cv::Mat& magnitude, const cv::Mat& angle);
    cv::Mat doubleThresholdAndHysteresis(const cv::Mat& image, int lowThresh, int highThresh);
};

#endif // CANNYEDGEDETECTOR_H
