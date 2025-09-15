// mainwindow.h
#pragma once
#include <QMainWindow>
#include <QStackedWidget>


class ImageBlendWidget;
class ClockWidget;
class ImageCropWidget;
class ImageScaleCopyWidget;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);


private slots:
    void showTask1();
    void showTask2();
    void showTask3();
    void showTask4();


private:
    QToolBar *mainToolBar;
    QStackedWidget *m_stack;
    ImageBlendWidget *m_imageBlendWidget;
    ClockWidget *m_clockWidget;
    ImageCropWidget *m_imageCropWidget;
    ImageScaleCopyWidget *m_imageScaleCopyWidget;
};
