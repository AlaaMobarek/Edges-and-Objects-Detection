#ifndef HOUGHLINEDETECTOR_H
#define HOUGHLINEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>

class HoughLineDetector {
public:
    HoughLineDetector();

    std::vector<cv::Vec2f> detectLines(const cv::Mat& edges, int threshold);

    // ضفنا const cv::Mat& edges هنا
    cv::Mat drawLines(const cv::Mat& image, const cv::Mat& edges, const std::vector<cv::Vec2f>& lines);
};

#endif // HOUGHLINEDETECTOR_H
