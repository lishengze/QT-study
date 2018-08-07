#include "io_func.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

QString removePathName(QString fullFileName) {
    QStringList tmpList = fullFileName.split ("/");
    QString fileName = tmpList.at(tmpList.size ()-1);
    fileName.remove(".xlsx");
    return fileName;
}

void ErrorMessage(QString msg) {
    qDebug() << msg;
}

void WarnMessage(QWidget* window, QString title, QString msg) {
    QMessageBox::warning(window, title, msg);
}

void OutputMsg(QWidget* window,QString msg) {
    QMessageBox::information(window, "TableDataNumb", msg);
}

QList<QFileInfo> getDirName(QString dirName) {
    QDir *dir=new QDir(dirName);
    dir->setFilter(QDir::Dirs|QDir::Files);
    dir->setSorting(QDir::DirsFirst);
    QFileInfoList fileInfo = dir->entryInfoList();

//    qDebug() << "fileInfo.size: " << fileInfo.size();
    QList<QFileInfo> dirList;

    for (int i = 0; i < fileInfo.count (); ++i) {
        if (fileInfo.at(i).isDir()) {
            dirList.append (fileInfo.at(i));
        }
    }
    return dirList;
}
