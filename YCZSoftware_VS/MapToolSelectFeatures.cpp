#include "MapToolSelectFeatures.h"
#include"QgsMapToolIdentify.h"
#include "qgsapplication.h"
#include "qgsvectorlayer.h"

#include <qgsfeature.h>
#include <qgsvectorlayer.h>
#include <qgsrectangle.h>
#include <QMouseEvent>
#include <QMessageBox>



MapToolSelectFeatures::MapToolSelectFeatures(QgsMapCanvas* mapCanvas)
    : QgsMapToolIdentify(mapCanvas)
{

}
void MapToolSelectFeatures::canvasMoveEvent(QgsMapMouseEvent* e)
{
    if (e->buttons() != Qt::LeftButton)
        return;
    QRect rect;
    if (!mSelectionActive)
    {
        mSelectionActive = true;
        rect = QRect(e->pos(), e->pos());
    }
    else
    {
        rect = QRect(e->pos(), mInitDragPos);
    }
    if (mSelectionRubberBand)
        mSelectionRubberBand->setToCanvasRectangle(rect);
}

void MapToolSelectFeatures::canvasPressEvent(QgsMapMouseEvent* e)
{
    if (!mSelectionRubberBand)
        initRubberBand();
    mInitDragPos = e->pos();
}

void MapToolSelectFeatures::canvasReleaseEvent(QgsMapMouseEvent* e)
{
    QPoint point = e->pos() - mInitDragPos;
    //��
    if (!mSelectionActive || (point.manhattanLength() < QApplication::startDragDistance()))
    {
        mSelectionActive = false;
        mSelectGeometry = QgsGeometry::fromPointXY(toMapCoordinates(e->pos()));
        identifyFromGeometry();
    }
    //����
    if (mSelectionRubberBand && mSelectionActive)
    {
        mSelectGeometry = mSelectionRubberBand->asGeometry();
        mSelectionRubberBand.reset();
        identifyFromGeometry();

    }
    mSelectionActive = false;



}

void MapToolSelectFeatures::initRubberBand()
{
    //mSelectionRubberBand.reset(new QgsRubberBand(mCanvas, QgsWkbTypes::PolygonGeometry));
    mSelectionRubberBand->setFillColor(mFillColor);
    mSelectionRubberBand->setStrokeColor(mStrokeColor);
}
void MapToolSelectFeatures::identifyFromGeometry()
{
    if (mCanvas)
    {
        mCanvas->setSelectionColor(Qt::red);//������ɫ
        QList< QgsMapLayer* > 	layers = mCanvas->layers();
        foreach(QgsMapLayer * l, layers)
        {
            QgsVectorLayer* l1 = qobject_cast<QgsVectorLayer*>(l);
            l1->removeSelection();
        }
    }
    //����ѡ�еĽ��
    QList<IdentifyResult> results = QgsMapToolIdentify::identify(mSelectGeometry, IdentifyMode::TopDownAll, AllLayers);
    //ѡ���Features����
    QList<QgsFeature> selectFeatures;
    //��ʾ����
    for (int i = 0; i < results.count(); ++i)
    {
        QgsVectorLayer* layer = qobject_cast<QgsVectorLayer*>(results.at(i).mLayer);
        QgsFeatureIds ids;
        for (IdentifyResult var : results)
        {
            QgsFeature _Feature = var.mFeature;
            ids.insert(_Feature.id());
        }
        if (ids.count() > 0)
            layer->selectByIds(ids);
        QgsFeature feature = results.at(i).mFeature;
        selectFeatures.append(feature);
    }
    //����ѡ�е�Feature��Ϣ�ź�
    if (selectFeatures.count() > 0)
        emit sigSelectFeatureChange(selectFeatures);
}

