#include "time_func.h"
//#include "toolfunc.h"
#include <QThread>
#include <QDebug>
#include <QtMath>
#include "widget_func.h"

QDateTime transIntDateTime(long long data) {
    // 20180414113400
    int year = data / 10000000000;
    data %= 10000000000;
    int month = data / 100000000;
    data %= 100000000;
    int day = data / 1000000;
    data %= 1000000;
    int hour = data / 10000;
    data %= 10000;
    int minut = data / 100;
    data %= 100;
    int second = data;
//    qDebug() << "year: " << year << ", month: " << month << ", day: " << day
//             << ",hour: " << hour << ", minut: " << minut << ", second: " << second;
    QDateTime result(QDate(year, month, day), QTime(hour, minut, second));
    return result;
}

QDateTime transQStringDateTime(QString data, QString separator) {
    QStringList dateTimeList = data.split(separator);
    int date = dateTimeList[0].toInt();
    int time = dateTimeList[1].toInt();
    return QDateTime(transIntDate(date), transIntTime(time));
}

QDate transIntDate(int data) {
    int year = data / 10000;
    data %= 10000;
    int month = data / 100;
    data %= 100;
    int day = data;
    return QDate(year, month, day);
}

QTime transIntTime(int data) {
    int hour = data / 10000;
    data %= 10000;
    int minut = data / 100;
    data %= 100;
    int second = data;
    return QTime(hour, minut, second);
}

qint64 transDateTime(double oridata) {
    long long  datetime = long long (oridata);
    int year = datetime / 10000000000;
    datetime = datetime % 10000000000;
    int month = datetime / 100000000;
    datetime = datetime % 100000000;
    int day = datetime / 1000000;
    datetime = datetime % 1000000;
    int hour = datetime / 10000;
    datetime = datetime % 10000;
    int miniute = datetime / 100;
    datetime = datetime % 100;
    int secs = datetime;
//    qDebug() << "oriData: " << oridata;
//    qDebug() << "year: " << year << ", month: " << month << ", day: " << day << ", "
//             << "hour: " << hour << ", miniute: " << miniute << ", secs: " << secs;
    QDate curDate = QDate(year, month, day);
    QTime curTime = QTime(hour, miniute, secs);
    QDateTime DateTime = QDateTime(curDate, curTime);
//    qDebug()<< "DateTime: " << DateTime;
    return DateTime.toMSecsSinceEpoch();
}

QString transOffsetSecondsToTime(qint64 offSecs) {
    QDateTime tmpDatetime;
    tmpDatetime = tmpDatetime.toOffsetFromUtc(offSecs);
    return tmpDatetime.toString ();
}

// 20170911 -> [2017, 9, 11]
QList<int> getDateList(int intDate) {
    QList<int> dateList;
    int year = intDate / 10000;
    int month = (intDate - year * 10000) / 100;
    int day = intDate - year * 10000 - month * 100;
    dateList.append (year);
    dateList.append (month);
    dateList.append (day);
    return dateList;
}

// 112002 -> [11, 20 ,2]
QList<int> getTimeList(int intDate) {
    QList<int> timeList;
    int hour = intDate / 10000;
    int minute = (intDate - hour * 10000) / 100;
    int second = intDate - hour * 10000 - minute * 100;
    timeList.append (hour);
    timeList.append (minute);
    timeList.append (second);
    return timeList;
}

QDate getDate(QString date) {
    int intDate = date.toLong();
    QList<int> dateInfoList = getDateList(intDate);
    int year = dateInfoList[0];
    int month = dateInfoList[1];
    int day = dateInfoList[2];
    QDate result(year, month, day);
    return result;
}

QTime StockAmStartTime() {
    return QTime(9, 30, 0);
}

QTime StockAmStopTime() {
    return QTime(11, 30, 20);
}

QTime StockPmStartTime() {
    return QTime(13, 0, 0);

}

QTime StockPmStopTime() {
    return QTime(15, 0, 20);
}

bool isStockTrading(QTime time) {
    if ((time >= StockAmStartTime() && time <= StockAmStopTime()) ||
         (time >= StockPmStartTime() && time <= StockPmStopTime())) {
        return true;
    } else {
        return false;
    }
}

bool isStockTradingNotStart(QTime time) {
    return time < StockAmStartTime();
}

bool isStockNoonBreak(QTime time) {
  return time > StockAmStopTime() && time <StockPmStartTime();
}

bool isStockTradingOver(QTime time) {
    return time > StockPmStopTime();
}

bool isTradingDay(QDate day) {
    QList<QDate> specialNoTradingDay;
    specialNoTradingDay << QDate(2018, 12, 30) << QDate(2018, 12, 31) << QDate(2018, 1,1) // 元旦
                        << QDate(2018, 2, 15)  << QDate(2018, 2, 16)  << QDate(2018, 2, 17) // 春节
                        << QDate(2018, 2, 18)  << QDate(2018, 2, 19)  << QDate(2018, 2, 20) << QDate(2018, 2, 21)
                        << QDate(2018, 4, 5)   << QDate(2018, 4, 6)   << QDate(2018, 4, 7)  // 清明节
                        << QDate(2018, 4, 29)  << QDate(2018, 4, 30)  << QDate(2018, 5, 1) // 劳动节
                        << QDate(2018, 6, 16)  << QDate(2018, 6, 17)  << QDate(2018, 6, 18) // 端午节
                        << QDate(2018, 9, 22)  << QDate(2018, 9, 23)  << QDate(2018, 9, 24) // 中秋节
                        << QDate(2018, 10, 1)  << QDate(2018, 10, 2)  << QDate(2018, 10, 3) // 国庆节
                        << QDate(2018, 10, 4)  << QDate(2018, 10, 5)  << QDate(2018, 10, 6)
                        << QDate(2018, 10, 7);

    if (day.dayOfWeek() > 5  || specialNoTradingDay.indexOf(day) >= 0) {
        return false;
    } else {
        return true;
    }
}

bool isTradingOver(QTime time) {
    QTime pmStopTime = QTime(15, 0, 20);
    if (time >= pmStopTime){
        return true;
    } else {
        return false;
    }
}

bool isTradingStart(QTime time) {
    QTime amStartTime = QTime(9,30,6);
    if (amStartTime > time) {
        return false;
    } else {
        return true;
    }
}

QDate getNextTradingData(QDate date) {
    QDate result = date.addDays(1);
    while(!isTradingDay(result)) {
        result = result.addDays(1);
    }
    return result;
}

bool waitForNextTradingDay(QTableView* programInfoTableView) {
    QDateTime curDateTime = QDateTime::currentDateTime();
    int dayOfWeek = QDate::currentDate().dayOfWeek();
    int waitDays = -1;
    if (!isTradingDay(curDateTime.date())) {
        updateProgramInfo(programInfoTableView, QString::fromLocal8Bit("----- 今天是非交易日, 等待新的交易日 -----"));
        if (dayOfWeek > 5) {
            waitDays = 8 - dayOfWeek;
        } else {
            waitDays = 1;
        }
    } else if (isStockTradingOver()) {
        updateProgramInfo(programInfoTableView, QString::fromLocal8Bit("交易结束, 等待下一个交易日"));
        if (dayOfWeek >= 5) {
            waitDays = 8 - dayOfWeek;
        } else {
            waitDays = 1;
        }
    }
    if (waitDays != -1) {
        QDateTime newDayTime = curDateTime.addDays(waitDays);
        newDayTime.setTime(QTime(0,10,0));
        int sleepMsecs = newDayTime.toMSecsSinceEpoch() - curDateTime.toMSecsSinceEpoch();
        qDebug() << "curDateTime: " << curDateTime <<"\n"
                 << "newDayTime: " << newDayTime << "\n"
                 << "sleepMsecs: " << sleepMsecs;
        QThread::msleep(sleepMsecs);
        updateProgramInfo(programInfoTableView, QString::fromLocal8Bit("----- 新的交易日 -----"));

        QDateTime startTime = QDateTime(QDate::currentDate(), QTime(9,0,0));
        sleepMsecs = startTime.toMSecsSinceEpoch() - newDayTime.toMSecsSinceEpoch();
        QThread::msleep(sleepMsecs);
    }
    return waitDays != -1;
}

bool isMinuteType(QString dataType) {
    if (dataType.indexOf("day") >= 0 || dataType.indexOf("month") >=0
        || dataType.indexOf("week") >= 0) {
        return false;
    } else {
        return true;
    }
}

QString getPreDate(QString oridata, QString timeType, int timeNumb) {
    QDate oriDate = transIntDate(oridata.toInt());
    int oneDayNumb = 1;
    int oneYearTradingDays = 220;
    if (timeType.indexOf("m") >=0 && timeType != "month") {
        QString time = timeType.remove('m');
        oneDayNumb = 4 * 60 / time.toInt();
    }
//    qDebug() << "onDayNumb: " << oneDayNumb;
    int preDays = qCeil(double(timeNumb) / double(oneYearTradingDays) / double(oneDayNumb) * 365);
    QDate result = oriDate.addDays(preDays*-1);
    return result.toString("yyyyMMdd");
}

int getStartIndex(QString startDate, QList<QString>& timeList) {
    int result = 0;
    for (int i = 0; i < timeList.size(); ++i) {
        QString currTime = timeList[i];
        if (timeList[i].indexOf(" ") >= 0) {
            currTime = timeList[i].split(" ")[0];
        }
        if (currTime.toInt() >= startDate.toInt()) {
            result = i;
            break;
        }
    }
    return result;
}

QList<QList<int>> getShortedStartEndDateList(int oriStartDate, int oriEndDate,
                                      QString databaseName, int dataNumbOnce) {
    QList<QList<int>> result;
    QString timeType = databaseName.split("_")[1];
    int oneDayNumb = 1;
    int oneYearTradingDays = 250;
    if (timeType.indexOf("m") >=0 && timeType != "month") {
        QString time = timeType.remove('m');
        oneDayNumb = 4 * 60 / time.toInt();
    }

    QDate startDate = transIntDate(oriStartDate);
    QDate endDate = transIntDate(oriEndDate);
    int addedDaysOnce = (dataNumbOnce/oneDayNumb) * (365/oneYearTradingDays);
//    int allDataNumb = startDate.daysTo(endDate) / 365 * 250 * oneDayNumb;
    while (startDate < endDate) {
        QDate newEndDate = startDate.addDays(addedDaysOnce);
        if (newEndDate > endDate) {
            newEndDate = endDate;
        }
        QList<int> currStartEndDate;
        currStartEndDate.append(startDate.toString("yyyyMMdd").toInt());
        currStartEndDate.append(newEndDate.toString("yyyyMMdd").toInt());
        result.append(currStartEndDate);
        startDate = newEndDate.addDays(1);
    }
    return result;
}
