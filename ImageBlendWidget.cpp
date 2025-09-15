#include "imageblendwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPainter>

ImageBlendWidget::ImageBlendWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *controls = new QWidget(this);
    auto *hl = new QHBoxLayout(controls);
    QPushButton *btnLoadBase = new QPushButton("Загрузить фон", controls);
    QPushButton *btnLoadTop = new QPushButton("Загрузить наложение", controls);

    comboMode = new QComboBox(controls);
    comboMode->addItem("SourceOver", QVariant(int(QPainter::CompositionMode_SourceOver)));
    comboMode->addItem("Multiply", QVariant(int(QPainter::CompositionMode_Multiply)));
    comboMode->addItem("Screen", QVariant(int(QPainter::CompositionMode_Screen)));
    comboMode->addItem("Overlay", QVariant(int(QPainter::CompositionMode_Overlay)));

    scaleCheck = new QCheckBox("Масштабировать наложение под фон", controls);

    // Добавляет все элементы управления в горизонтальный layout
    hl->addWidget(btnLoadBase);
    hl->addWidget(btnLoadTop);
    hl->addWidget(comboMode);
    hl->addWidget(scaleCheck);
    hl->addStretch();   // Добавляет растягивающийся элемент
    controls->setLayout(hl);                    // Устанавливает layout для виджета управления
    layout->addWidget(controls);                // Добавляет виджет управления в основной layout

    imageLabel = new QLabel;
    scrollArea = new QScrollArea;               // Создает область прокрутки
    scrollArea->setWidget(imageLabel);          // Устанавливает QLabel как виджет в области прокрутки
    scrollArea->setWidgetResizable(true);       // Разрешает изменение размера виджета
    layout->addWidget(scrollArea);

    connect(btnLoadBase, &QPushButton::clicked, this, &ImageBlendWidget::loadBaseImage);
    connect(btnLoadTop, &QPushButton::clicked, this, &ImageBlendWidget::loadTopImage);
    connect(comboMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImageBlendWidget::rebuildComposite);
    connect(scaleCheck, &QCheckBox::toggled, this, &ImageBlendWidget::rebuildComposite);
}

void ImageBlendWidget::loadBaseImage() {
    QString f = QFileDialog::getOpenFileName(this, "Открыть фон", {}, "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    // Открывает диалог выбора файла для фонового изображения
    if (!f.isEmpty()) {
        base = QImage(f);
        rebuildComposite(); // Перестраивает композитное изображение
    }
}

void ImageBlendWidget::loadTopImage() {
    QString f = QFileDialog::getOpenFileName(this, "Открыть наложение", {}, "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    if (!f.isEmpty()) {
        top = QImage(f);
        rebuildComposite();
    }
}

void ImageBlendWidget::rebuildComposite() {
    if (base.isNull()) return;
    QImage result = base.convertToFormat(QImage::Format_ARGB32);

    if (!top.isNull()) {
        QImage overlay = top.convertToFormat(QImage::Format_ARGB32);
        if (scaleCheck->isChecked())
            overlay = overlay.scaled(result.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter p(&result);
        auto mode = QPainter::CompositionMode(comboMode->currentData().toInt());
        p.setCompositionMode(mode);
        p.drawImage((result.width()-overlay.width())/2, (result.height()-overlay.height())/2, overlay);
        p.end();
    }
    imageLabel->setPixmap(QPixmap::fromImage(result));
    imageLabel->adjustSize();
}
