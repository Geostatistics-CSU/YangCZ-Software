#include "ColorGradientPreview.h"
#include <QPainter>

ColorGradientPreview::ColorGradientPreview(QWidget* parent) : QWidget(parent), startColor(255, 0, 0, 255), endColor(0, 0, 255, 255), numSteps(10) {}

void ColorGradientPreview::setGradientColors(const QColor& start, const QColor& end) {
    startColor = start;
    endColor = end;

    update();
}

void ColorGradientPreview::setNumSteps(int steps) {
    numSteps = steps;
    update();
}

void ColorGradientPreview::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    int width = this->width();
    int height = this->height();

    // Calculate color gradient
    QVector<QColor> gradient;
    calculateColorGradient(startColor, endColor, numSteps, gradient);

    // Draw color bands
    int bandWidth = width / numSteps;
    for (int i = 0; i < numSteps; ++i) {
        painter.fillRect(i * bandWidth, 0, bandWidth, height, gradient[i]);
    }
}

void ColorGradientPreview::calculateColorGradient(const QColor& start, const QColor& end, int numSteps, QVector<QColor>& gradient) {
    gradient.clear();
    /*
    for (int i = 0; i < numSteps; ++i) {
        int r = start.red() + (end.red() - start.red()) * i / (numSteps - 1);
        int g = start.green() + (end.green() - start.green()) * i / (numSteps - 1);
        int b = start.blue() + (end.blue() - start.blue()) * i / (numSteps - 1);
        int a = start.alpha() + (end.alpha() - start.alpha()) * i / (numSteps - 1);
        gradient.push_back(QColor(r, g, b, a));
    }*/

    // ��ȡ��ʼ�ͽ�����ɫ��HSLֵ
    int startHue = start.hue() == -1 ? 0 : start.hue(); // QColor��hue()���������ɫ����ɫ�ģ����紿�ڻ򴿰ף����᷵��-1
    int endHue = end.hue() == -1 ? 0 : end.hue();
    int startSaturation = start.saturation();
    int startLightness = start.lightness();
    int endSaturation = end.saturation();
    int endLightness = end.lightness();

    for (int i = 0; i < numSteps; ++i) {
        // ���㵱ǰ�����HSLֵ
        // ɫ��ļ�����Ҫ�������·������
        int hueDifference = endHue - startHue;
        int hueShortestDistance = (((hueDifference + 180) % 360) - 180);
        int hue = (startHue + hueShortestDistance * i / (numSteps - 1)) % 360;
        if (hue < 0) hue += 360; // ȷ��ɫ��Ϊ��ֵ

        int saturation = startSaturation + (endSaturation - startSaturation) * i / (numSteps - 1);
        int lightness = startLightness + (endLightness - startLightness) * i / (numSteps - 1);

        // �Ӽ����HSLֵ������ɫ����ӵ�����������
        gradient.push_back(QColor::fromHsl(hue, saturation, lightness));
    }
}
