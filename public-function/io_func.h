#ifndef IO_FUNC_H
#define IO_FUNC_H

#include <QString>
#include <QWidget>
#include <QFileInfo>
#include <QDir>

QString removePathName(QString fullFileName);

void ErrorMessage(QString msg);

void WarnMessage(QWidget* window, QString title, QString msg);

void OutputMsg(QWidget* window,QString msg);

QList<QFileInfo> getDirName(QString dirName);

template<class valueType>
void printList(QList<valueType> data, QString message) {
    qDebug() << "message: " << message << ", datasize: " <<  data.size();
    for (int i = 0; i < data.size(); ++i) {
        qDebug() << data[i];
    }
}

template<class keyType, class valueType>
void printMap(QMap<keyType, valueType> data, QString message) {
    qDebug() << "message: " << message << ", datasize: " << data.size();
    for ( QMap<keyType, valueType>::iterator it = data.begin();
          it != data.end(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }
}


#endif // IO_FUNC_H
