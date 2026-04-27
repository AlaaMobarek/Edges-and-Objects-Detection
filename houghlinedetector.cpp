#include "HoughLineDetector.h"
#include <cmath>

HoughLineDetector::HoughLineDetector() {}

std::vector<cv::Vec2f> HoughLineDetector::detectLines(const cv::Mat& edges, int threshold) {
    int width = edges.cols;
    int height = edges.rows;

    int max_rho = std::ceil(std::sqrt(width * width + height * height));
    int num_thetas = 180;

    cv::Mat accumulator = cv::Mat::zeros(2 * max_rho + 1, num_thetas, CV_32S);

    std::vector<double> sin_t(num_thetas), cos_t(num_thetas);
    for(int t = 0; t < num_thetas; t++) {
        double rad = t * CV_PI / 180.0;
        sin_t[t] = std::sin(rad);
        cos_t[t] = std::cos(rad);
    }

    // 1. Voting Process
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(edges.at<uchar>(y, x) == 255) {
                for(int t = 0; t < num_thetas; t++) {
                    int rho = std::round(x * cos_t[t] + y * sin_t[t]);
                    accumulator.at<int>(rho + max_rho, t)++;
                }
            }
        }
    }

    // 2. إيجاد القمم (Peaks)
    std::vector<cv::Vec2f> lines;
    for(int r = 0; r < accumulator.rows; r++) {
        for(int t = 0; t < accumulator.cols; t++) {
            if(accumulator.at<int>(r, t) >= threshold) {
                bool isMax = true;
                for(int i = -1; i <= 1; i++) {
                    for(int j = -1; j <= 1; j++) {
                        int nr = r + i, nt = t + j;
                        if(nr >= 0 && nr < accumulator.rows && nt >= 0 && nt < accumulator.cols) {
                            if(accumulator.at<int>(nr, nt) > accumulator.at<int>(r, t)) {
                                isMax = false;
                            }
                        }
                    }
                }
                if(isMax) {
                    lines.push_back(cv::Vec2f(r - max_rho, t * CV_PI / 180.0));
                }
            }
        }
    }
    return lines;
}

cv::Mat HoughLineDetector::drawLines(const cv::Mat& image, const cv::Mat& edges, const std::vector<cv::Vec2f>& lines) {
    cv::Mat result;
    if (image.channels() == 1) cv::cvtColor(image, result, cv::COLOR_GRAY2BGR);
    else result = image.clone();

    // قللنا الفراغ المسموح بيه عشان يقف أسرع عند الزوايا
    int maxGap = 5;
    int minLength = 20;

    for(size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0], theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;

        cv::Point pt1(cvRound(x0 + 3000 * (-b)), cvRound(y0 + 3000 * (a)));
        cv::Point pt2(cvRound(x0 - 3000 * (-b)), cvRound(y0 - 3000 * (a)));

        cv::LineIterator it(edges, pt1, pt2, 8);

        bool inSegment = false;
        cv::Point segStart(-1, -1), segEnd(-1, -1);
        int currentGap = 0;

        for(int j = 0; j < it.count; j++, ++it) {
            cv::Point p = it.pos();
            if(p.x >= 0 && p.x < edges.cols && p.y >= 0 && p.y < edges.rows) {

                // خلينا البحث Strict أكتر: بنشوف البيكسل نفسه أو اللي لازق فيه مباشرة بدون حواف واسعة
                bool isEdge = false;
                if(edges.at<uchar>(p.y, p.x) == 255) {
                    isEdge = true;
                } else {
                    // لو مش في البيكسل نفسه، ندور في 4 اتجاهات بس مش 8 عشان نقلل التداخل في الزوايا
                    int dx[] = {0, 0, -1, 1};
                    int dy[] = {-1, 1, 0, 0};
                    for(int k = 0; k < 4; k++) {
                        int ny = p.y + dy[k];
                        int nx = p.x + dx[k];
                        if(ny >= 0 && ny < edges.rows && nx >= 0 && nx < edges.cols) {
                            if(edges.at<uchar>(ny, nx) == 255) {
                                isEdge = true;
                                break;
                            }
                        }
                    }
                }

                if(isEdge) {
                    if(!inSegment) {
                        inSegment = true;
                        segStart = p;
                    }
                    segEnd = p;
                    currentGap = 0;
                } else {
                    if(inSegment) {
                        currentGap++;
                        if(currentGap > maxGap) {
                            double len = cv::norm(segStart - segEnd);
                            if(len >= minLength) {
                                cv::line(result, segStart, segEnd, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
                            }
                            inSegment = false;
                        }
                    }
                }
            }
        }

        if(inSegment) {
            double len = cv::norm(segStart - segEnd);
            if(len >= minLength) {
                cv::line(result, segStart, segEnd, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            }
        }
    }
    return result;
}
