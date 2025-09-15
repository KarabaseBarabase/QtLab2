#include "imagescalecopywidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>

ImageScaleCopyWidget::ImageScaleCopyWidget(QWidget *parent) : QWidget(parent), rubber(nullptr), selectingFirst(true) {
    auto *layout = new QVBoxLayout(this);
    auto *controls = new QWidget(this);
    auto *hl = new QHBoxLayout(controls);
    QPushButton *btnLoad = new QPushButton("Загрузить", controls);
    hl->addWidget(btnLoad);
    hl->addStretch();
    controls->setLayout(hl);
    layout->addWidget(controls);


    imageLabel = new QLabel;
    scrollArea = new QScrollArea;
    scrollArea->setWidget(imageLabel);
    layout->addWidget(scrollArea);

    connect(btnLoad, &QPushButton::clicked, this, &ImageScaleCopyWidget::loadImage);
}

void ImageScaleCopyWidget::loadImage() {
    QString f = QFileDialog::getOpenFileName(this, "Картинка", {}, "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    if (!f.isEmpty()) {
        base = QPixmap(f);
        imageLabel->setPixmap(base);
        imageLabel->adjustSize();               // фикс
        imageLabel->setScaledContents(true);    // фикс
        selectingFirst = true;
    }
}

void ImageScaleCopyWidget::mousePressEvent(QMouseEvent *e) {
    origin = e->pos();
    if (!rubber) rubber = new QRubberBand(QRubberBand::Rectangle, this);
    rubber->setGeometry(QRect(origin, QSize()));
    rubber->show();
}

void ImageScaleCopyWidget::mouseMoveEvent(QMouseEvent *e) {
    if (rubber) rubber->setGeometry(QRect(origin, e->pos()).normalized());
}

void ImageScaleCopyWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (!rubber) return;
    QRect sel = rubber->geometry();
    rubber->hide();

    if (selectingFirst) {
        firstSelection = sel;
        selectingFirst = false;
    } else {
        secondSelection = sel;
        if (!firstSelection.isEmpty() && !secondSelection.isEmpty()) {
            QImage img = base.toImage();
            QImage part = img.copy(firstSelection).scaled(secondSelection.size());
            QPainter p(&img);
            p.drawImage(secondSelection.topLeft(), part);
            p.end();
            base = QPixmap::fromImage(img);
            imageLabel->setPixmap(base);
            selectingFirst = true;
        }
    }
}
