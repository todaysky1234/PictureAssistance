#include "frmmain.h"
#include "ui_frmmain.h"

#include <QFileDialog>
#include <QDir>

#include <QFileInfo>
#include <QtDebug>
#include <QDateTime>
#include <QFile>
#include <QCryptographicHash>


struct MD{
    int index;
    QString md5;
};

//bool subMdListSort(const MD *info1, const MD *info2)
//{
//    return info1->md5 < info2->md5;  //升序排列
// //return info1->posid < info2->posid;     //降序排列
//}


bool subMdListSort(const MD info1, const MD info2)
{
    return info1.md5 < info2.md5;  //升序排列
 //return info1->posid < info2->posid;     //降序排列
}

// 仅读取部分内容
QString fileMd5V2(const QString &sourceFilePath) {

    QFile sourceFile(sourceFilePath);


    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    qint64 pos1 = fileSize / 4;
    qint64 pos2 = fileSize / 2;
    qint64 pos3 = fileSize * 3 / 4;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;

        QCryptographicHash hash(QCryptographicHash::Md5);

        sourceFile.seek(pos1);
        bytesRead = sourceFile.read(buffer, bufferSize);
        hash.addData(buffer, bytesRead);

        sourceFile.seek(pos2);
        bytesRead = sourceFile.read(buffer, bufferSize);
        hash.addData(buffer, bytesRead);

        sourceFile.seek(pos3);
        bytesRead = sourceFile.read(buffer, bufferSize);
        hash.addData(buffer, bytesRead);


        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}


frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::on_btnRename_clicked()
{

    int seq = 1000;
    QFileInfo  fileInfo;
    if(this->ui->checkBox->isChecked())
    {// 去重

        QList<MD> metadata;
        int index = 0;
        foreach (fileInfo, fileInfoList) {

            MD data;
            data.index = index++;
            data.md5 = fileMd5V2(fileInfo.absoluteFilePath());
            metadata.append(data);

        }

        // 排序
        qSort(metadata.begin(), metadata.end(), subMdListSort);

        // 需要保留的文件名
        QList<int> sve;
        QString oldMD5;
        index = 0;
        foreach (MD var, metadata) {
            if(oldMD5.isEmpty()){
                oldMD5 = var.md5;
                sve.append(index);
            }else{
                // 判断是否相同，相同不处理
                if(var.md5.compare(oldMD5) !=0)
                {
                    oldMD5 = var.md5;
                    sve.append(index);
                }

            }
            index ++;
        }

        for(int i = 0 ; i < sve.size();i++)
        {
            this->ui->txtLog->append(QString("%1").arg(sve[i]));
            QFileInfo fileInfo1 =  fileInfoList[sve[i]];


            QString newName =  fileInfo1.lastModified().toString("yyyy-MM-dd")+ QString("-%1").arg(seq++)+ "." + fileInfo1.completeSuffix();

            QString fullsrc =fileInfo1.absoluteFilePath();
            QString fullout = outDir+"/" + newName;

//            if(QFileInfo(fullsrc).exists())
//                this->ui->txtLog->append(fullsrc);
//            else
//                this->ui->txtLog->append(fullsrc + " not exist");
            this->ui->txtLog->append(fullsrc+"->" + fullout);
            if(!QFile::copy(fullsrc, fullout))
            {
                continue;
            }
        }


    }else{

        foreach (fileInfo, fileInfoList) {
            qDebug()<< fileInfo.baseName();

            this->ui->txtLog->append(fileInfo.absoluteFilePath());
            qDebug()<< fileInfo.completeSuffix();

            this->ui->txtLog->append(fileInfo.completeSuffix());
            this->ui->txtLog->append(fileInfo.created().toString("yyyy-MM-dd hh-mm-ss") + " -> " +
                                     fileInfo.lastModified().toString("yyyy-MM-dd hh-mm-ss"));


            QString newName =  fileInfo.lastModified().toString("yyyy-MM-dd")+ QString("-%1").arg(seq++)+ "." + fileInfo.completeSuffix();


            QString fullsrc =fileInfo.absoluteFilePath();
            QString fullout = outDir+"/" + newName;
            this->ui->txtLog->append(fullsrc + "->" + fullout);

            if(!QFile::copy(fullsrc, fullout))
            {
                continue;
            }
        }

    }

}

void frmMain::on_btnOpenDirSrc_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    // 读取目录下的照片和视频的文件信息
    this->ui->txtSrcPath->setText(dirName);

    QDir dir(dirName);
    if(!dir.exists())
        return;

    QStringList filters;
    filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp")<<QString("*.mp4");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式（后缀为.jpeg等图片格式）

    // 获取所有文件
    fileInfoList = QDir(dir).entryInfoList();

}

void frmMain::on_btnOpenDirDst_clicked()
{
    outDir =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    this->ui->txtDstPath->setText(outDir);


}

void frmMain::on_btnClear_clicked()
{
    this->ui->txtLog->clear();

}
