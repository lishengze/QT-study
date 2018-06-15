#include "time_func.h"
#include "toolfunc.h"
#include <QThread>
#include <QDebug>

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

bool isStockTrading() {
    if ((QTime::currentTime() >= StockAmStartTime() && QTime::currentTime() <= StockAmStopTime()) ||
         (QTime::currentTime() >= StockPmStartTime() && QTime::currentTime() <= StockPmStopTime())) {
        return true;
    } else {
        return false;
    }
}

bool isStockTradingNotStart() {
    return QTime::currentTime() < StockAmStartTime();
}

bool isStockNoonBreak() {
  return QTime::currentTime() > StockAmStopTime() && QTime::currentTime() <StockPmStartTime();
}

bool isStockTradingOver() {
    return QTime::currentTime() > StockPmStopTime();
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

        QDateTime startTime = QDateTime(QDate::currentDate(), QTime(8,0,0));
        sleepMsecs = startTime.toMSecsSinceEpoch() - newDayTime.toMSecsSinceEpoch();
        QThread::msleep(sleepMsecs);
    }
    return waitDays != -1;
}
