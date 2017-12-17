#include "toolfunc.h"
//#include <afxdisp.h>

ToolFunc::ToolFunc()
{

}

QString variantToQString(const LPVARIANT data)
{
    QString str;
    switch (data->vt)
    {
    case VT_INT:
        str = QString::number(data->intVal);
        break;
    case VT_I4:
        str = QString::number(data->lVal);
        break;
    case VT_I8:
        str = QString::number(data->llVal);
        break;
    case VT_R4:
        str = QString("%1").arg(data->fltVal, 0, 'f', 2);
        break;
    case VT_R8:
        str = QString("%1").arg(data->dblVal, 0, 'f', 2);
        break;
    case VT_EMPTY:
        break;
    case VT_BSTR:
        str = QString((QChar*)data->bstrVal, wcslen(data->bstrVal));
    break;
    case VT_DATE:
//        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
        break;
    }
    return str;
}
