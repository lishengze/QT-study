#include "strategy.h"
#include <QDebug>

strategy_ceil::strategy_ceil(QString secode, int buyCount):
    m_buyCount(buyCount){
    while (secode.size ()< 6) {
        secode.insert (0,'0');
    }
    if (secode.startsWith ("6")) {
        m_secode = "SH" + secode;
    } else {
        m_secode = "SZ" + secode;
    }
//    qDebug() << "m_secode: " << m_secode << " m_buyCount: " << m_buyCount;
}

//strategy::strategy()
//{

//}
