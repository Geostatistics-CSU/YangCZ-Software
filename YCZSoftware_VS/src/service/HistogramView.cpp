#include "HistogramView.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

HistogramView::HistogramView(QWidget* parent) : QWidget(parent) {
}

void HistogramView::setData(const QVector<double>& data, int numbers) {
    m_data = data;
    m_number = numbers;
    update(); // �������»��ƿؼ�
}

void HistogramView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    if (m_data.isEmpty())
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        // Ϊ������ͱ�ǩԤ���ռ�
        const int margin = 40;
        int width = this->width() - 2 * margin + 30;
        int height = this->height() - 2 * margin + 20;

        // ����������
        //painter.drawLine(margin, height + margin, width + margin, height + margin); // X��
        //painter.drawLine(margin, margin, margin, height + margin); // Y��
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // �����

    // ���Ʊ���
    painter.fillRect(rect(), Qt::white);

    // Ϊ������ͱ�ǩԤ���ռ�
    const int margin = 40;
    int width = this->width() - 2 * margin+30;
    int height = this->height() - 2 * margin;

    // ����������
    painter.drawLine(margin, height + margin, width + margin, height + margin); // X��
    painter.drawLine(margin, margin, margin, height + margin); // Y��

    // �������ݷ�Χ�ͷֶδ�С
    double min = *std::min_element(m_data.begin(), m_data.end());
    double max = *std::max_element(m_data.begin(), m_data.end());
    double range = max - min;
    if (range == 0) range = 1; // �������0���������ֵ��ͬ������Ϊ��ΧΪ1

    QVector<int> frequency(m_number, 0);
    double segmentSize = range / m_number;

    // ����Ƶ��
    int maxFrequency = 0;
    for (double value : m_data) {
        int index = std::min(static_cast<int>((value - min) / segmentSize), m_number - 1);
        frequency[index]++;
        maxFrequency = std::max(maxFrequency, frequency[index]);
    }

    // ����ֱ��ͼ
    for (int i = 0; i < m_number; ++i) {
        double rectWidth = static_cast<double>(width) / m_number;
        double rectHeight = (static_cast<double>(frequency[i]) / maxFrequency) * height;
        QRectF bar(margin + i * rectWidth, height + margin - rectHeight, rectWidth, rectHeight);
        painter.fillRect(bar, Qt::blue);
        painter.drawRect(bar);

    }

    //painter.drawText(0, margin+heiht/4, margin - 5, 20, Qt::AlignRight, label);

    QFont font = painter.font();
    font.setPointSize(9); // �����ֺ�Ϊ10
    painter.setFont(font);
// ��עY��Ƶ��
    for (float i = 0; i <= 4; i++) { // �����Ƶ�ʵ��ķ�֮һΪ��������Y���ǩ
        double height_i = height / 4 * i; // ���Ǹ������ڻ����ϵĴ�ֱλ��
        double freqRatio_i = maxFrequency * (i / 4); // ��ʵ�����Ƿֶ���������Ƶ��
        double proportion_i = freqRatio_i / m_data.size(); // ת��Ϊ��Ӧ�ı���

        // ע�⣺�ڽ�Ƶ��ת��ΪƵ�ʣ���������ʱ������ʹ����������������ÿһ���ֵı���
        QString label = QString::number(proportion_i, 'g', 2); // ʹ��'g'��ʽ�����Ա��ⲻ��Ҫ��0

        // ע�����y����ļ��㣬��ȷ����ǩ��ȷ����
        int y = margin + height - static_cast<int>(height_i);
        painter.drawText(0, y - 10, margin - 3, 20, Qt::AlignRight, label);
    }

    // ���Ƹ����ߺ�Y���ǩ
    QPen gridPen(Qt::gray, 0, Qt::DashLine);
    painter.setPen(gridPen);
    for (float i = 0; i <= 4; i++) { // �����Ƶ�ʵ��ķ�֮һΪ��������Y���ǩ
        double height_i = height / 4 * i;
        double frequency_i = maxFrequency / 4 * i;
        painter.drawLine(margin, margin + height - height_i, width + margin, margin + height - height_i);
    }
    painter.setPen(Qt::black);
    // X����Сֵ�����ֵ�ı�ע
    painter.drawText(margin-15, height + margin + 15, QString::number(min, 'f', 2));
    painter.drawText(width + margin - 40, height + margin + 15, QString::number(max, 'f', 2)); // ȷ���ı����һ���
}
