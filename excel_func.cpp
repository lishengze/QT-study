#include "excel.h"
#include "xlsxdocument.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "toolfunc.h"
#include "database.h"

QList<QFileInfo> getExcelFileInfo(QString dirName) {
    QDir *dir=new QDir(dirName);
    QStringList filter;
    filter<<"*.xlsx";
    dir->setNameFilters(filter);
    QList<QFileInfo> filaInfoList = dir->entryInfoList(filter);
    return filaInfoList;
}

QList<QString> readExcelSecodeList(QString fileName, QString sheetName,
                                   int startRow, QString style) {
    QList<QString> result;
    QXlsx::Document xlsx(fileName);
    xlsx.selectSheet(sheetName);
    QXlsx::CellRange range = xlsx.dimension();
    int rowCount = range.rowCount();
    int colCount = range.columnCount();
    qDebug() <<"fileName: " << fileName <<"rowCount: " << rowCount << ", colCount" << colCount;

    for (int i = startRow; i <= rowCount; i++){
        result.append(completeSecode(xlsx.cellAt(i, 1)->value().toString(), style));
     }
    return result;
}

QList<QStringList> readExcelData(QString fileName, QString sheetName) {
    QList<QStringList> result;
    QXlsx::Document xlsx(fileName);
    xlsx.selectSheet(sheetName);
    QXlsx::CellRange range = xlsx.dimension();
    int rowCount = range.rowCount();
    int colCount = range.columnCount();
    qDebug() <<"fileName: " << fileName <<"rowCount: " << rowCount << ", colCount" << colCount;

//    int defaultColNumb = 2;
//    if (colCount < defaultColNumb) {
//        defaultColNumb = colCount;
//    }

    for (int i = 1; i <= rowCount; i++){
        QStringList tmpData;
         for (int j = 1; j <= colCount; j++){
             if(int(xlsx.cellAt(i,j)) != 0) {
                 tmpData.append(xlsx.cellAt(i, j)->value().toString());
             } else {
                 break;
             }
         }
         if (tmpData.size() > 0) {
            result.append(tmpData);
         }
     }
    return result;
}

QList<QString> getExcelFileName(QString dirName) {
    QDir *dir=new QDir(dirName);
    QStringList filter;
    filter<<"*.xlsx";
    dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    QList<QString> filaPathList;

//    qDebug() << "fileCount: " <<fileInfo->count ();
    for (int i = 0; i < fileInfo->count (); ++i) {
        filaPathList.append (fileInfo->at(i).filePath());
    }
    return filaPathList;
}

void checkFile(QString fileName) {
    QFile tmpfile(fileName);
    if (tmpfile.exists()) {
        tmpfile.remove();
    }
    QXlsx::Document xlsx;
    xlsx.saveAs(fileName);
}

int writeFileInfo(QString fileName, QString colName, QString value, QString sheetName) {
    QFile tmpfile(fileName);
    if (tmpfile.exists()) {
        QXlsx::Document xlsx(fileName);
        if (!xlsx.selectSheet(sheetName)) {
            if (!xlsx.addSheet(sheetName)) return -2;
            if (!xlsx.selectSheet(sheetName)) return -1;
        }
        xlsx.write(colName, value);
        xlsx.save();
    } else {
        QXlsx::Document xlsx;
        if (!xlsx.selectSheet(sheetName)) {
            if (!xlsx.addSheet(sheetName)) return -2;
            if (!xlsx.selectSheet(sheetName)) return -1;
        }
        xlsx.write(colName, value);
        xlsx.saveAs(fileName);
    }
    return 1;
}


int writeMatrixData(QString fileName, QList<QStringList>& oriData,
                    QString sheetName, bool isTranspose) {
    checkFile(fileName);
    QXlsx::Document xlsx(fileName);

    QStringList sheetNames = xlsx.sheetNames();
    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }
    xlsx.moveSheet(sheetName, 0);
    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

    for (int rowIndex = 0; rowIndex < oriData.length(); ++rowIndex) {
        for (int colIndex = 0; colIndex < oriData[rowIndex].length(); ++colIndex) {
            if (isTranspose) {
              xlsx.write(colIndex+1, rowIndex+1, oriData[rowIndex][colIndex]);
            } else {
              xlsx.write(rowIndex+1, colIndex+1, oriData[rowIndex][colIndex]);
            }
        }
    }
    xlsx.save();
    return 1;
}

int writeOneColData(QString fileName, QStringList oriData, int targetCol, int startRow, QString sheetName) {
    QXlsx::Document xlsx(fileName);
    QStringList sheetNames = xlsx.sheetNames();

    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }
    xlsx.moveSheet(sheetName, 0);
    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

    for (int index = 0; index < oriData.length(); ++index) {
        xlsx.write(startRow+index, targetCol, oriData[index]);
    }
}

int writeOneRowData(QString fileName, QStringList oriData, int targetRow, int startCol,QString sheetName) {
    QXlsx::Document xlsx(fileName);
    QStringList sheetNames = xlsx.sheetNames();

    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }
    xlsx.moveSheet(sheetName, 0);
    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

    for (int index = 0; index < oriData.length(); ++index) {
        xlsx.write(targetRow, startCol + index, oriData[index]);
    }
}

// 补充未上市的数据;
// 删除可能多余的数据;
int completeExcelData(QList<QStringList>& ori_result, QList<QStringList>& index_time_list) {
    QList<QStringList> ori_time_list;
    for (int i = 0; i <ori_result.size(); ++i) {
        QStringList tmp_data;
        tmp_data.append(ori_result[i][0]);
        tmp_data.append(ori_result[i][1]);
        ori_time_list.append(tmp_data);
    }

    // 未上市的数据
    for (int i = 0; i < index_time_list.size(); ++i) {
        if (ori_time_list.indexOf(index_time_list[i]) < 0) {
            QStringList unlist_data;
            unlist_data.append(index_time_list[i][0]);
            unlist_data.append(index_time_list[i][1]);
            for (int j = 2; j < ori_result[0].size(); ++j) {
                unlist_data.append(QString("%1").arg(-1));
            }
            ori_result.insert(i, unlist_data);
        } else {
            break;
        }
    }

    // 可能多余的数据
    for (int i = 0; i < ori_result.size(); ++i) {
        QStringList tmp_data;
        tmp_data.append(ori_result[i][0]);
        tmp_data.append(ori_result[i][1]);
        if (index_time_list.indexOf(tmp_data) < 0) {
            ori_result.removeAt(i);
        }
    }

//    qDebug() << "ori_result: " << ori_result;
    for (int i = 0; i < ori_result.size(); ++i) {
        ori_result[i].removeAt(0);
        ori_result[i].removeAt(0);
    }
//    qDebug() << "ori_result: " << ori_result;
    return 1;
}

QList<QStringList> getIndexTimeList(QString startDate, QString endDate,
                                    QString databaseName, QString dbhost) {
    Database databaseObj("0", dbhost);
    QStringList keyValueList;
    keyValueList << "TDATE" << "TIME";
    QString tableName = "SH000300";
    QList<QStringList> result = databaseObj.getDataByDate(startDate, endDate, keyValueList,
                                                          tableName, databaseName);
    return result;
}

QStringList getIndexTimeStrList(QList<QStringList> indexTimeList) {
    QStringList result;
    for (int i = 0; i < indexTimeList.size(); ++i) {
        result.append(indexTimeList[i].join(" "));
    }
    return result;
}

QList<QString> getSingleColData(QList<QStringList> ori_result, int targetCol) {
    QList<QString> result;

    for (int i = 0; i < ori_result.size(); ++i) {
        if (targetCol >= ori_result[i].size()) {
            break;
        } else {
            result.append(ori_result[i][targetCol]);
        }
    }
    return result;
}
