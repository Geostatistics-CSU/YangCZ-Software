#pragma once

#include <QDialog>
#include <qgsproject.h>
#include "ui_LoadingXYZDataDialog.h"

#define SEPERATOR_COMMA_TEXT "����(,)"
#define SEPERATOR_SPACE_TEXT "�ո�( )"
#define SEPERATOR_TAB_TEXT "�Ʊ��(\\t)"
#define SEPERATOR_COMMA ","
#define SEPERATOR_SPACE " "
#define SEPERATOR_TAB "\t"

QT_BEGIN_NAMESPACE
namespace Ui { class LoadingXYZDataDialogClass; };
QT_END_NAMESPACE

class LoadingXYZDataDialog : public QDialog
{
	Q_OBJECT

public:
	LoadingXYZDataDialog(QString path, QgsProject* project, QWidget *parent = nullptr);
	~LoadingXYZDataDialog();

private slots:
	void onCmbSeperatorSelectedItemChanged(const QString& text);
	void onBtnComfirmClicked();
private:
	Ui::LoadingXYZDataDialogClass *ui;

	QString _mPath;
	QString _mSeperator;
	QStringList _mColNames;
	QgsProject* _mProject;

	void initUI();
	void createLyrFromXYZ();
	QStringList getColNames(QString firstLine, QString seperator);
	
};
