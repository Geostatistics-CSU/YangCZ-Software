// colorwidget.cpp
#include "ColorWidget.h"
#include <QPainter>

ColorWidget::ColorWidget(QWidget* parent) : QWidget(parent), m_color(Qt::black)
{
    setFixedSize(150, 45); // ���ÿؼ��̶���С
}

void ColorWidget::setColor(const QColor& color)
{
    if (m_color != color) {
        m_color = color;
        update(); // ���¿ؼ��Ա��ػ�
    }
}

void ColorWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), m_color); // ��ָ����ɫ���ؼ�����
}
