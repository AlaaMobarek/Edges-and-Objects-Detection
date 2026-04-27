#ifndef ACTIVECONTOUR_H
#define ACTIVECONTOUR_H

#include <opencv2/opencv.hpp>
#include <vector>

class ActiveContour {
public:
    ActiveContour();

    // إنشاء الدائرة المبدئية اللي هتبدأ تقفل على الشكل
    std::vector<cv::Point> initializeCircle(int centerX, int centerY, int radius, int numPoints = 100);

    // حساب طاقة الصورة (المغناطيس اللي بيسحب النقط للحواف)
    cv::Mat calculateImageEnergy(const cv::Mat& image);

    // تنفيذ لفة (Iteration) واحدة من خوارزمية الـ Snake
    void iterate(std::vector<cv::Point>& points, const cv::Mat& imageEnergy, float alpha, float beta, float gamma);

    // رسم الـ Snake على الصورة
    cv::Mat drawContour(const cv::Mat& image, const std::vector<cv::Point>& points, const cv::Scalar& color = cv::Scalar(0, 0, 255));
};

#endif // ACTIVECONTOUR_H
