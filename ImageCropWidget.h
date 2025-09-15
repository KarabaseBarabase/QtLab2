#pragma once
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QRubberBand>

    class ImageCropWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageCropWidget(QWidget *parent = nullptr);

public slots:
    void loadImage();
    void saveSelection();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QPixmap base;
    QPushButton *btnSave;
    QRubberBand *rubber;
    QPoint origin;
    QRect selection;
};
