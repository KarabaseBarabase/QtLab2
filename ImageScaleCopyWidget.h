#pragma once
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QRubberBand>
#include <QPushButton>

class ImageScaleCopyWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageScaleCopyWidget(QWidget *parent = nullptr);

public slots:
    void loadImage();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QPixmap base;
    QRubberBand *rubber;
    QPoint origin;
    QRect firstSelection;
    QRect secondSelection;
    bool selectingFirst;
    QPushButton *btnSave;
    QRect sourceRect, destRect;
};
