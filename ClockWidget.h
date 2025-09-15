#pragma once
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QTimer>

class ClockWidget : public QWidget {
    Q_OBJECT
public:
    explicit ClockWidget(QWidget *parent = nullptr);

public slots:
    void loadBackground();
    void loadHandImage();

protected:
    void paintEvent(QPaintEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

private:
    void drawDial(QPainter *p);
    void drawHands(QPainter *p);
    void updateBackgroundCache();

    QPixmap originalBg;        // Оригинальное изображение фона циферблата
    QPixmap dialBackground;    // Кэшированное изображение круглого циферблата с фоном
    QImage handImg;            // Изображение стрелки
    QTimer *timer;             // Таймер для обновления времени
    bool backgroundLoaded;     // Флаг загрузки фона
};
