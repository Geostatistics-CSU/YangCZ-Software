#include "CorrelationMatrixWidget.h"
#include <QPainter>
#include <QPaintEvent>

CorrelationMatrixWidget::CorrelationMatrixWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(400, 300); // ������С�ߴ���ȷ��ɫ�����㹻�Ŀռ���ʾ
}

void CorrelationMatrixWidget::setCorrelationMatrix(const QVector<QVector<double>>& matrix) {
    correlationMatrix = matrix;
    update(); // �����ػ�
}

void CorrelationMatrixWidget::setLabels(const QStringList& labels) {
    this->labels = labels;
    update();
}

void CorrelationMatrixWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // �����

    if (correlationMatrix.isEmpty())
        return;

    int n = correlationMatrix.size();
    int cellWidth = (width() - 2 * margin - 50) / n; // ����50���ؿ�ȸ�ɫ��
    int cellHeight = (height() - 2 * margin) / n;

    // ���ƾ���
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            painter.setFont(QFont("Arial", 8));
            double value = correlationMatrix[i][j];
            QColor color = colorForValue(value);
            QRect cellRect(margin + j * cellWidth, margin + i * cellHeight, cellWidth, cellHeight);
            painter.fillRect(cellRect, color);
            painter.setPen(Qt::black);
            painter.drawText(cellRect, Qt::AlignCenter, QString::number(value, 'f', 2));

            if (i == 0) {
                painter.setFont(QFont("Arial", 8));
                painter.drawText(QRect(margin + j * cellWidth, 15, cellWidth, margin), Qt::AlignCenter, labels[j]);
            }
        }
        painter.setFont(QFont("Arial", 8));
        painter.drawText(QRect(15, margin + i * cellHeight, margin, cellHeight), Qt::AlignCenter, labels[i]);
    }

    // ����ɫ��
    drawColorBar(painter);
}

QColor CorrelationMatrixWidget::colorForValue(double value) const {
    // ӳ����ɫֵ����ɫ (-1) �� ��ɫ (0) ����ɫ (1)
    if (value < 0) {
        // ����ɫ����ɫ
        int t = static_cast<int>((value + 1) * 155);  // value + 1 �� 0 �� 1 �ķ�Χ
        return QColor(100+t, 100+t, 255);
    } else {
        // �Ӱ�ɫ����ɫ
        int t = static_cast<int>(value * 155);  // value �� 0 �� 1 �ķ�Χ
        return QColor(255, 255 - t, 255 - t);
    }
}
void CorrelationMatrixWidget::drawColorBar(QPainter& painter) {
    int barWidth = 20;
    int barHeight = height() - 2 * margin;
    QRect barRect(width() - margin - barWidth, margin, barWidth, barHeight);
    QLinearGradient gradient(barRect.topLeft(), barRect.bottomLeft());

    // ����ɫ����ɫ����ɫ�Ľ���
    gradient.setColorAt(0.0, QColor(255, 100, 100));  // �º͵ĺ�ɫ
    gradient.setColorAt(0.5, QColor(255, 255, 255));  // ��ɫ
    gradient.setColorAt(1.0, QColor(100, 100, 255));  // ��ɫ

    painter.fillRect(barRect, gradient);

    // ���ƿ̶��ߺͱ�ע
    int numTicks = 5;
    painter.setPen(Qt::black);
    for (int i = 0; i < numTicks; ++i) {
        double value = -1 + i * 2.0 / (numTicks - 1); // ����̶ȵ����ϵ��ֵ
        int y = margin + static_cast<int>(barHeight * (i * 1.0 / (numTicks - 1)));

        // ���ƿ̶���
        painter.drawLine(barRect.right() + 1, y, barRect.right() + 5, y);

        // ���Ʊ�ע
        painter.drawText(barRect.right() + 10, y + painter.fontMetrics().height() / 3, QString::number(value, 'f', 1));
    }
}
