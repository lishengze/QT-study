#include "secode_func.h"
#include "database.h"
#include "id_func.h"
#include <QList>

bool isCodeInDatabase(QString codeName, QString databaseName, QString dbHost) {
    Database database_obj = Database("0", dbHost);
    QStringList tableList = database_obj.getTableList(databaseName);
    if (tableList.indexOf(codeName) >= 0) {
        return true;
    } else {
        return false;
    }

}

QString completeSecode(QString secode, QString style) {
    if (secode.size() > 6) {
        return secode;
    }

    while (secode.size() < 6) {
        secode.prepend("0");
    }

    if (style == "tinysoft") {
        if (secode.startsWith("6")) {
            secode.prepend("SH");
        } else {
            secode.prepend("SZ");
        }
    }

    if (style == "wind") {
        if (secode.startsWith("6")) {
            secode.append(".SH");
        } else {
            secode.append(".SZ");
        }
    }

    return secode;
}

QString getCompleteSecode(QString secode, QString style, QString letterCase) {
    while (secode.size() < 6) {
        secode.prepend("0");
    }

    if (secode.length() > 6) {
        return secode;
    }

    if (style == "tinysoft") {
        if (secode.startsWith("6")) {
            if (letterCase == "capital") {
                secode.prepend("SH");
            } else if (letterCase == "lower") {
                secode.prepend("sh");
            }
        } else {
            if (letterCase == "capital") {
                secode.prepend("SZ");
            } else if (letterCase == "lower") {
                secode.prepend("sz");
            }
        }
    }

    if (style == "wind") {
        if (secode.startsWith("6")) {
            if (letterCase == "capital") {
                secode.append(".SH");
            } else if (letterCase == "lower") {
                secode.append(".sh");
            }
        } else {
            if (letterCase == "capital") {
                secode.append(".SZ");
            } else if (letterCase == "lower") {
                secode.append(".sz");
            }
        }
    }

    return secode;
}

QString getWindSecode(QString secode) {
    QString windSecode;
    for (int j = 2; j < secode.size(); ++j) {
        windSecode.append(secode[j]);
    }
    windSecode.append('.');
    windSecode.append(secode[0]);
    windSecode.append(secode[1]);
    return windSecode;
}

QString getSimpleSecode(QString secode) {
//    qDebug() << "secode: " << secode;
    while(secode.startsWith("0")) {
        secode.remove(0,1);
    }
//    qDebug() << "secode: " << secode;
    return secode;
}

QString getCompleteIndexCode(QString secode, QString style) {
    while (secode.size() < 6) 
    {
        secode.prepend("0");
    }

    if (style == "tinysoft") 
    {
        if (secode.size() == 9)
        {
            if (secode.endsWith("SH"))
            {
                secode = secode.remove(6,3).prepend("SH");
            }

            if (secode.endsWith("SZ"))
            {
                secode = secode.remove(6,3).prepend("SZ");
            }
        }

        if (secode.size() == 6)
        {
            if (secode.startsWith("0")) 
            {
                secode.prepend("SH");
            } 
            else 
            {
                secode.prepend("SZ");
            }            
        }
    }

    if (style == "wind") 
    {
        if (secode.size() == 8)
        {
            if (secode.startsWith("SH"))
            {
                secode = secode.remove(0,2).append(".SH");
            }

            if (secode.startsWith("SZ"))
            {
                secode = secode.remove(0,2).append(".SZ");
            }
        }

        if (secode.size() == 6)
        {
            if (secode.startsWith("0")) 
            {
                secode.append(".SH");
            } 
            else 
            {
                secode.append(".SZ");
            }            
        }        
    }

    return secode;
}

QList<QString> getIndexCode(QString style) {
    QList<QString> indexCodeList;
    if (style == "wind") 
    {
        indexCodeList << "000300.SH" << "000016.SH" << "000852.SH"
                      << "000904.SH" << "000905.SH" << "000906.SH" << "399903.SZ"
                      << "000908.SH" << "000909.SH" << "000910.SH" << "000911.SH"
                      << "000912.SH" << "000913.SH" << "000951.SH" << "000849.SH"
                      << "000952.SH" << "000915.SH" << "000917.SH";
    }
    if (style == "tinysoft") {
        indexCodeList  << "SH000300" << "SH000016" << "SH000852"
                       << "SH000904" << "SH000905" << "SH000906" <<"SZ399903"
                       << "SH000908" << "SH000909" << "SH000910" << "SH000911"
                       << "SH000912" << "SH000913" << "SH000951" << "SH000849"
                       << "SH000952" << "SH000915" << "SH000917";
    }
    if (style == "ori") {
        indexCodeList << "000300" << "000016" << "000852"
                      << "000904" << "000905" << "000906" << "399903"
                      << "000908" << "000909" << "000910" << "000911"
                      << "000912" << "000913" << "000951" << "000849"
                      << "000952" << "000915" << "000917";
    }

    return indexCodeList;
}

bool isSecodeValid(QString secode) {
    bool result = true;
    if (secode == "") {
        result = false;
    }

    return result;
}

QString pureSecode(QString secode) {
    QString result = secode;
    if (secode.length() == 8) {
        result.remove(0,2);
    }

    if (secode.length() == 9) {
        result.remove(6,3);
    }
    return result;
}
