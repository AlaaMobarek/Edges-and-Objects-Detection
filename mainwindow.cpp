#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle("Computer Vision - Hough & Active Contour");
    resize(1200, 700);

    isDrawing = false;

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    btnOpen = new QPushButton("Upload Image", this);
    mainLayout->addWidget(btnOpen);

    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    // ==========================================
    // Tab 1: Hough Transforms
    // ==========================================
    QWidget *houghTab = new QWidget();
    QHBoxLayout *houghLayout = new QHBoxLayout(houghTab);

    // تثبيت عرض اللوحة الشمال عشان نمنع تحرك الشاشة تماماً
    QWidget *houghLeftPanel = new QWidget();
    houghLeftPanel->setFixedWidth(350);
    QVBoxLayout *houghControls = new QVBoxLayout(houghLeftPanel);
    houghControls->setContentsMargins(0, 0, 10, 0);

    lblLowThresh = new QLabel("Canny Low Threshold: 30");
    lowThreshSlider = new QSlider(Qt::Horizontal); lowThreshSlider->setRange(0, 255); lowThreshSlider->setValue(30);
    lblHighThresh = new QLabel("Canny High Threshold: 100");
    highThreshSlider = new QSlider(Qt::Horizontal); highThreshSlider->setRange(0, 255); highThreshSlider->setValue(100);

    chkLines = new QCheckBox("Detect Lines (Red)"); chkLines->setChecked(true);
    lblHoughThresh = new QLabel("Line Threshold: 150");
    houghThreshSlider = new QSlider(Qt::Horizontal); houghThreshSlider->setRange(50, 400); houghThreshSlider->setValue(150);

    chkCircles = new QCheckBox("Detect Circles (Green)");
    lblCircleThresh = new QLabel("Circle Threshold: 90");
    circleThreshSlider = new QSlider(Qt::Horizontal); circleThreshSlider->setRange(30, 200); circleThreshSlider->setValue(90);

    chkEllipses = new QCheckBox("Detect Ellipses (Cyan)");
    lblEllipseThresh = new QLabel("Ellipse Threshold: 60");
    ellipseThreshSlider = new QSlider(Qt::Horizontal); ellipseThreshSlider->setRange(20, 150); ellipseThreshSlider->setValue(60);

    houghControls->addWidget(new QLabel("<b>Canny Settings:</b>"));
    houghControls->addWidget(lblLowThresh); houghControls->addWidget(lowThreshSlider);
    houghControls->addWidget(lblHighThresh); houghControls->addWidget(highThreshSlider);
    houghControls->addSpacing(15);
    houghControls->addWidget(chkLines); houghControls->addWidget(lblHoughThresh); houghControls->addWidget(houghThreshSlider);
    houghControls->addSpacing(15);
    houghControls->addWidget(chkCircles); houghControls->addWidget(lblCircleThresh); houghControls->addWidget(circleThreshSlider);
    houghControls->addSpacing(15);
    houghControls->addWidget(chkEllipses); houghControls->addWidget(lblEllipseThresh); houghControls->addWidget(ellipseThreshSlider);
    houghControls->addStretch();

    imageLabel = new QLabel("Original Image"); imageLabel->setAlignment(Qt::AlignCenter); imageLabel->setStyleSheet("border: 1px solid gray;");
    resultLabel = new QLabel("Canny Edges"); resultLabel->setAlignment(Qt::AlignCenter); resultLabel->setStyleSheet("border: 1px solid gray;");
    houghLabel = new QLabel("Detected Shapes"); houghLabel->setAlignment(Qt::AlignCenter); houghLabel->setStyleSheet("border: 1px solid gray;");

    // حل مشكلة الـ Zoom
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); imageLabel->setMinimumSize(200, 200);
    resultLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); resultLabel->setMinimumSize(200, 200);
    houghLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); houghLabel->setMinimumSize(200, 200);

    QHBoxLayout *houghImages = new QHBoxLayout();
    houghImages->addWidget(imageLabel); houghImages->addWidget(resultLabel); houghImages->addWidget(houghLabel);

    houghLayout->addWidget(houghLeftPanel);
    houghLayout->addLayout(houghImages);
    tabWidget->addTab(houghTab, "Hough Transforms");

    // ==========================================
    // Tab 2: Active Contour (Snake)
    // ==========================================
    QWidget *snakeTab = new QWidget();
    QHBoxLayout *snakeLayout = new QHBoxLayout(snakeTab);

    // تثبيت عرض اللوحة الشمال لمنع حركة الشاشة
    QWidget *snakeLeftPanel = new QWidget();
    snakeLeftPanel->setFixedWidth(350);
    QVBoxLayout *snakeControls = new QVBoxLayout(snakeLeftPanel);
    snakeControls->setContentsMargins(0, 0, 10, 0);

    btnClearSnake = new QPushButton("Clear Initial Shape");

    lblAlpha = new QLabel("Alpha (Elasticity): 1.0");
    sliderAlpha = new QSlider(Qt::Horizontal); sliderAlpha->setRange(0, 50); sliderAlpha->setValue(10);

    lblBeta = new QLabel("Beta (Smoothness): 1.0");
    sliderBeta = new QSlider(Qt::Horizontal); sliderBeta->setRange(0, 50); sliderBeta->setValue(10);

    lblGamma = new QLabel("Gamma (Edge Pull): 2.0");
    sliderGamma = new QSlider(Qt::Horizontal); sliderGamma->setRange(0, 50); sliderGamma->setValue(20);

    lblIterations = new QLabel("Iterations: 100");
    sliderIterations = new QSlider(Qt::Horizontal); sliderIterations->setRange(0, 500); sliderIterations->setValue(100);

    // --- إعدادات المخرجات داخل Container شيك ---
    QGroupBox *metricsGroup = new QGroupBox("Contour Metrics");
    metricsGroup->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 1ex; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; font-weight: bold;}");
    QVBoxLayout *metricsLayout = new QVBoxLayout(metricsGroup);

    lblArea = new QLabel("Area: 0 px²");
    lblArea->setStyleSheet("font-weight: bold;");

    lblPerimeter = new QLabel("Perimeter: 0 px");
    lblPerimeter->setStyleSheet("font-weight: bold;");

    lblChainCode = new QLabel("Chain Code: ");
    lblChainCode->setStyleSheet("font-weight: bold;");
    lblChainCode->setWordWrap(true);

    metricsLayout->addWidget(lblArea);
    metricsLayout->addWidget(lblPerimeter);
    metricsLayout->addWidget(lblChainCode);

    snakeControls->addWidget(btnClearSnake);
    snakeControls->addSpacing(10);
    snakeControls->addWidget(new QLabel("<b>Snake Parameters:</b>"));
    snakeControls->addWidget(lblAlpha); snakeControls->addWidget(sliderAlpha);
    snakeControls->addWidget(lblBeta); snakeControls->addWidget(sliderBeta);
    snakeControls->addWidget(lblGamma); snakeControls->addWidget(sliderGamma);
    snakeControls->addSpacing(15);
    snakeControls->addWidget(lblIterations); snakeControls->addWidget(sliderIterations);

    // إضافة الـ Container للواجهة
    snakeControls->addSpacing(20);
    snakeControls->addWidget(metricsGroup);
    snakeControls->addStretch();

    snakeOriginalLabel = new QLabel("Click and Drag to Draw Initial Snake"); snakeOriginalLabel->setAlignment(Qt::AlignCenter); snakeOriginalLabel->setStyleSheet("border: 1px solid gray;");
    snakeResultLabel = new QLabel("Final Snake"); snakeResultLabel->setAlignment(Qt::AlignCenter); snakeResultLabel->setStyleSheet("border: 1px solid gray;");

    // حل مشكلة الـ Zoom
    snakeOriginalLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); snakeOriginalLabel->setMinimumSize(200, 200);
    snakeResultLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); snakeResultLabel->setMinimumSize(200, 200);

    snakeOriginalLabel->installEventFilter(this);

    QHBoxLayout *snakeImages = new QHBoxLayout();
    snakeImages->addWidget(snakeOriginalLabel); snakeImages->addWidget(snakeResultLabel);

    snakeLayout->addWidget(snakeLeftPanel);
    snakeLayout->addLayout(snakeImages);
    tabWidget->addTab(snakeTab, "Active Contour (Snake)");

    // ==========================================
    // Signals & Slots
    // ==========================================
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(btnClearSnake, &QPushButton::clicked, this, &MainWindow::clearSnakePoints);

    auto updateHough = [this](int){
        lblLowThresh->setText(QString("Canny Low Threshold: %1").arg(lowThreshSlider->value()));
        lblHighThresh->setText(QString("Canny High Threshold: %1").arg(highThreshSlider->value()));
        lblHoughThresh->setText(QString("Line Threshold: %1").arg(houghThreshSlider->value()));
        lblCircleThresh->setText(QString("Circle Threshold: %1").arg(circleThreshSlider->value()));
        lblEllipseThresh->setText(QString("Ellipse Threshold: %1").arg(ellipseThreshSlider->value()));
        processHough();
    };
    connect(lowThreshSlider, &QSlider::valueChanged, updateHough);
    connect(highThreshSlider, &QSlider::valueChanged, updateHough);
    connect(houghThreshSlider, &QSlider::valueChanged, updateHough);
    connect(circleThreshSlider, &QSlider::valueChanged, updateHough);
    connect(ellipseThreshSlider, &QSlider::valueChanged, updateHough);
    connect(chkLines, &QCheckBox::toggled, this, &MainWindow::processHough);
    connect(chkCircles, &QCheckBox::toggled, this, &MainWindow::processHough);
    connect(chkEllipses, &QCheckBox::toggled, this, &MainWindow::processHough);

    auto updateSnake = [this](int){
        lblAlpha->setText(QString("Alpha: %1").arg(sliderAlpha->value() / 10.0));
        lblBeta->setText(QString("Beta: %1").arg(sliderBeta->value() / 10.0));
        lblGamma->setText(QString("Gamma: %1").arg(sliderGamma->value() / 10.0));
        lblIterations->setText(QString("Iterations: %1").arg(sliderIterations->value()));
        processSnake();
    };
    connect(sliderAlpha, &QSlider::valueChanged, updateSnake);
    connect(sliderBeta, &QSlider::valueChanged, updateSnake);
    connect(sliderGamma, &QSlider::valueChanged, updateSnake);
    connect(sliderIterations, &QSlider::valueChanged, updateSnake);
}

MainWindow::~MainWindow() {}

void MainWindow::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        currentImage = cv::imread(fileName.toStdString());
        snakeEnergyMat = activeContour.calculateImageEnergy(currentImage);
        clearSnakePoints();
        processHough();
    }
}

void MainWindow::processHough() {
    if (currentImage.empty()) return;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    cv::Mat gradX, gradY;
    currentEdges = cannyDetector.apply(currentImage, lowThreshSlider->value(), highThreshSlider->value(), gradX, gradY);

    cv::Mat resultImage;
    if (currentImage.channels() == 1) cv::cvtColor(currentImage, resultImage, cv::COLOR_GRAY2BGR);
    else resultImage = currentImage.clone();

    if (chkLines->isChecked()) {
        auto lines = houghLineDetector.detectLines(currentEdges, houghThreshSlider->value());
        resultImage = houghLineDetector.drawLines(resultImage, currentEdges, lines);
    }
    if (chkCircles->isChecked()) {
        auto circles = houghCircleDetector.detectCircles(currentEdges, 10, 150, circleThreshSlider->value());
        resultImage = houghCircleDetector.drawCircles(resultImage, circles);
    }
    if (chkEllipses->isChecked()) {
        auto ellipses = houghEllipseDetector.detectEllipses(currentEdges, 15, 150, 15, 150, ellipseThreshSlider->value());
        resultImage = houghEllipseDetector.drawEllipses(resultImage, ellipses);
    }

    finalHoughImage = resultImage;
    displayMat(currentImage, imageLabel);
    displayMat(currentEdges, resultLabel);
    displayMat(finalHoughImage, houghLabel);

    QApplication::restoreOverrideCursor();
}

// ==========================================
// Mouse Events (Freehand Drawing)
// ==========================================

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == snakeOriginalLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDrawing = true;
                userSnakePoints.clear();
                cv::Point pt = mapLabelToImage(mouseEvent->pos(), snakeOriginalLabel, currentImage);
                if (pt.x != -1) {
                    userSnakePoints.push_back(pt);
                    drawInitialSnake();
                }
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (isDrawing) {
                cv::Point pt = mapLabelToImage(mouseEvent->pos(), snakeOriginalLabel, currentImage);
                if (pt.x != -1) {
                    if (userSnakePoints.empty() || cv::norm(userSnakePoints.back() - pt) > 8) {
                        userSnakePoints.push_back(pt);
                        drawInitialSnake();
                    }
                }
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && isDrawing) {
                isDrawing = false;
                drawInitialSnake();
                if (userSnakePoints.size() >= 3) {
                    processSnake();
                }
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

cv::Point MainWindow::mapLabelToImage(const QPoint& pos, const QLabel* label, const cv::Mat& img) {
    if (img.empty()) return cv::Point(-1, -1);

    double labelW = label->width();
    double labelH = label->height();
    double imgW = img.cols;
    double imgH = img.rows;

    double scale = std::min(labelW / imgW, labelH / imgH);
    double scaledW = imgW * scale;
    double scaledH = imgH * scale;

    double offsetX = (labelW - scaledW) / 2.0;
    double offsetY = (labelH - scaledH) / 2.0;

    int imgX = std::round((pos.x() - offsetX) / scale);
    int imgY = std::round((pos.y() - offsetY) / scale);

    if (imgX >= 0 && imgX < imgW && imgY >= 0 && imgY < imgH) {
        return cv::Point(imgX, imgY);
    }
    return cv::Point(-1, -1);
}

void MainWindow::drawInitialSnake() {
    if (currentImage.empty()) return;
    cv::Mat initImg;
    if (currentImage.channels() == 1) cv::cvtColor(currentImage, initImg, cv::COLOR_GRAY2BGR);
    else initImg = currentImage.clone();

    // رسم الخط بلون أصفر عشان يكون واضح جداً
    for (size_t i = 0; i < userSnakePoints.size(); i++) {
        cv::circle(initImg, userSnakePoints[i], 2, cv::Scalar(0, 255, 255), -1);
        if (i > 0) {
            cv::line(initImg, userSnakePoints[i - 1], userSnakePoints[i], cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
        }
    }
    if (!isDrawing && userSnakePoints.size() >= 3) {
        cv::line(initImg, userSnakePoints.back(), userSnakePoints.front(), cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
    }
    displayMat(initImg, snakeOriginalLabel);
}

void MainWindow::clearSnakePoints() {
    userSnakePoints.clear();
    drawInitialSnake();
    if (!currentImage.empty()) {
        finalSnakeImage = currentImage.clone();
        displayMat(finalSnakeImage, snakeResultLabel);
    }
    lblArea->setText("Area: 0 px²");
    lblPerimeter->setText("Perimeter: 0 px");
    lblChainCode->setText("Chain Code: ");
}

void MainWindow::processSnake() {
    if (currentImage.empty() || snakeEnergyMat.empty() || userSnakePoints.size() < 3) return;

    float alpha = sliderAlpha->value() / 10.0f;
    float beta = sliderBeta->value() / 10.0f;
    float gamma = sliderGamma->value() / 10.0f;
    int iterations = sliderIterations->value();

    std::vector<cv::Point> currentSnake = userSnakePoints;

    for (int i = 0; i < iterations; i++) {
        activeContour.iterate(currentSnake, snakeEnergyMat, alpha, beta, gamma);
    }

    // تلوين المخرج النهائي للـ Snake بلون أصفر فاقع عشان يبان بوضوح
    finalSnakeImage = activeContour.drawContour(currentImage, currentSnake, cv::Scalar(0, 255, 255));
    displayMat(finalSnakeImage, snakeResultLabel);

    // --- حساب المساحة والمحيط ---
    double area = cv::contourArea(currentSnake);
    double perimeter = cv::arcLength(currentSnake, true);

    lblArea->setText(QString("Area: %1 px²").arg(area, 0, 'f', 2));
    lblPerimeter->setText(QString("Perimeter: %1 px").arg(perimeter, 0, 'f', 2));

    // --- حساب الـ Chain Code ---
    cv::Mat mask = cv::Mat::zeros(currentImage.size(), CV_8UC1);
    std::vector<std::vector<cv::Point>> fillPts = {currentSnake};
    cv::fillPoly(mask, fillPts, cv::Scalar(255));

    std::vector<std::vector<cv::Point>> denseContours;
    cv::findContours(mask, denseContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    QString chainStr = "Chain Code: ";
    if (!denseContours.empty() && denseContours[0].size() > 0) {
        std::string code = "";
        const auto& c = denseContours[0];
        for (size_t i = 0; i < c.size(); i++) {
            cv::Point p1 = c[i];
            cv::Point p2 = c[(i + 1) % c.size()];
            int dx = p2.x - p1.x;
            int dy = p2.y - p1.y;

            if (dx > 0 && dy == 0) code += "0";
            else if (dx > 0 && dy < 0) code += "1";
            else if (dx == 0 && dy < 0) code += "2";
            else if (dx < 0 && dy < 0) code += "3";
            else if (dx < 0 && dy == 0) code += "4";
            else if (dx < 0 && dy > 0) code += "5";
            else if (dx == 0 && dy > 0) code += "6";
            else if (dx > 0 && dy > 0) code += "7";
        }

        if (code.length() > 60) {
            chainStr += QString::fromStdString(code.substr(0, 60)) + " ...";
        } else {
            chainStr += QString::fromStdString(code);
        }
    }
    lblChainCode->setText(chainStr);
}

// ==========================================
// Display & Resize
// ==========================================

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (!currentImage.empty()) {
        displayMat(currentImage, imageLabel);
        displayMat(currentEdges, resultLabel);
        displayMat(finalHoughImage, houghLabel);

        drawInitialSnake();
        if (!finalSnakeImage.empty()) displayMat(finalSnakeImage, snakeResultLabel);
    }
}

void MainWindow::displayMat(const cv::Mat& mat, QLabel* label) {
    if (mat.empty() || label->width() <= 0 || label->height() <= 0) return;
    QImage qimg;
    cv::Mat rgbMat;
    if (mat.channels() == 1) cv::cvtColor(mat, rgbMat, cv::COLOR_GRAY2RGB);
    else cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);

    qimg = QImage((const unsigned char*)(rgbMat.data), rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
    label->setPixmap(QPixmap::fromImage(qimg).scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
