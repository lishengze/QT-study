#include "realtimedataread.h"
#include <QDebug>
#include <iostream>
#include <QDate>
#include <QThread>
#include "toolfunc.h"
#include "WAPIWrapperCpp.h"
using namespace std;

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView, QObject* parent):
    m_programInfoTableView(programInfoTableView), m_login(false),
    QObject(parent)
{

}

void RealTimeDataRead::loginWind() {
//    qDebug() << "Test: Logining...... ";
    int errcode = CWAPIWrapperCpp::start();
//    qDebug() << "Login errcode: " << errcode;
    if (0 == errcode) {
        cout << "login successfully" << endl;
        emit loginWindSucc();
        m_login = true;
    }else {
        emit loginWindFailed(errcode);
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        cout << buffer << endl;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
    }
}

void RealTimeDataRead::getPreData(QList<QString> secodeList) {
    int errcode;
//    qDebug() << "RealTimeDataRead::getOldStrategySpread ";
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecode(secodeList);
        LPCWSTR indicators = TEXT("pre_close");
        LPCWSTR options = TEXT("Days=Alldays");
        LPCWSTR beginDate = transSecode(QDate::currentDate().toString("yyyy-MM-dd"));

        errcode = CWAPIWrapperCpp::wsd(wd, windcodes, indicators, beginDate, beginDate, options);
        qDebug() << "wsd errcode: " << errcode;
        if (errcode == 0) {
            QMap<QString, QStringList> result;
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            int colnum = fieldslen + 1;
//            cout  << "WindCodes    ";
            for (int i =1;i < colnum; ++i)
            {
                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
//                cout << outfields.toStdString() << "    ";
            }
//            cout << endl;
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
        //            cout << temp.toStdString() << "    ";
                }
                result.insert(codes, singleCodeData);
            }
            emit sendPreData(result);
        } else {

        }
        delete[] beginDate;
    } else {
        errcode = -1;
    }

}

void RealTimeDataRead::startWsq(QStringList secodeList, int reqID) {
//    qDebug() << "Thread: " << QThread::currentThreadId() <<  " wsq...... ";
    int errcode;
    if (m_login) {
        updateProgramInfo(m_programInfoTableView, "订阅实时数据");
//        qDebug() <<"secodeList: " << secodeList;
//        LPCWSTR windcodes = TEXT("600000.SH,000001.SZ,000002.SZ");
        LPCWSTR windcodes = transSecode(secodeList);
//        wcout << "wcout windcodes: " << windcodes << endl;
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_last,rt_pre_close,rt_amt");
        LPCWSTR options = TEXT("");
//        ULONGLONG reqid = 0;
        ULONGLONG reqid = unsigned long long (reqID);
        errcode = CWAPIWrapperCpp::wsq(reqid, windcodes, indicators, wsqCallBack, options, TRUE);
        qDebug() << "wsq errcode: " << errcode;
        if (errcode == 0) {
            emit startWsqSucc();
        } else {
            emit startWsqFailed(errcode, reqID);
        }
        delete[] windcodes;
    } else {
        errcode = -1;
        qDebug() << "Login first!";
    }
}

void RealTimeDataRead::cancelWsqRequest(int reqID) {
    ULONGLONG reqid = unsigned long long (reqID);
    CWAPIWrapperCpp::cancelRequest(reqid);
    qDebug() << "cancelWsqRequest ";
}

void RealTimeDataRead::cancelAllWsqRequest() {
    CWAPIWrapperCpp::cancelAllRequest();
    qDebug() << "cancelAllWsqRequest ";
}
