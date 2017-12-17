#include "test.h"
#include <QDebug>
#include <iostream>
using namespace std;

#include "WAPIWrapperCpp.h"
Test::Test(QObject* parent):QObject(parent)
{

}

void Test::login() {
    qDebug() << "Logining...... ";
    int errcode = CWAPIWrapperCpp::start();
    qDebug() << "Login errcode: " << errcode;
    if (0 == errcode) {
        cout << "login successfully" << endl;
        testWsd();
    }else {
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        cout << buffer << endl;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
    }
}

int Test::testWset(){
    qDebug() << "Get all A stock id....";
    WindData wd;
    LPCWSTR reportname = TEXT("sectorconstituent");
    LPCWSTR options = TEXT("date=2017-11-07;sectorid=a001010100000000");
    int errcode = CWAPIWrapperCpp::wset(wd,reportname,options);
    qDebug() << "wset errcode: " << errcode;
    if (0 == errcode)
    {
        qDebug() << "get data successfuly!";
        int codelen = wd.GetCodesLength();
        int fieldslen = wd.GetFieldsLength();
        int colnum = fieldslen + 1;
        cout << "colnum: " << colnum << ", codeLen: " << codelen << endl;
        for (int i =1;i < colnum; ++i)
        {
            LPCWSTR outfields = wd.GetFieldsByIndex(i-1);
            qDebug() << QString::fromStdWString(outfields);
        }
        std::wcout <<std::endl;
        for (int i = 0; i < codelen; ++i)
        {
            QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
            qDebug() << codes;
            for (int j = 0; j < fieldslen; ++j)
            {
                VARIANT var;
//                wd.GetDataItem(0,i,j,var);
//                QString temp = QString::fromStdWString(toString(&var));
//                qDebug() << var;
            }
        }
        return 0;

    }
    else
    {
        qDebug() << "Failed in getting stock datae";
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        qDebug() << buffer;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
        return 0;
    }
}

int Test::testWsd()
{
    WindData wd;
    //WindCode为平安银行
    LPCWSTR windcodes = TEXT("000001.SZ");
    //指标为基本行情
    LPCWSTR indicators = TEXT("high,low,open,close");
    //开始日期前推一年
    LPCWSTR begtime = TEXT("-1Y");
    //结束日期默认当前
    LPCWSTR endtime = TEXT("");
    //保存错误代码
    int errcode = CWAPIWrapperCpp::wsd(wd,windcodes,indicators,begtime,endtime);
    if (0 == errcode)
    {
        int timelen = wd.GetTimesLength();
        int fieldslen = wd.GetFieldsLength();
        int colnum = fieldslen + 1;
        for (int i =1;i < colnum; ++i)
        {
            LPCWSTR outfields = wd.GetFieldsByIndex(i-1);
            std::wcout << outfields;
            std::wcout << "     ";
        }
        std::wcout <<std::endl;

        qDebug() << "timelen: " << timelen;
//        CString dateFormat = L"%Y-%m-%d";
//        for (int i = 0; i < timelen; ++i)
//        {
//            DATE date = wd.GetTimeByIndex(i);
//            WCHAR* time = WindData::DateToString(date,dateFormat);
//            std::wcout << time;
//            WindData::FreeString(time);
//            std::wcout << "  ";
//            for (int j = 0; j < fieldslen; ++j)
//            {
//                VARIANT var;
//                wd.GetDataItem(i,0,j,var);
//                CString temp = toString(&var);
//                std::wcout.imbue(std::locale("chs"));
//                std::wcout << temp.GetString();
//                std::wcout << "  ";

//            }
//            std::wcout << std::endl;
//        }
        return 0;
    }
    else
    {
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
        return 0;
    }
}
