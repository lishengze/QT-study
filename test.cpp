#include "test.h"
#include <QDate>
#include <QDebug>

Test::Test()
{
    IsTradingDay();
}

void Test::IsTradingDay() {
    QDate testDate = QDate(2018, 3, 24);
    if (isTradingDay(testDate)) {
        qDebug() << testDate << " is trading day";
    } else {
        int dayOfWeek = testDate.dayOfWeek();
        int waitDays = -1;
        if (dayOfWeek > 5) {
            waitDays = 8 - dayOfWeek;
        } else {
            waitDays = 1;
        }
        qDebug() << testDate << " is not trading day, "
                 << "waitDays: " << waitDays;
    }

}

