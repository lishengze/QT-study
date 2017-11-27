#include "tabledata.h"

TableData::TableData()
{

}

TableData::TableData(int date, int time, float value):
    m_date(date),
    m_time(time),
    m_value(value)
{

}

ostream& operator << (ostream& output, const TableData& data) {
    output << data.m_date << ", " << data.m_time << ", " << data.m_value << endl;
    return output;
}
