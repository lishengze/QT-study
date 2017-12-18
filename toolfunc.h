#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QDebug>

#include "WAPIWrapperCpp.h"

class ToolFunc
{
public:
    ToolFunc();
};

QString variantToQString(const LPVARIANT data);

LONG WINAPI CallBack( ULONGLONG reqid,const WindData &wd);

void writeWsqData(int reqId, QList<QStringList> result);

#endif // TOOLFUNC_H
