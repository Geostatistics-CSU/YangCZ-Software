#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QVector>

class HistogramView : public QWidget {
    Q_OBJECT

public:
    explicit HistogramView(QWidget* parent = nullptr);

    // ��������
    void setData(const QVector<double>& data, int numbers);

protected:
    // ��д�����¼�
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<double> m_data; // �洢ֱ��ͼ����
    int m_number;
};

#endif // HISTOGRAMWIDGET_H
