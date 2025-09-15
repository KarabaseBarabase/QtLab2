#include "clockwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPainter>
#include <QTime>
#include <QCheckBox>
#include <QTimer>
#include <QPainterPath>
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>

ClockWidget::ClockWidget(QWidget *parent) : QWidget(parent), backgroundLoaded(false) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    auto *controls = new QWidget(this);
    auto *hl = new QHBoxLayout(controls);
    controls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // Создаем кнопки
    QPushButton *btnLoadBg = new QPushButton("Загрузить фон", controls);
    QPushButton *btnLoadHand = new QPushButton("Загрузить стрелку", controls);

    hl->addWidget(btnLoadBg);
    hl->addWidget(btnLoadHand);
    hl->addStretch();

    controls->setLayout(hl);
    layout->addWidget(controls);

    // Подключаем сигналы
    connect(btnLoadBg, &QPushButton::clicked, this, &ClockWidget::loadBackground);
    connect(btnLoadHand, &QPushButton::clicked, this, &ClockWidget::loadHandImage);

    // Таймер для обновления времени
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, qOverload<>(&ClockWidget::update));
    timer->start(1000);
}

void ClockWidget::loadBackground() {
    QString f = QFileDialog::getOpenFileName(this, "Фон циферблата", {},
                                             "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    if (!f.isEmpty()) {
        originalBg = QPixmap(f);
        backgroundLoaded = true;
        updateBackgroundCache();
        update();
    }
}

void ClockWidget::loadHandImage() {
    QString f = QFileDialog::getOpenFileName(this, "Изображение стрелки", {},
                                             "Images (*.png *.jpg *.bmp *.gif *.tiff)");
    if (!f.isEmpty()) {
        handImg = QImage(f);
        if (handImg.isNull()) {
            qDebug() << "Не удалось загрузить изображение стрелки";
        } else {
            qDebug() << "Изображение стрелки загружено, размер:" << handImg.size();
        }
        update();
    }
}

void ClockWidget::updateBackgroundCache() {
    if (originalBg.isNull()) {
        dialBackground = QPixmap();
        return;
    }

    // Создаем круглое изображение циферблата с фоном
    int dialSize = qMin(width(), height() - 50); // Учитываем место для панели управления
    if (dialSize <= 0) dialSize = 300;

    dialBackground = QPixmap(dialSize, dialSize);
    dialBackground.fill(Qt::transparent);

    QPainter p(&dialBackground);
    p.setRenderHint(QPainter::Antialiasing);

    // Создаем круглую маску для фона
    QPainterPath path;
    path.addEllipse(0, 0, dialSize, dialSize);
    p.setClipPath(path);

    // Масштабируем фон под размер циферблата
    QPixmap scaledBg = originalBg.scaled(dialSize, dialSize,
                                         Qt::KeepAspectRatioByExpanding,
                                         Qt::SmoothTransformation);

    // Вырезаем центральную часть
    int x = (scaledBg.width() - dialSize) / 2;
    int y = (scaledBg.height() - dialSize) / 2;
    p.drawPixmap(0, 0, scaledBg, x, y, dialSize, dialSize);

    p.setClipping(false);

    // Рисуем обводку циферблата
    p.setPen(QPen(Qt::black, 2));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(0, 0, dialSize, dialSize);

    // Рисуем цифры и деления поверх фона
    p.translate(dialSize/2, dialSize/2);
    p.scale(dialSize/200.0, dialSize/200.0);

    // Поворачиваем систему координат, чтобы 12 было вверху
    p.rotate(-90);

    QFont f = p.font();
    f.setPointSize(10);
    p.setFont(f);
    p.setPen(Qt::black);

    // Деления и цифры (перпендикулярно радиусу)
    for (int i = 0; i < 60; i++) {
        p.save();
        p.rotate(i * 6);

        if (i % 5 == 0) {
            // Большие деления для часов
            p.drawLine(80, 0, 95, 0);

            // Цифры перпендикулярно радиусу
            p.save();
            p.translate(70, 0);

            // Убираем поворот текста, так как мы уже повернули систему координат
            int num = (i/5 == 0) ? 12 : i/5;
            p.drawText(QRect(-10, -10, 20, 20), Qt::AlignCenter, QString::number(num));
            p.restore();
        } else {
            // Малые деления для минут
            p.drawLine(90, 0, 95, 0);
        }

        p.restore();
    }
}

void ClockWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);

    // Фон всего виджета
    p.fillRect(rect(), QColor(240, 240, 240));

    // Определяем размер и позицию круглого циферблата
    int dialSize = qMin(width(), height() - 50);
    if (dialSize <= 0) dialSize = 300;
    int x = (width() - dialSize) / 2;
    int y = 40; // Отступ сверху для панели управления

    // Рисуем циферблат
    if (backgroundLoaded && !dialBackground.isNull()) {
        p.drawPixmap(x, y, dialBackground);
    } else {
        // Стандартный циферблат без фона
        p.save();
        p.translate(x + dialSize/2, y + dialSize/2);
        p.scale(dialSize/200.0, dialSize/200.0);
        // Поворачиваем систему координат, чтобы 12 было вверху
        p.rotate(-90);
        drawDial(&p);
        p.restore();
    }

    // Рисуем стрелки
    p.save();
    p.translate(x + dialSize/2, y + dialSize/2);
    p.scale(dialSize/200.0, dialSize/200.0);
    // Поворачиваем систему координат, чтобы 12 было вверху
    p.rotate(-90);
    drawHands(&p);
    p.restore();
}

void ClockWidget::resizeEvent(QResizeEvent *) {
    updateBackgroundCache();
    update();
}

void ClockWidget::drawDial(QPainter *p) {
    // Круг циферблата
    p->setBrush(Qt::white);
    p->setPen(Qt::black);
    p->drawEllipse(-100, -100, 200, 200);

    QFont f = p->font();
    f.setPointSize(10);
    p->setFont(f);
    p->setPen(Qt::black);

    // Деления и цифры (перпендикулярно радиусу)
    for (int i = 0; i < 60; i++) {
        p->save();
        p->rotate(i * 6);

        if (i % 5 == 0) {
            // Большие деления для часов
            p->drawLine(80, 0, 95, 0);

            // Цифры перпендикулярно радиусу
            p->save();
            p->translate(70, 0);

            // Убираем поворот текста, так как мы уже повернули систему координат
            int num = (i/5 == 0) ? 12 : i/5;
            p->drawText(QRect(-10, -10, 20, 20), Qt::AlignCenter, QString::number(num));
            p->restore();
        } else {
            // Малые деления для минут
            p->drawLine(90, 0, 95, 0);
        }

        p->restore();
    }
}

void ClockWidget::drawHands(QPainter *p) {
    QDateTime utcNow = QDateTime::currentDateTimeUtc();
    QDateTime samaraTime = utcNow.addSecs(4 * 3600); // Добавляем 4 часа для UTC+4
    QTime t = samaraTime.time();

    // Базовый размер для всех стрелок
    const int baseHandLength = 70;

    // Функция для рисования одной стрелки
    auto drawOneHand = [&](double angle, int width, const QColor &fallbackColor, double lengthFactor = 1.0) {
        p->save();
        p->rotate(angle);

        // Фактическая длина стрелки с учетом коэффициента
        int actualLength = baseHandLength * lengthFactor;

        if (!handImg.isNull()) {
            // Используем изображение стрелки с масштабированием
            // Масштабируем с учетом коэффициента длины
            int scaledHeight = actualLength;
            QImage scaledHand = handImg.scaledToHeight(scaledHeight, Qt::SmoothTransformation);

            // Центрируем изображение и позиционируем так, чтобы основание было в центре
            // Смещаем вниз на половину высоты, чтобы центр вращения был внизу изображения
            p->drawImage(-scaledHand.width()/2, -scaledHand.height()/2, scaledHand);
        } else {
            // Рисуем простую линию (резервный вариант)
            p->setPen(QPen(fallbackColor, width, Qt::SolidLine, Qt::RoundCap));
            p->drawLine(0, 0, 0, -actualLength);

            // Круг в центре стрелок
            p->setBrush(fallbackColor);
            p->setPen(Qt::NoPen);
            p->drawEllipse(-width, -width, width*2, width*2);
        }

        p->restore();
    };

    // Коэффициенты длины для разных стрелок (относительно базового размера)
    double hourLengthFactor = 0.7;    // Часовая стрелка - 70% от базового размера
    double minuteLengthFactor = 0.9;  // Минутная стрелка - 90% от базового размера
    double secondLengthFactor = 1.0;  // Секундная стрелка - 100% от базового размера

    // Рисуем стрелки
    // Часовая стрелка
    double hourAngle = 30.0 * (t.hour() % 12) + t.minute() / 2.0;
    drawOneHand(hourAngle, 6, Qt::black, hourLengthFactor);

    // Минутная стрелка
    drawOneHand(6.0 * t.minute(), 4, Qt::black, minuteLengthFactor);

    // Секундная стрелка
    drawOneHand(6.0 * t.second(), 2, Qt::red, secondLengthFactor);

    // Центральный кружок
    p->setBrush(Qt::red);
    p->setPen(Qt::NoPen);
    p->drawEllipse(-4, -4, 8, 8);
}
