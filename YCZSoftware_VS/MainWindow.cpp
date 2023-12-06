#include "MainWindow.h"
#include "./src/macro.h"
#include "./src/service/LoadingXYZDataDialog.h"
#include "LayerMenuProvider.h"

#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(Json::Value config, QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->_mAppConfig = config;
    this->_mConfig = config[MAINWINDOW_2D];
    this->_mHighlight = nullptr;
    this->isProjectOpened = false;
    Py_SetPythonHome(L"E:/Anaconda3/InstallPos/envs/cluster");
    //this->setupEagleEyeMap();
    this->initUi();

}

MainWindow::~MainWindow()
{}

void MainWindow::initUi()
{
    // set window size
    this->setGeometry(this->_mConfig[X_LOCATION].asInt(),
        this->_mConfig[Y_LOCATION].asInt(),
        this->_mConfig[WIDTH].asInt(),
        this->_mConfig[HEIGHT].asInt());


    this->setWindowTitle(this->_mConfig[TITLE].asCString());  // set title

    // create left dock
    this->_mLayerDock = new QgsDockWidget(this);
    this->_mLayerDock->setWindowTitle(this->_mConfig[LAYER_DOCK_TITLE].asCString());
    this->_mLayerDock->setObjectName(this->_mConfig[LAYER_DOCK_OBJ_NAME].asCString());
    this->addDockWidget(Qt::LeftDockWidgetArea, this->_mLayerDock);

    // Set minimum and maximum width for the dock
    this->_mLayerDock->setMinimumWidth(500);  // Adjust with your desired minimum width
    this->_mLayerDock->setMaximumWidth(1000);  // Adjust with your desired maximum width

    // Set minimum and maximum hight for the dock
    this->_mLayerDock->setMinimumHeight(300);  // Adjust with your desired minimum height
    this->_mLayerDock->setMaximumHeight(550);  // Adjust with your desired maximum height

    // create EagleEyeView dock
    this->_mEagleEyeDock = new QgsDockWidget(this);
    this->_mEagleEyeDock->setWindowTitle(this->_mConfig[EagleEye_DOCK_TITLE].asCString());
    this->_mEagleEyeDock->setObjectName(this->_mConfig[EagleEye_DOCK_OBJ_NAME].asCString());
    this->addDockWidget(Qt::LeftDockWidgetArea, this->_mEagleEyeDock);


    // create container of canvas
    this->_mCenterTabWidget = new QTabWidget(this);

    // create 2D canvas
    this->_mCanvas2D = new QgsMapCanvas{};
    this->_mCanvas2D->setCanvasColor(QColor(this->_mConfig[CANVAS_2D_BGC_R].asInt(),
        this->_mConfig[CANVAS_2D_BGC_G].asInt(),
        this->_mConfig[CANVAS_2D_BGC_B].asInt()));
    this->_mCanvas2D->setSelectionColor(QColor(this->_mConfig[CANVAS_2D_STC_R].asInt(),
        this->_mConfig[CANVAS_2D_STC_G].asInt(),
        this->_mConfig[CANVAS_2D_STC_B].asInt()));
    // add canvas to container
    this->_mCenterTabWidget->addTab(this->_mCanvas2D, this->_mConfig[MAP_VIEW_TITLE].asCString());

    this->setCentralWidget(this->_mCenterTabWidget);

    // create layer tree
    this->_mLayerTreeView = new QgsLayerTreeView(this);
    this->_mLayerTreeView->setObjectName(this->_mConfig[LAYER_TREE_OBJ_NAME].asCString());

    //create eagle eye canvas
    this->_mEagleEyeMapOverview = new QgsMapOverviewCanvas(_mEagleEyeDock,_mCanvas2D);

    this->createStatusBar();

    // TODO: bind layer tree model to project, after functions of project is complete
    QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
    model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
    model->setFlag(QgsLayerTreeModel::AllowNodeRename);
    model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
    model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
    model->setAutoCollapseLegendNodes(this->_mConfig[LAYER_TREE_COLLAPSE_NUM].asInt());

    this->_mLayerTreeView->setModel(model);
    this->_mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(),
        this->_mCanvas2D, this);
    this->_mLayerDock->setWidget(this->_mLayerTreeView);
    this->_mEagleEyeDock->setWidget(this->_mEagleEyeMapOverview);

    //new Tool
    _mZooomInTool = new QgsMapToolZoom(_mCanvas2D, false);
    _mZoomOutTool = new QgsMapToolZoom(_mCanvas2D, true);
    _mPanTool = new QgsMapToolPan(_mCanvas2D);

    //layermenuprovider
    _mLayerTreeView->setMenuProvider(new LayerMenuProvider(_mLayerTreeView, _mCanvas2D));

    this->connectFunc();

    this->showMaximized();
}

void MainWindow::connectFunc()
{
    // connect signals and slots
    this->connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::onActionOpen);
    this->connect(ui.actionNew, &QAction::triggered, this, &MainWindow::onActionNew);
    this->connect(ui.actionSave, &QAction::triggered, this, &MainWindow::onActionSave);
    this->connect(ui.actionSaveAs, &QAction::triggered, this, &MainWindow::onActionSaveAs);
    this->connect(ui.actionImportShp, &QAction::triggered, this, &MainWindow::onActionImportShpTriggered);
    this->connect(ui.actionOpen3DWindow, &QAction::triggered, this, &MainWindow::onActionOpen3DWindowTriggered);
    this->connect(ui.actionImportXYZ, &QAction::triggered, this, &MainWindow::onActionImportXYZTriggered);
    this->connect(ui.actionYCZFilter, &QAction::triggered, this, &MainWindow::onActionYCZFilterTriggered);
    this->connect(ui.actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    this->connect(ui.actionPan, &QAction::triggered, this, &MainWindow::onActionPanTriggered);
    this->connect(ui.actionZoomOut, &QAction::triggered, this, &MainWindow::onActionZoomOutTriggered);
    this->connect(ui.actionZoomFull, &QAction::triggered, this, &MainWindow::onActionZoomFullTriggered);
    this->connect(ui.actionZoomIn, &QAction::triggered, this, &MainWindow::onActionZoomInTriggered);
    this->connect(ui.actionUnset, &QAction::triggered, this, &MainWindow::onActionUnsetTriggered);
    this->connect(ui.actionShowTable, &QAction::triggered, this, &MainWindow::onActionShowTableTriggered);
    this->connect(ui.actionClearSelect, &QAction::triggered, this, &MainWindow::onActionClearHighlight);
    this->connect(ui.actionEmitPoint, &QAction::triggered, this, &MainWindow::onActionEmitPoint);
    this->connect(ui.actionFeatureInfo, &QAction::triggered, this, &MainWindow::onActionFeatureInfo);
    this->connect(ui.actionSelectPoint, &QAction::triggered, this, &MainWindow::onActionSelectPoint);
    this->connect(ui.actionSelectVector, &QAction::triggered, this, &MainWindow::onActionFeatureInfo);
    this->connect(ui.actionSerchSQL, &QAction::triggered, this, &MainWindow::onActionSerchSQL);
    this->connect(this->_mCanvas2D, SIGNAL(layersChanged()), this, SLOT(onCanvasRefresh()));
}

bool MainWindow::onActionOpen()
{
    QString projectPath = QFileDialog::getOpenFileName(this, "Select Project File", QDir::homePath(), "QGIS Project Files (*.qgs)");

    if (!projectPath.isEmpty()) {
        QgsProject project;
        project.read(projectPath);
    }
    this->isProjectOpened = true;

    return true;
}

bool MainWindow::onActionNew()
{
    QString projectPath = QFileDialog::getSaveFileName(this, "Select Project Location", QDir::homePath(), "QGIS Project Files (*.qgs)");

    if (!projectPath.isEmpty()) {
        QgsProject project;
        project.setFileName(projectPath);
        project.write();
    }
    this->isProjectOpened = true;

    return true;
}

bool MainWindow::onActionSave()
{
    if (this->isProjectOpened) {
        QgsProject* project = QgsProject::instance();
        project->write();
    }
    else {
        QString projectPath = QFileDialog::getSaveFileName(this, "Save Project As", QDir::homePath(), "QGIS Project Files (*.qgs)");

        if (!projectPath.isEmpty()) {
            QgsProject project;
            project.setFileName(projectPath);
            project.write();
            this->isProjectOpened = true;
        }
    }
    return true;
}

bool MainWindow::onActionSaveAs()
{
    return false;
}

bool MainWindow::onActionImportShpTriggered()
{
    QString title = QString(this->_mConfig[OPEN_VEC_DIALOG_TITLE].asCString());
    QString filterName = QString(this->_mConfig[VEC_FILTER_NAME].asCString());
    QgsProject* project = QgsProject::instance();
    QList<QgsMapLayer*> lyrsToBeAdded = FileLoadingService::loadVecLayers(title, filterName, project, this);
    if (lyrsToBeAdded.size() <= 0) {
        return false;
    }
    project->addMapLayers(lyrsToBeAdded);
    return true;
    
}

bool MainWindow::onActionOpen3DWindowTriggered()
{
    QgsProject* project = QgsProject::instance();
    QVector<QgsVectorLayer*> vecLyrs = project->layers<QgsVectorLayer*>();
    QStringList lyrNameList;
    for (const auto &vecLyr : vecLyrs){
        lyrNameList.append(vecLyr->name());
    }
    
    Lyr3DSelectDialog* lyr3DDialog = new Lyr3DSelectDialog(this->_mAppConfig[LYR_3D_SELECT_DIALOG], lyrNameList, this);
    this->connect(lyr3DDialog, &Lyr3DSelectDialog::sendSelectedFilePath, this, &MainWindow::onSendSelected3DPathEmitted);
    this->connect(lyr3DDialog, &Lyr3DSelectDialog::sendSelectedLyrName, this, &MainWindow::onSendSelected3DLyrEmitted);
    lyr3DDialog->show();

    return true;
}

bool MainWindow::onActionImportXYZTriggered()
{
    QString title = QString(this->_mConfig[OPEN_CSV_DIALOG_TITLE].asCString());
    QString filterName = QString(this->_mConfig[CSV_FILTER_NAME].asCString());
    QStringList fileList = FileLoadingService::openCSVFileSelectDialog(title, filterName, this);
    if (fileList.isEmpty()) {
        return false;
    }

    QString path = fileList.at(0);
    LoadingXYZDataDialog* loadingXYZDialog = new LoadingXYZDataDialog(path, QgsProject::instance(), this);
    loadingXYZDialog->show();
    _mCanvas2D->refresh();
    _mEagleEyeMapOverview->refresh();


    return true;
}

bool MainWindow::onActionYCZFilterTriggered()
{
    QgsProject* project = QgsProject::instance();
    YCZFilterServiceDialog* yczFilterDialog = new YCZFilterServiceDialog(project, this);
    connect(yczFilterDialog, &YCZFilterServiceDialog::sendPyParams, this, &MainWindow::onYCZFilterParamsSended);
    yczFilterDialog->show();
    return false;
}


bool MainWindow::onActionAbout()
{
    AboutSoftDialog* abloutDialog = new AboutSoftDialog(this);
    return true;
}

void MainWindow::onActionClearHighlight()
{
    if (this->_mHighlight)
    {
        delete this->_mHighlight;
    }
    this->_mHighlight = nullptr;

    QList< QgsMapLayer* > 	layers = _mCanvas2D->layers();
    foreach(QgsMapLayer * l, layers)
    {
        QgsVectorLayer* l1 = qobject_cast<QgsVectorLayer*>(l);
        l1->removeSelection();
    }
    this->_mCanvas2D->refresh();
}

void MainWindow::onSendSelected3DLyrEmitted(QString lyrName)
{
    QgsProject* project = QgsProject::instance();
    QVector<QgsVectorLayer*> vecLyrs = project->layers<QgsVectorLayer*>();
    Entity3D entity{};
    for (const auto& vecLyr : vecLyrs) {
        if (vecLyr->name() == lyrName) {
            entity.loadFromVecLyr(*vecLyr);          
        }
    }

    MainWindow3D* mainWindow3D = new MainWindow3D(entity, this->_mAppConfig[MAINWINDOW_3D], this);
    mainWindow3D->show();
}

void MainWindow::onYCZFilterParamsSended(QList<ObPt> obPts, QList<UnobPt> unobPts, QString outputPath)
{
    YCZFilterPyThread* YCZThread = new YCZFilterPyThread(obPts, unobPts, outputPath);
    YCZThread->start();
}

void MainWindow::onActionPanTriggered()
{
    _mCanvas2D->setMapTool(_mPanTool);
}

void MainWindow::onActionZoomInTriggered()
{
    _mCanvas2D->setMapTool(_mZooomInTool);
}

void MainWindow::onActionZoomOutTriggered()
{
    _mCanvas2D->setMapTool(_mZoomOutTool);
}

void MainWindow::onActionZoomFullTriggered()
{
    _mCanvas2D->zoomToFullExtent();
}

void MainWindow::onActionUnsetTriggered()
{
    QgsMapTool* lastMapTool = _mCanvas2D->mapTool();
    _mCanvas2D->unsetMapTool(lastMapTool);
}


void MainWindow::onActionShowTableTriggered()
{
    QModelIndex idx = this->_mLayerTreeView->currentIndex();
    QgsLayerTreeNode* node = this->_mLayerTreeView->index2node(idx);
    if (QgsLayerTree::isLayer(node))
    {
        QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
        if (layer && dynamic_cast<QgsVectorLayer*>(layer)) {
            QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
            this->_mVectorLayerCache = new QgsVectorLayerCache(vectorLayer, vectorLayer->featureCount());
            this->_mAttrTableModel = new QgsAttributeTableModel(_mVectorLayerCache);
            this->_mAttrTableModel->loadLayer();
            _mAttrTableFilterModel = new QgsAttributeTableFilterModel(this->_mCanvas2D, this->_mAttrTableModel);
            _mAttrTableFilterModel->setSourceModel(_mAttrTableModel);

            QgsAttributeTableView* pView = new QgsAttributeTableView(this);
            
            pView->setSelectionBehavior(QAbstractItemView::SelectRows);
            pView->setSelectionMode(QAbstractItemView::ExtendedSelection);

            pView->setWindowModality(Qt::WindowModality::WindowModal);
            pView->setWindowFlag(Qt::Window);
            pView->setWindowTitle("LayerTable");
            pView->setModel(_mAttrTableFilterModel);
            pView->resize(700, 450);
            //pView->setSelectionBehavior(QAbstractItemView::SelectRows);

            //QItemSelectionModel* selectionModel = pView->selectionModel();
            //QItemSelectionModel* selectionModel = new QItemSelectionModel(_mAttrTableModel);
            //pView->setSelectionModel(selectionModel);
            //pView->setSelectionBehavior(QAbstractItemView::SelectItems);
            //pView->setSelectionBehavior(QAbstractItemView::SelectRows);
            //pView->setSelectionBehavior(QAbstractItemView::SelectColumns);
            /*
            connect(selectionModel, &QItemSelectionModel::selectionChanged, [=](const QItemSelection& selected, const QItemSelection& deselected) {

                QModelIndexList selectedIndexes = selected.indexes();

                QModelIndexList deselectedIndexes = deselected.indexes();

                for (const QModelIndex& index : selectedIndexes) {
                    pView->model()->setData(index, QBrush(Qt::blue), Qt::BackgroundRole); // ����Ը�����ɫ
                }

                for (const QModelIndex& index : deselectedIndexes) {
                    pView->model()->setData(index, QVariant(), Qt::BackgroundRole);
                }
                });*/

            this->_mLayerTreeView->setCurrentIndex(QModelIndex());

            pView->show();

        }
        else {
            QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
        } 
    }
    else {
        QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
    }
}

void MainWindow::onActionFeatureInfo()
{
    QModelIndex idx = this->_mLayerTreeView->currentIndex();
    QgsLayerTreeNode* node = this->_mLayerTreeView->index2node(idx);
    if (QgsLayerTree::isLayer(node))
    {
        QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
        if (layer && dynamic_cast<QgsVectorLayer*>(layer)) 
        {
            QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
            this->_mToolIdentifyFeature = new QgsMapToolIdentifyFeature(this->_mCanvas2D);
            this->_mToolIdentifyFeature->setLayer(vectorLayer);
            QObject::connect(this->_mToolIdentifyFeature, static_cast<void (QgsMapToolIdentifyFeature::*)(const QgsFeature&)>
                (&QgsMapToolIdentifyFeature::featureIdentified), [=](const QgsFeature& feature) {
                onActionClearHighlight();

                this->_mHighlight = new QgsHighlight(_mCanvas2D, feature.geometry(), vectorLayer);
                // DEFAULT_HIGHLIGHT_COLOR = QColor( 255, 0, 0, 128 )
                QColor color = QColor(Qgis::DEFAULT_HIGHLIGHT_COLOR.name());
                color.setAlpha(Qgis::DEFAULT_HIGHLIGHT_COLOR.alpha());
                this->_mHighlight->setColor(color);	
                this->_mHighlight->setFillColor(color);	
                // DEFAULT_HIGHLIGHT_BUFFER_MM = 0.5
                this->_mHighlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);	
                // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
                this->_mHighlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);
                QString msg;
                QgsFields fields = feature.fields();	
                for (int i = 0; i < fields.count(); i++)
                {
                    msg += QString("%1: %2\n").arg(fields.at(i).name(), feature.attribute(i).toString());
                }
                QMessageBox::information(this, "QgsMapToolIdentifyFeature", msg);
                });
            this->_mCanvas2D->setMapTool(_mToolIdentifyFeature);
        }
        else
        {
            QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
        }
    }
    else {
        QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
    }
}

void MainWindow::onActionEmitPoint()
{
    this->_mToolEmitPoint = new QgsMapToolEmitPoint(this->_mCanvas2D);
    QObject::connect(_mToolEmitPoint, &QgsMapToolEmitPoint::canvasClicked, [=](const QgsPointXY& point, Qt::MouseButton button) {
        QMessageBox::information(this, "QgsMapToolEmitPoint", QString("X: %1\nY: %2").arg(QString::number(point.x()), QString::number(point.y())));
        });
    this->_mCanvas2D->setMapTool(_mToolEmitPoint);
}

void MainWindow::onActionSelectPoint()
{
    QModelIndex idx = this->_mLayerTreeView->currentIndex();
    QgsLayerTreeNode* node = this->_mLayerTreeView->index2node(idx);
    if (QgsLayerTree::isLayer(node))
    {
        QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
        if (layer && dynamic_cast<QgsVectorLayer*>(layer))
        {
            QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
            this->_mToolIdentifyFeature = new QgsMapToolIdentifyFeature(this->_mCanvas2D);
            this->_mToolIdentifyFeature->setLayer(vectorLayer);
            QObject::connect(this->_mToolIdentifyFeature, static_cast<void (QgsMapToolIdentifyFeature::*)(const QgsFeature&)>
                (&QgsMapToolIdentifyFeature::featureIdentified), [=](const QgsFeature& feature) {
                    //onActionClearHighlight();

                    this->_mHighlight = new QgsHighlight(_mCanvas2D, feature.geometry(), vectorLayer);
                    // DEFAULT_HIGHLIGHT_COLOR = QColor( 255, 0, 0, 128 )
                    QColor color = QColor(Qgis::DEFAULT_HIGHLIGHT_COLOR.name());
                    color.setAlpha(Qgis::DEFAULT_HIGHLIGHT_COLOR.alpha());
                    this->_mHighlight->setColor(color);
                    this->_mHighlight->setFillColor(color);
                    // DEFAULT_HIGHLIGHT_BUFFER_MM = 0.5
                    this->_mHighlight->setBuffer(Qgis::DEFAULT_HIGHLIGHT_BUFFER_MM);
                    // DEFAULT_HIGHLIGHT_MIN_WIDTH_MM = 1.0
                    this->_mHighlight->setMinWidth(Qgis::DEFAULT_HIGHLIGHT_MIN_WIDTH_MM);
                });
            this->_mCanvas2D->setMapTool(_mToolIdentifyFeature);
        }
        else
        {
            QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
        }
    }
    else {
        QMessageBox::information(this, tr("Information"), tr("NO LAYER IS SELECTED"));
    }
}

void MainWindow::onActionSelectVector()
{

}

void MainWindow::onActionSerchSQL()
{
    SQLDialog* SQL = new SQLDialog();
}

void MainWindow::onCanvasRefresh()
{
    QList<QgsMapLayer*> mlayers = _mCanvas2D->layers();
    _mEagleEyeMapOverview->setLayers(mlayers);
    _mEagleEyeMapOverview->refresh();
}

void MainWindow::onClickSQLSearch()
{/*
    // ��ȡ����Ĳ�ѯ���
    QString whereClause = ui.actionSerchSQL->text();
    // ���õ�ͬʱ����в�ѯ�������ѯ���ִ����򷵻�false
    if (!mpVectorLayer->setSubsetString(whereClause))
    {
        // ����в�ѯ���ִ���
        if (mpVectorLayer->dataProvider()->hasErrors())
        {
            // ��ʾ��¼�Ĵ�����Ϣ
            QMessageBox::warning(this, u8"����", QString(u8"��ѯִ�д��󡣷�����Ϣ��\n\n%1").arg(mpVectorLayer->dataProvider()->errors().join("\n")));
            // �����¼�Ĵ�����Ϣ
            mpVectorLayer->dataProvider()->clearErrors();
        }
        // �����ѯ�ַ�������ʧ�ܣ�����û�д���˵�����ù��˱��ʽʧ��
        else
        {
            QMessageBox::warning(this, u8"����", u8"���ù��˱��ʽʧ�ܡ�");
        }
    }*/
}


//void MainWindow::onActionMeasureTriggered()
//{
//}



void MainWindow::onSendSelected3DPathEmitted(QString path)
{
    /*MainWindow3D* mainWindow3D = new MainWindow3D(this->_mAppConfig[MAINWINDOW_3D], this);
    mainWindow3D->show();*/
}

void MainWindow::createStatusBar()
{
    statusBar()->setStyleSheet("QStatusBar::item {border: none;}");

    //! ��ӽ�����
    m_progressBar = new QProgressBar(statusBar());
    m_progressBar->setObjectName("m_progressBar");
    m_progressBar->setMaximum(100);
    m_progressBar->hide();
    statusBar()->addPermanentWidget(m_progressBar, 1);
    //connect(this->_mCanvas2D, SIGNAL(renderStarting()), this, SLOT(canvasRefreshStarted()));
    //connect(this->_mCanvas2D, SIGNAL(mapCanvasRefreshed()), this, SLOT(canvasRefreshFinished()));

    QFont myFont("Arial", 9);
    statusBar()->setFont(myFont);

    //! ���������ʾ��ǩ
    m_coordsLabel = new QLabel(QString(), statusBar());
    m_coordsLabel->setObjectName("m_coordsLabel");
    m_coordsLabel->setFont(myFont);
    m_coordsLabel->setMinimumWidth(10);
    m_coordsLabel->setMargin(3);
    m_coordsLabel->setAlignment(Qt::AlignCenter);
    m_coordsLabel->setFrameStyle(QFrame::NoFrame);
    m_coordsLabel->setText(tr("Coordinate:"));
    m_coordsLabel->setToolTip(tr("Current map coordinate"));
    statusBar()->addPermanentWidget(m_coordsLabel, 0);

    //! ����༭��ǩ
    m_coordsEdit = new QLineEdit(QString(), statusBar());
    m_coordsEdit->setObjectName("m_coordsEdit");
    m_coordsEdit->setFont(myFont);
    m_coordsEdit->setMinimumWidth(10);
    m_coordsEdit->setMaximumWidth(300);
    m_coordsEdit->setContentsMargins(0, 0, 0, 0);
    m_coordsEdit->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_coordsEdit, 0);
    //connect(m_coordsEdit, SIGNAL(returnPressed()), this, SLOT(userCenter()));

    //! �����߱�ǩ
    m_scaleLabel = new QLabel(QString(), statusBar());
    m_scaleLabel->setObjectName("m_scaleLabel");
    m_scaleLabel->setFont(myFont);
    m_scaleLabel->setMinimumWidth(10);
    m_scaleLabel->setMargin(3);
    m_scaleLabel->setAlignment(Qt::AlignCenter);
    m_scaleLabel->setFrameStyle(QFrame::NoFrame);
    m_scaleLabel->setText(tr("Scale"));
    m_scaleLabel->setToolTip(tr("Current map scale"));
    //statusBar()->addPermanentWidget(m_scaleLabel, 0);

    m_scaleEdit = new QgsScaleComboBox(statusBar());
    m_scaleEdit->setObjectName("m_scaleEdit");
    m_scaleEdit->setFont(myFont);
    m_scaleEdit->setMinimumWidth(10);
    m_scaleEdit->setMaximumWidth(100);
    m_scaleEdit->setContentsMargins(0, 0, 0, 0);
    m_scaleEdit->setToolTip(tr("Current map scale (formatted as x:y)"));
    statusBar()->addPermanentWidget(m_scaleEdit, 0);
    //connect(m_scaleEdit, SIGNAL(scaleChanged()), this, SLOT(userScale()));

}



