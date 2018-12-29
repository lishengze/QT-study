#ifndef TABLEDATA_H
#define TABLEDATA_H
#include <iostream>

using namespace std;

class TableData
{
public:
    TableData();
    TableData(int date, int time, float value);
    int m_date;
    int m_time;
    float m_value;

    friend ostream& operator << (ostream& os, const TableData& data);
};

#endif // TABLEDATA_H
