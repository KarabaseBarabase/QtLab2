#pragma once
#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QComboBox>
#include <QCheckBox>

class ImageBlendWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageBlendWidget(QWidget *parent = nullptr);

public slots:
    void loadBaseImage();
    void loadTopImage();
    void rebuildComposite();

private:
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QImage base, top;
    QComboBox *comboMode;
    QCheckBox *scaleCheck;
};
