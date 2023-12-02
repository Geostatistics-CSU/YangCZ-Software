#ifndef QGSMAPTOOLSELECTFEATURES_H
#define QGSMAPTOOLSELECTFEATURES_H

#endif // QGSMAPTOOLSELECTFEATURES_H
#include <QObject>
#include <QList>
#include <QScopedPointer>
#include <qmath.h>
#include <qgsmapcanvas.h>
#include <qgsmaptoolidentify.h>
#include <qgsrubberband.h>
#include <qgsmapmouseevent.h>
#include <qgsgeometry.h>

class MapToolSelectFeatures : public QgsMapToolIdentify
{
    Q_OBJECT
public:
    MapToolSelectFeatures(QgsMapCanvas* mapCanvas);
protected:
    //��д����ƶ�
    void canvasMoveEvent(QgsMapMouseEvent* e) override;
    //��д��갴��
    void canvasPressEvent(QgsMapMouseEvent* e) override;
    //��д���̧��
    void canvasReleaseEvent(QgsMapMouseEvent* e) override;

    void initRubberBand();
    void identifyFromGeometry();
signals:
    //����ѡ�е�Ҫ���ź�
    void sigSelectFeatureChange(QList<QgsFeature>);
private:
    // �Ƿ�����ѡ��
    bool mSelectionActive = false;
    QScopedPointer<QgsRubberBand> mSelectionRubberBand;
    QColor mFillColor = QColor(254, 178, 76, 63);
    QColor mStrokeColor = QColor(254, 58, 29, 100);
    QPoint mInitDragPos;
    //ѡ���Geometry
    QgsGeometry mSelectGeometry;
};
