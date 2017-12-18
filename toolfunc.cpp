#include "toolfunc.h"
#include <QString>
#include <QMutexLocker>
#include <iostream>
using namespace std;

extern QMap<QString, QList<QStringList>> g_wsqData;
extern QMutex g_wsqMutex;

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
        str = QString("%1").arg(data->fltVal, 0, 'f', 4);
        break;
    case VT_R8:
        str = QString("%1").arg(data->dblVal, 0, 'f', 4);
        break;
    case VT_EMPTY:
        break;
    case VT_BSTR:
        str = QString((QChar*)data->bstrVal, wcslen(data->bstrVal));
    break;
    case VT_DATE:
//        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
        str = QString("%1").arg(data->date);
        break;
    }
    return str;
}

//功能：WSQ回调函数输出行情数值至控制台
LONG WINAPI CallBack( ULONGLONG reqid, const WindData &wd)
{
    QMutexLocker locker(&g_wsqMutex);
    QList<QString> curSecodeList = g_wsqData.keys();
    system("cls");
    cout << "reqid: " << reqid << endl;
    int codelen = wd.GetCodesLength();
    int fieldslen = wd.GetFieldsLength();
    int colnum = fieldslen + 1;
    cout  << "WindCodes    ";
    for (int i =1;i < colnum; ++i)
    {
        QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
        cout << outfields.toStdString() << "    ";
    }
    cout << endl;
    for (int i = 0; i < codelen; ++i)
    {
        QStringList singleCodeData;
        QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
        if (curSecodeList.indexOf(codes) < 0) {
            QList<QStringList> emtpy;
            g_wsqData.insert(codes, emtpy);
        }
        cout << codes.toStdString() << "    ";
        singleCodeData.append(codes);
        for (int j = 0; j < fieldslen; ++j)
        {
            VARIANT var;
            wd.GetDataItem(0,i,j,var);
            QString temp = variantToQString(&var);
            singleCodeData.append(temp);
            cout << temp.toStdString() << "    ";
        }
        g_wsqData[codes].append(singleCodeData);
        cout << endl;
    }
    qDebug() << "g_wsqData: " << g_wsqData;
    return 0;
}

void writeWsqData(int reqId, QList<QStringList> result) {
//     QMutexLocker locker(&g_wsqMutex);
//     QList<int> keys = g_wsqData.keys();
//     if (keys.indexOf(reqId) < 0) {
//         QList<QList<QStringList>> initData;
//         initData.append(result);
//         g_wsqData.insert(reqId, initData);
//     } else {
//         g_wsqData[reqId].append(result);
//     }
//     qDebug() << g_wsqData;
}
