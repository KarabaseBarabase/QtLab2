#include "imagecropwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMouseEvent>
#include <QScrollBar>

ImageCropWidget::ImageCropWidget(QWidget *parent) : QWidget(parent), rubber(nullptr) {
    auto *layout = new QVBoxLayout(this);
    auto *controls = new QWidget(this);
    auto *hl = new QHBoxLayout(controls);
    QPushButton *btnLoad = new QPushButton("Загрузить", controls);
    btnSave = new QPushButton("Сохранить", controls);
    btnSave->setEnabled(false);
    hl->addWidget(btnLoad);
    hl->addWidget(btnSave);
    hl->addStretch();
    controls->setLayout(hl);
    layout->addWidget(controls);

    imageLabel = new QLabel;
    scrollArea = new QScrollArea;
    scrollArea->setWidget(imageLabel);
    layout->addWidget(scrollArea);

    connect(btnLoad, &QPushButton::clicked, this, &ImageCropWidget::loadImage);
    connect(btnSave, &QPushButton::clicked, this, &ImageCropWidget::saveSelection);
}

void ImageCropWidget::loadImage() {
    QString f = QFileDialog::getOpenFileName(this, "Картинка", {}, "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    if (!f.isEmpty()) {
        base = QPixmap(f);
        imageLabel->setPixmap(base);
        imageLabel->adjustSize();
        imageLabel->setScaledContents(true);    // Масштабирование
        btnSave->setEnabled(false);
    }
}
void ImageCropWidget::saveSelection() {
    if (selection.isEmpty()) return;
    QImage img = base.toImage().copy(selection);
    QString f = QFileDialog::getSaveFileName(this, "Сохранить", {}, "PNG (*.png);;JPEG (*.jpg)");
    if (!f.isEmpty()) img.save(f);
}

void ImageCropWidget::mousePressEvent(QMouseEvent *e) {
    origin = e->pos();
    if (!rubber) rubber = new QRubberBand(QRubberBand::Rectangle, this);
    rubber->setGeometry(QRect(origin, QSize()));
    rubber->show();
}

void ImageCropWidget::mouseMoveEvent(QMouseEvent *e) {
    if (rubber) rubber->setGeometry(QRect(origin, e->pos()).normalized());
}

void ImageCropWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (rubber) {
        selection = rubber->geometry();
        btnSave->setEnabled(true);
        rubber->hide();
    }
}
