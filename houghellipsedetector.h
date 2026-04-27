#ifndef HOUGHELLIPSEDETECTOR_H
#define HOUGHELLIPSEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>

class HoughEllipseDetector {
public:
    HoughEllipseDetector();

    // بترجع مصفوفة فيها 4 قيم: xc, yc, a, b
    std::vector<cv::Vec4f> detectEllipses(const cv::Mat& edges, int minA, int maxA, int minB, int maxB, int threshold);

    // دالة الرسم باللون اللبني (Cyan) زي ما مطلوب في التاسك
    cv::Mat drawEllipses(const cv::Mat& image, const std::vector<cv::Vec4f>& ellipses);
};

#endif // HOUGHELLIPSEDETECTOR_H
