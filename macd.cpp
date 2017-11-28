#include "macd.h"

MACD::MACD()
{

}

MACD::MACD(double ema1, double ema2, double diff, double dea, double macd):
    m_ema1(ema1), m_ema2(ema2), m_diff(diff), m_dea(dea), m_macd(macd)
{

}

ostream& operator << (ostream& output, const MACD& value) {
    output << value.m_ema1 << ", " << value.m_ema2 << ", "
           << value.m_diff << ", " << value.m_dea << ", " << value.m_macd << endl;
    return output;
}
