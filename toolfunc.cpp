#include "toolfunc.h"
#include <QString>
#include <QMutexLocker>
#include <iostream>
#include <QThread>
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
int g_count = 0;

//功能：WSQ回调函数输出行情数值至控制台
LONG WINAPI CallBack( ULONGLONG reqid, const WindData &wd)
{
    cout <<"reqid: " << reqid << " ++g_count: " << ++g_count << endl;
    cout <<"reqid: " << reqid << " --g_count: " << --g_count << endl;
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
        cout << codes.toStdString() << "    ";
        for (int j = 0; j < fieldslen; ++j)
        {
            VARIANT var;
            wd.GetDataItem(0,i,j,var);
            QString temp = variantToQString(&var);
            singleCodeData.append(temp);
            cout << temp.toStdString() << "    ";
        }
        cout << endl;
        writeWsqData(codes, singleCodeData);
    }
    return 0;
}

void writeWsqData(QString secode, QStringList data) {
    QList<QString> keys = g_wsqData.keys();
    if (keys.indexOf(secode) < 0) {
        QList<QStringList> initData;
        initData.append(data);
        g_wsqData.insert(secode, initData);
    } else {
        QList<QStringList> currData = g_wsqData[secode];
        QStringList latestData = currData[currData.size()-1];
        if (latestData[1].toDouble() < data[1].toDouble()) {
            g_wsqData[secode].append(data);
        }
    }
//    qDebug() << g_wsqData;
}
