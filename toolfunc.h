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

#endif // TOOLFUNC_H
