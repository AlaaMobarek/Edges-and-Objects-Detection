#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QTabWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QGroupBox>
#include <opencv2/opencv.hpp>

// استدعاء الكلاسات
#include "CannyEdgeDetector.h"
#include "HoughLineDetector.h"
#include "HoughCircleDetector.h"
#include "HoughEllipseDetector.h"
#include "ActiveContour.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void openImage();
    void processHough();
    void processSnake();
    void clearSnakePoints();

private:
    QPushButton *btnOpen;
    QTabWidget *tabWidget;

    // --- Tab 1: Hough Transforms ---
    QLabel *imageLabel;
    QLabel *resultLabel;
    QLabel *houghLabel;

    QLabel *lblLowThresh;
    QSlider *lowThreshSlider;
    QLabel *lblHighThresh;
    QSlider *highThreshSlider;

    QCheckBox *chkLines;
    QLabel *lblHoughThresh;
    QSlider *houghThreshSlider;

    QCheckBox *chkCircles;
    QLabel *lblCircleThresh;
    QSlider *circleThreshSlider;

    QCheckBox *chkEllipses;
    QLabel *lblEllipseThresh;
    QSlider *ellipseThreshSlider;

    // --- Tab 2: Active Contour (Snake) ---
    QLabel *snakeOriginalLabel;
    QLabel *snakeResultLabel;

    QPushButton *btnClearSnake;

    QLabel *lblAlpha;
    QSlider *sliderAlpha;
    QLabel *lblBeta;
    QSlider *sliderBeta;
    QLabel *lblGamma;
    QSlider *sliderGamma;
    QLabel *lblIterations;
    QSlider *sliderIterations;

    // --- Metrics Labels ---
    QLabel *lblArea;
    QLabel *lblPerimeter;
    QLabel *lblChainCode;

    // Variables
    cv::Mat currentImage;
    cv::Mat currentEdges;
    cv::Mat snakeEnergyMat;
    cv::Mat finalHoughImage;
    cv::Mat finalSnakeImage;

    std::vector<cv::Point> userSnakePoints;
    bool isDrawing;

    // Objects
    CannyEdgeDetector cannyDetector;
    HoughLineDetector houghLineDetector;
    HoughCircleDetector houghCircleDetector;
    HoughEllipseDetector houghEllipseDetector;
    ActiveContour activeContour;

    // Helper Functions
    void displayMat(const cv::Mat& mat, QLabel* label);
    cv::Point mapLabelToImage(const QPoint& pos, const QLabel* label, const cv::Mat& img);
    void drawInitialSnake();
};

#endif // MAINWINDOW_H
