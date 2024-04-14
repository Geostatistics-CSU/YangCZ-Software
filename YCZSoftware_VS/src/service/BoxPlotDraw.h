#ifndef BOXPLOTWIDGET_H
#define BOXPLOTWIDGET_H

#include <QWidget>
#include <QVector>
#include <cmath>
#include<iostream>

class BoxPlotDraw : public QWidget
{
    Q_OBJECT
public:
    explicit BoxPlotDraw(QWidget* parent = nullptr);

    // ��������
    void setData(const QVector<double>& data);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<double> m_data;

    // �߾�
    int m_margin = 20;

    // ����ͼ��λ�úʹ�С
    QRectF m_boxRect;

    // ��������ͼ��λ�úʹ�С
    double percentile(QVector<double>& data, double percent);
    double mean(const QVector<double> data);
    double standardDeviation(const QVector<double> data);
    double slopheight(double height, double range, double value, double bottomval);
};

#endif // BOXPLOTWIDGET_H
