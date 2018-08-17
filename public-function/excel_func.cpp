#include "excel.h"
#include "xlsxdocument.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "secode_func.h"
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

QMap<QString, double> readExcelMapDouble(QString fileName, QString sheetName) {
    QList<QStringList> oridata = readExcelData(fileName, sheetName);
    QMap<QString, double> result;
    for (int i = 0; i < oridata.size(); ++i) {
        if (oridata[i].size() < 2) {
            result.insert("Error", -1);
            break;
        } else {
            for (int j = 0; j < 2; ++j) {
                result.insert(oridata[i][0], oridata[i][1].toDouble());
            }
        }
    }
    return result;
}

QMap<QString, int> readExcelMapInt(QString fileName, QString sheetName) {
    QList<QStringList> oridata = readExcelData(fileName, sheetName);
    QMap<QString, int> result;
    for (int i = 0; i < oridata.size(); ++i) {
        if (oridata[i].size() < 2) {
            result.insert("Error", -1);
            break;
        } else {
            for (int j = 0; j < 2; ++j) {
                result.insert(oridata[i][0], oridata[i][1].toInt());
            }
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
    qDebug() << "fileName: " << fileName;
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
//    qDebug() << "writeMatrixData";
    checkFile(fileName);
    QXlsx::Document xlsx(fileName);

//    qDebug() << 0;
    QStringList sheetNames = xlsx.sheetNames();
    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }

//    qDebug() << 1;
    xlsx.moveSheet(sheetName, 0);
    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

//    qDebug() << "oriData: " << oriData;
    for (int rowIndex = 0; rowIndex < oriData.length(); ++rowIndex) {
        for (int colIndex = 0; colIndex < oriData[rowIndex].length(); ++colIndex) {
            if (isTranspose) {
              xlsx.write(colIndex+1, rowIndex+1, oriData[rowIndex][colIndex]);
            } else {
              xlsx.write(rowIndex+1, colIndex+1, oriData[rowIndex][colIndex]);
            }
        }
    }
//    qDebug() << 2;
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
int completeExcelData(QList<QStringList>& ori_result, QList<QStringList>& index_time_list, int keyCount) {
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
            for (int j = 2; j < keyCount; ++j) {
                unlist_data.append(QString("%1").arg(-1));
            }
            ori_result.insert(i, unlist_data);
        } else {
            break;
        }
    }

    // 补充停牌的数据
    for (int i = 0; i < index_time_list.size(); ++i) {
        if (ori_result[i][0] != index_time_list[i][0] || ori_result[i][1] != index_time_list[i][1]) {
            QStringList delist_data;
            delist_data.append(index_time_list[i][0]);
            delist_data.append(index_time_list[i][1]);
            delist_data.append(QString("%1").arg(ori_result[i-1][2]));
            ori_result.insert(i, delist_data);
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

    for (int i = 0; i < ori_result.size(); ++i) {
        ori_result[i].removeAt(0);
        ori_result[i].removeAt(0);
    }
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

void initExcelFile(QString fileName) {
    QFile currFile(fileName);
    if (!currFile.exists()) {
        QXlsx::Document initxlsx;
        initxlsx.saveAs(fileName);

        QXlsx::Document xlsx(fileName);
        QStringList sheetNames = xlsx.sheetNames();
//        qDebug() << "init sheetNames: " << sheetNames;
        for (QString sheetname : sheetNames) {
            if (xlsx.deleteSheet(sheetname)) {
//                qDebug() << "delete " << sheetname << " successfully!";
            } else {
//                qDebug() << "delete " << sheetname << " failed!";
            }
        }
        xlsx.save();
    }
}

int writePortfolio(QMap<QString, double> portfolio, QString fileName,
                    QString accountName, QString type, QString sheetName) {
    initExcelFile(fileName);

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

    if (accountName == "ori" || accountName == "") {
        int index = 1;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it, ++index) {
            xlsx.write(index, 1, it.key());
            xlsx.write(index, 2, it.value());
        }
    }

    if (accountName.indexOf("1") >= 0) {
        xlsx.write("A1", QString::fromLocal8Bit("买卖"));
        xlsx.write("B1", QString::fromLocal8Bit("证券代码"));
        xlsx.write("C1", QString::fromLocal8Bit("数量"));

        QString keyWord;
        if (type == "buy") {
            keyWord =  QString::fromLocal8Bit("买入");
        } else if (type == "sale") {
            keyWord =  QString::fromLocal8Bit("卖出");
        }

        int index = 2;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it) {
            xlsx.write(QString("A%1").arg(index), keyWord);
            xlsx.write(QString("B%1").arg(index), getCompleteSecode(it.key(), "tinysoft", "lower"));
            xlsx.write(QString("C%1").arg(index), it.value());
            ++index;
        }
    }

    if (accountName.indexOf("2") >= 0) {
        xlsx.write("A1", QString::fromLocal8Bit("证券代码"));
        xlsx.write("B1", QString::fromLocal8Bit("证券名称"));
        xlsx.write("C1", QString::fromLocal8Bit("交易市场"));
        xlsx.write("D1", QString::fromLocal8Bit("委托方向"));
        xlsx.write("E1", QString::fromLocal8Bit("价格类型"));
        xlsx.write("F1", QString::fromLocal8Bit("委托价格"));
        xlsx.write("G1", QString::fromLocal8Bit("委托数量"));
        xlsx.write("H1", QString::fromLocal8Bit("委托金额"));

        QString keyWord;
        if (type == "buy") {
            keyWord = "1";
        } else if (type == "sale") {
            keyWord = "2";
        }
//        qDebug() << accountName << ", keyWord: " << keyWord;
        int index = 2;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it) {
            xlsx.write(QString("A%1").arg(index), getSimpleSecode(it.key()));
            xlsx.write(QString("D%1").arg(index), keyWord);
            xlsx.write(QString("G%1").arg(index), it.value());
            ++index;
        }
    }

    xlsx.save();

//    qDebug() << "sheetNames: " << xlsx.sheetNames();

    return 1;
}

QString readlFileData(QString fileName, int row, int col,
                      QString sheetName) {
    QXlsx::Document xlsx(fileName);
    if (!xlsx.selectSheet(sheetName)) {
        return "NULL";
    } else {
        if (xlsx.cellAt(row, col) != 0) {
            return xlsx.cellAt(row, col)->value().toString();
        } else {
            return "NULL";
        }
    }
}

double getPortfolioAmount(QMap<QString, double> priceMap, QMap<QString, double> porfolioMap) {
    double sum = 0.0;
    for (QMap<QString, double>::iterator it = porfolioMap.begin();
         it != porfolioMap.end(); ++it) {
        sum += priceMap[it.key()] * it.value();
    }
    return sum;
}

QMap<QString, QString> getPortfolioAmountMap(QList<QFileInfo> fileInfoList) {
    QMap<QString, QString> result;
    for (QFileInfo tmpFileInfo : fileInfoList) {
        result.insert(tmpFileInfo.fileName(), readlFileData(tmpFileInfo.filePath(), 1, 1, "Sheet2"));
    }
    return result;
}


