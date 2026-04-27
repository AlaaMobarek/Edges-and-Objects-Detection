#include "HoughCircleDetector.h"
#include <cmath>

HoughCircleDetector::HoughCircleDetector() {}

std::vector<cv::Vec3f> HoughCircleDetector::detectCircles(const cv::Mat& edges, int minRadius, int maxRadius, int threshold) {
    int width = edges.cols;
    int height = edges.rows;
    int num_radii = maxRadius - minRadius + 1;

    // 3D Accumulator مفرود في 1D عشان تسريع الميموري جداً
    // Index = a + b * width + r_idx * width * height
    std::vector<int> accumulator(width * height * num_radii, 0);

    // حساب الـ Sin والـ Cos مسبقاً
    std::vector<double> sin_t(360), cos_t(360);
    for (int t = 0; t < 360; t++) {
        double rad = t * CV_PI / 180.0;
        sin_t[t] = std::sin(rad);
        cos_t[t] = std::cos(rad);
    }

    // 1. Voting Process
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (edges.at<uchar>(y, x) == 255) {
                for (int r = minRadius; r <= maxRadius; r++) {
                    int r_idx = r - minRadius;
                    // بنط خطوتين في الزاوية لتسريع الأداء للضعف بدون خسارة دقة كبيرة
                    for (int t = 0; t < 360; t += 2) {
                        int a = std::round(x - r * cos_t[t]);
                        int b = std::round(y - r * sin_t[t]);

                        if (a >= 0 && a < width && b >= 0 && b < height) {
                            accumulator[a + b * width + r_idx * width * height]++;
                        }
                    }
                }
            }
        }
    }

    // 2. إيجاد القمم (Peaks) و Non-Maximum Suppression
    std::vector<cv::Vec3f> circles;
    for (int r = minRadius; r <= maxRadius; r++) {
        int r_idx = r - minRadius;
        for (int b = 0; b < height; b++) {
            for (int a = 0; a < width; a++) {
                int val = accumulator[a + b * width + r_idx * width * height];
                if (val >= threshold) {
                    // نتأكد إن دي أعلى نقطة في محيطها عشان الدائرة متترسمش كذا مرة فوق بعض
                    bool isMax = true;
                    for (int dy = -5; dy <= 5 && isMax; dy++) {
                        for (int dx = -5; dx <= 5 && isMax; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            int na = a + dx, nb = b + dy;
                            if (na >= 0 && na < width && nb >= 0 && nb < height) {
                                if (accumulator[na + nb * width + r_idx * width * height] > val) {
                                    isMax = false;
                                }
                            }
                        }
                    }
                    if (isMax) {
                        circles.push_back(cv::Vec3f(a, b, r));
                    }
                }
            }
        }
    }
    return circles;
}

cv::Mat HoughCircleDetector::drawCircles(const cv::Mat& image, const std::vector<cv::Vec3f>& circles) {
    cv::Mat result;
    if (image.channels() == 1) cv::cvtColor(image, result, cv::COLOR_GRAY2BGR);
    else result = image.clone();

    for (size_t i = 0; i < circles.size(); i++) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);

        // رسم نقطة المركز
        cv::circle(result, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        // رسم محيط الدائرة (باللون الأخضر)
        cv::circle(result, center, radius, cv::Scalar(0, 255, 0), 2, 8, 0);
    }
    return result;
}
