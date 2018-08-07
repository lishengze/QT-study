#include "wind_func.h"
#include <QStringList>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <string>
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::wcout;

LPCWSTR transSecode(QStringList secodeList) {
    QString result="";
    for (int i = 0; i < secodeList.size(); ++i) {
        QString curSecode = secodeList[i];
        QString transSecode;
        for (int j = 2; j < curSecode.size(); ++j) {
            transSecode.append(curSecode[j]);
        }
        transSecode.append('.');
        transSecode.append(curSecode[0]);
        transSecode.append(curSecode[1]);
        result += transSecode + ',';
    }
    result.remove(result.size()-1, result.size()-1);

    string data = result.toStdString();
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
    return buffer;
}

LPCWSTR transSecodeList(QStringList secodeList) {
    QString result="";
    for (int i = 0; i < secodeList.size(); ++i) {
        result += secodeList[i] + ',';
    }
    result.remove(result.size()-1, result.size()-1);

    string data = result.toStdString();
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
    return buffer;
}

LPCWSTR transSecode(QString qString) {
    string data = qString.toStdString();
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
    return buffer;
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
        str = QString((QChar*)data->bstrVal, (int)(wcslen(data->bstrVal)));
    break;
    case VT_DATE:
//        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
        str = QString("%1").arg(data->date);
        break;
    }
    return str;
}

//功能：WSQ回调函数输出行情数值至控制台
int g_count = 0;
LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd)
{
    qDebug() << "Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
             << ", g_count: " << ++g_count << ", reqid: " << reqid;
    reqid;
    int codelen = wd.GetCodesLength();
    int fieldslen = wd.GetFieldsLength();
    int colnum = fieldslen + 1;
    QMap<QString, QStringList> tmpResult;
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
//        cout << codes.toStdString() << "    ";
        for (int j = 0; j < fieldslen; ++j)
        {
            VARIANT var;
            wd.GetDataItem(0,i,j,var);
            QString temp = variantToQString(&var);
            singleCodeData.append(temp);
            cout << temp.toStdString() << "    ";
        }
        cout << endl;
//        writeWsqData(codes, singleCodeData);
        tmpResult.insert(codes, singleCodeData);
    }
//    testSpread(tmpResult);
    return 0;
}

QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList) {
    bool bOutputMsg = true;
//    qDebug() << "toolFunc::wsqSnaphootData";
//    qDebug() << "Thread: " << QThread::currentThreadId();
    QMap<QString, QStringList> result;
    QTime curTime = QTime::currentTime();
    QTime amStopTime = QTime(11, 30, 10);
    QTime pmStartTime = QTime(13, 0, 0);
    if (curTime >= amStopTime && pmStartTime >= curTime) return result;
    int errcode;
    if (true) {
        WindData wd;
        LPCWSTR windcodes = transSecodeList(secodeList);
//        LPCWSTR windcodes = TEXT("000001.SZ");
        if (bOutputMsg) wcout << "windcodes: " << windcodes << endl;
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt");
        LPCWSTR options = TEXT("");
        errcode = CWAPIWrapperCpp::wsq(wd, windcodes, indicators, options);
        if (bOutputMsg) qDebug() << "startWsqOneTime errcode: " << errcode;
        if (errcode == 0) {
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            int colnum = fieldslen + 1;
            QString datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            if (bOutputMsg)  cout  << "WindCodes    ";
            for (int i =1;i < colnum; ++i)
            {
                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
                if (bOutputMsg) cout << outfields.toStdString() << "    ";
            }
            if (bOutputMsg) cout << endl;
            for (int i = 0; i < codelen; ++i)
            {
                QStringList singleCodeData;
                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
                if (bOutputMsg)  cout << codes.toStdString() << "    ";
                for (int j = 0; j < fieldslen; ++j)
                {
                    VARIANT var;
                    wd.GetDataItem(0,i,j,var);
                    QString temp = variantToQString(&var);
                    singleCodeData.append(temp);
                    if (bOutputMsg) cout << temp.toStdString() << "    ";
                }
                singleCodeData.append(datetime);
                if (bOutputMsg) cout << endl;
                result.insert(codes, singleCodeData);
            }
        } else {
//            emit startWsqFailed(errcode, reqID);
        }
        delete[] windcodes;
    } else {
        errcode = -1;
        qDebug() << "Login first!";
    }
//    qDebug() << "result: " << result;
    return result;
}
