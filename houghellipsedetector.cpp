#include "HoughEllipseDetector.h"
#include <cmath>

HoughEllipseDetector::HoughEllipseDetector() {}

std::vector<cv::Vec4f> HoughEllipseDetector::detectEllipses(const cv::Mat& edges, int minA, int maxA, int minB, int maxB, int threshold) {
    int width = edges.cols;
    int height = edges.rows;
    std::vector<cv::Vec4f> ellipses;

    // تجميع بيكسلات الحواف الأول لتسريع اللوب
    std::vector<cv::Point> edge_pts;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (edges.at<uchar>(y, x) == 255) {
                edge_pts.push_back(cv::Point(x, y));
            }
        }
    }

    // حساب الـ Sin والـ Cos مسبقاً لـ 72 زاوية (بنمشي step 5 درجات لتسريع الأداء)
    std::vector<double> sin_t(72), cos_t(72);
    for (int t = 0; t < 72; t++) {
        double rad = t * 5 * CV_PI / 180.0;
        sin_t[t] = std::sin(rad);
        cos_t[t] = std::cos(rad);
    }

    // Loop على المحاور (step 5 عشان السرعة)
    for (int a = minA; a <= maxA; a += 5) {
        for (int b = minB; b <= maxB; b += 5) {

            // نتجاهل الدوائر (لو a و b قريبين جداً من بعض) عشان ميرسمش دائرة وشكل بيضاوي فوق بعض
            if (std::abs(a - b) < 10) continue;

            // 2D Accumulator آمن على الميموري
            cv::Mat acc = cv::Mat::zeros(height, width, CV_32S);

            // Voting
            for (const auto& pt : edge_pts) {
                for (int t = 0; t < 72; t++) {
                    int xc = std::round(pt.x - a * cos_t[t]);
                    int yc = std::round(pt.y - b * sin_t[t]);

                    if (xc >= 0 && xc < width && yc >= 0 && yc < height) {
                        acc.at<int>(yc, xc)++;
                    }
                }
            }

            // إيجاد القمم
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (acc.at<int>(y, x) >= threshold) {
                        // Non-Maximum Suppression عشان ميرسمش كذا شكل جنب بعض
                        bool isMax = true;
                        for (int dy = -2; dy <= 2 && isMax; dy++) {
                            for (int dx = -2; dx <= 2 && isMax; dx++) {
                                if (dx == 0 && dy == 0) continue;
                                int ny = y + dy, nx = x + dx;
                                if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                                    if (acc.at<int>(ny, nx) > acc.at<int>(y, x)) {
                                        isMax = false;
                                    }
                                }
                            }
                        }
                        if (isMax) {
                            ellipses.push_back(cv::Vec4f(x, y, a, b));
                        }
                    }
                }
            }
        }
    }
    return ellipses;
}

cv::Mat HoughEllipseDetector::drawEllipses(const cv::Mat& image, const std::vector<cv::Vec4f>& ellipses) {
    cv::Mat result;
    if (image.channels() == 1) cv::cvtColor(image, result, cv::COLOR_GRAY2BGR);
    else result = image.clone();

    for (size_t i = 0; i < ellipses.size(); i++) {
        cv::Point center(cvRound(ellipses[i][0]), cvRound(ellipses[i][1]));
        int a = cvRound(ellipses[i][2]);
        int b = cvRound(ellipses[i][3]);

        // رسم الشكل البيضاوي باللون اللبني Cyan
        cv::ellipse(result, center, cv::Size(a, b), 0, 0, 360, cv::Scalar(255, 255, 0), 2, cv::LINE_AA);
    }
    return result;
}
