#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QFileInfoList>

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

    QFileInfoList fileInfoList ;

    QString outDir;

private slots:

    void on_btnRename_clicked();

    void on_btnOpenDirSrc_clicked();

    void on_btnOpenDirDst_clicked();

    void on_btnClear_clicked();

};

#endif // FRMMAIN_H
