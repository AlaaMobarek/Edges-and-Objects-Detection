#ifndef HOUGHCIRCLEDETECTOR_H
#define HOUGHCIRCLEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>

class HoughCircleDetector {
public:
    HoughCircleDetector();

    // بترجع مصفوفة فيها 3 قيم: a, b, r
    std::vector<cv::Vec3f> detectCircles(const cv::Mat& edges, int minRadius, int maxRadius, int threshold);

    // دالة الرسم باللون الأخضر
    cv::Mat drawCircles(const cv::Mat& image, const std::vector<cv::Vec3f>& circles);
};

#endif // HOUGHCIRCLEDETECTOR_H
