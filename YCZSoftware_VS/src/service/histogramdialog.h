#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>

namespace Ui {
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent = nullptr);
    ~HistogramDialog();

private:
    Ui::HistogramDialog *ui;
};

#endif // HISTOGRAMDIALOG_H
