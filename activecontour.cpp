#include "ActiveContour.h"
#include <cmath>

ActiveContour::ActiveContour() {}

std::vector<cv::Point> ActiveContour::initializeCircle(int centerX, int centerY, int radius, int numPoints) {
    std::vector<cv::Point> points;
    for (int i = 0; i < numPoints; i++) {
        double theta = 2.0 * CV_PI * i / numPoints;
        int x = centerX + radius * std::cos(theta);
        int y = centerY + radius * std::sin(theta);
        points.push_back(cv::Point(x, y));
    }
    return points;
}

cv::Mat ActiveContour::calculateImageEnergy(const cv::Mat& image) {
    cv::Mat gray, edges;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else gray = image.clone();

    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::Canny(gray, edges, 50, 150);

    // عكس الحواف (عشان الـ Distance Transform بيقيس المسافة للبيكسل قيمته 0)
    cv::Mat invertedEdges;
    cv::bitwise_not(edges, invertedEdges);

    // الـ Distance Transform بيعمل تدرج حوالين الحافة كأنه "وادي" الـ Snake بيتزحلق جواه
    cv::Mat distTransform;
    cv::distanceTransform(invertedEdges, distTransform, cv::DIST_L2, 3);
    cv::normalize(distTransform, distTransform, 0, 1.0, cv::NORM_MINMAX);

    return distTransform;
}

void ActiveContour::iterate(std::vector<cv::Point>& points, const cv::Mat& imageEnergy, float alpha, float beta, float gamma) {
    int n = points.size();
    std::vector<cv::Point> newPoints = points;

    // نحسب متوسط المسافة عشان نمنع الـ Snake إنه يكش في نقطة واحدة ويختفي
    float avgDist = 0;
    for (int i = 0; i < n; i++) {
        avgDist += cv::norm(points[i] - points[(i + 1) % n]);
    }
    avgDist /= n;

    for (int i = 0; i < n; i++) {
        cv::Point prev = points[(i - 1 + n) % n];
        cv::Point curr = points[i];
        cv::Point next = points[(i + 1) % n];

        float minEnergy = std::numeric_limits<float>::max();
        cv::Point bestPt = curr;

        // بندور في المربع اللي حوالين النقطة عن أفضل مكان ليها يقلل الطاقة
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                cv::Point p(curr.x + dx, curr.y + dy);

                if (p.x < 0 || p.y < 0 || p.x >= imageEnergy.cols || p.y >= imageEnergy.rows) continue;

                float dist = cv::norm(p - prev);
                float e_cont = std::pow(dist - avgDist, 2);
                float e_curv = std::pow(prev.x - 2 * p.x + next.x, 2) + std::pow(prev.y - 2 * p.y + next.y, 2);
                float e_img = imageEnergy.at<float>(p.y, p.x);

                // مجموع القوى
                float totalEnergy = alpha * e_cont + beta * e_curv + gamma * e_img * 100.0f;

                if (totalEnergy < minEnergy) {
                    minEnergy = totalEnergy;
                    bestPt = p;
                }
            }
        }
        newPoints[i] = bestPt;
    }
    points = newPoints;
}

cv::Mat ActiveContour::drawContour(const cv::Mat& image, const std::vector<cv::Point>& points, const cv::Scalar& color) {
    cv::Mat result;
    if (image.channels() == 1) cv::cvtColor(image, result, cv::COLOR_GRAY2BGR);
    else result = image.clone();

    for (size_t i = 0; i < points.size(); i++) {
        cv::line(result, points[i], points[(i + 1) % points.size()], color, 2, cv::LINE_AA);
        cv::circle(result, points[i], 2, cv::Scalar(0, 255, 0), -1); // نقط المفاصل الخضرا
    }
    return result;
}
