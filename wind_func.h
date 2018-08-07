#ifndef WIND_FUNC_H
#define WIND_FUNC_H

#include "WAPIWrapperCpp.h"
#include <QStringList>

LPCWSTR transSecode(QStringList secodeList);

LPCWSTR transSecodeList(QStringList secodeList);

LPCWSTR transSecode(QString qString);

QString variantToQString(const LPVARIANT data);

LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd);

QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList);


#endif // WIND_FUNC_H
