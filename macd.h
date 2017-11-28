#ifndef MACD_H
#define MACD_H
#include <iostream>

using namespace std;

class MACD
{
public:
    MACD();
    MACD(double ema1, double ema2, double diff, double dea, double macd);
    double m_diff;
    double m_dea;
    double m_macd;
    double m_ema1;
    double m_ema2;
    friend ostream& operator << (ostream& output, const MACD& value);
};

#endif // MACD_H
