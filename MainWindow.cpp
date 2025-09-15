#include "mainwindow.h"
#include "imageblendwidget.h"
#include "clockwidget.h"
#include "imagecropwidget.h"
#include "imagescalecopywidget.h"
#include <QToolBar>
#include <QAction>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_imageBlendWidget(nullptr)
    , m_clockWidget(nullptr)
    , m_imageCropWidget(nullptr)
    , m_imageScaleCopyWidget(nullptr)
{
    mainToolBar = addToolBar("Задания");
    mainToolBar->setMovable(false);


    QAction *action1 = mainToolBar->addAction("Наложение изображений");
    QAction *action2 = mainToolBar->addAction("Часы");
    QAction *action3 = mainToolBar->addAction("Выделение и сохранение");
    QAction *action4 = mainToolBar->addAction("Копирование с масштабированием");


    connect(action1, &QAction::triggered, this, &MainWindow::showTask1);
    connect(action2, &QAction::triggered, this, &MainWindow::showTask2);
    connect(action3, &QAction::triggered, this, &MainWindow::showTask3);
    connect(action4, &QAction::triggered, this, &MainWindow::showTask4);


    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);


    showTask1();
    resize(1000, 700);
}


void MainWindow::showTask1() {
    if (!m_imageBlendWidget) {
        m_imageBlendWidget = new ImageBlendWidget(this);
        m_stack->addWidget(m_imageBlendWidget);
    }
    m_stack->setCurrentWidget(m_imageBlendWidget);
}


void MainWindow::showTask2() {
    if (!m_clockWidget) {
        m_clockWidget = new ClockWidget(this);
        m_stack->addWidget(m_clockWidget);
    }
    m_stack->setCurrentWidget(m_clockWidget);
}


void MainWindow::showTask3() {
    if (!m_imageCropWidget) {
        m_imageCropWidget = new ImageCropWidget(this);
        m_stack->addWidget(m_imageCropWidget);
    }
    m_stack->setCurrentWidget(m_imageCropWidget);
}


void MainWindow::showTask4() {
    if (!m_imageScaleCopyWidget) {
        m_imageScaleCopyWidget = new ImageScaleCopyWidget(this);
        m_stack->addWidget(m_imageScaleCopyWidget);
    }
    m_stack->setCurrentWidget(m_imageScaleCopyWidget);
}
