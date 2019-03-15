#include "datastruct.h"
#include "process_data_func.h"
#include "secode_func.h"

CSSParamAtom::CSSParamAtom()
{
    m_aveNumb = 1;
    m_css12Rate = 0.014;
    m_cssRate1 = 1.0 / 3.0;
    m_cssRate2 = 2.0 / 3.0;
    m_maxCSS = 300;
    m_minCSS = -300;    
    m_isPotentialEnergy = false;
}

CSSParamAtom::CSSParamAtom(int aveNumb, int css12Rate, double cssRate1, double cssRate2,
                double maxCSS, double minCSS, bool isPotentialEnergy):
    m_aveNumb(aveNumb), m_css12Rate(css12Rate), m_cssRate1(cssRate1),
    m_cssRate2(cssRate2), m_maxCSS(maxCSS), 
    m_minCSS(minCSS), m_isPotentialEnergy(isPotentialEnergy)
{
}

CSSParam::CSSParam()
{
    m_mainAveNumb = 1;
    m_subAveNumb = 1;
    m_energyAveNumb = 1;
    m_css12Rate = 0.014;
    m_mainCssRate1 = 1.0 / 3.0;
    m_mainCssRate2 = 2.0 / 3.0;
    m_energyCssRate1 = 11.0 / 13.0;
    m_energyCssRate2 = 2.0 / 13.0;
    m_maxCSS = 300;
    m_minCSS = -300;

    setAtomCSSParam();
}

CSSParam::CSSParam(int mainAveNumb, int subAveNumb, int energyAveNumb,
        double css12Rate, double mainCssRate1, double mainCssRate2,
        double energyCssRate1, double energyCssRate2, double maxCSS,
        double minCSS, QList<int> aveNumbList, QList<bool> isEMAList):
        m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
        m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
        m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2), 
        m_maxCSS(maxCSS), m_minCSS(minCSS), m_aveNumbList(aveNumbList), m_isEMAList(isEMAList)
{
    setAtomCSSParam();
}

void CSSParam::setAtomCSSParam()
{
    m_mainCSSParamAtom = CSSParamAtom(m_mainAveNumb, m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                      m_maxCSS, m_minCSS, false);

    m_subCSSParamAtom = CSSParamAtom(m_subAveNumb, m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                    m_maxCSS, m_minCSS, false);

    m_energyCSSParamAtom = CSSParamAtom(m_energyAveNumb, m_css12Rate, m_energyCssRate1, m_energyCssRate2,
                                        m_maxCSS, m_minCSS, true);
}

HedgedParam::HedgedParam()
{
    m_startDate = "";
    m_endDate = "";
    m_weightDate = "";

    m_oriCode = "";
    m_hedgedCode = "";
    m_portfolioName = "";
    m_oriPortfolio.clear();
    m_hedgedPortfolio.clear();

    m_hedgedType = -1;
    m_macdTime.clear();

    m_isRealTime = false;
    m_isPortfolio = false;
    m_isPortfolioHedge = false;
    m_isCSSChart = false;
    m_isIndex = false;

    m_energyUpdateMSecs = 10 * 1000;
    m_spreadUpdateMSecs = 4 * 1000;
}

void HedgedParam::setCodeList()
{

    if (m_isRealTime && m_isCSSChart == false)
    {
        m_oriPortfolio = transStrategyMap(m_oriPortfolio, "wind");
        m_hedgedPortfolio = transStrategyMap(m_hedgedPortfolio, "wind");
        m_hedgedCode = getCompleteIndexCode(m_hedgedCode, "wind");
    }
    else
    {
        m_oriPortfolio = transStrategyMap(m_oriPortfolio, "tinysoft");
        m_hedgedPortfolio = transStrategyMap(m_hedgedPortfolio, "tinysoft");
        m_hedgedCode = getCompleteIndexCode(m_hedgedCode, "tinysoft");
    }
    
    if (m_hedgedCode.size() > 0 && m_hedgedType != 0)
    {
        m_oriPortfolio.insert(m_hedgedCode, m_hedgedType);
    }

    for(QMap<QString, double>::const_iterator it = m_oriPortfolio.begin();
        it != m_oriPortfolio.end(); ++it)
    {
        if (m_codeList.indexOf(it.key()) < 0)
        {
            m_codeList.append(it.key());
        }
    }

    for(QMap<QString, double>::const_iterator it = m_hedgedPortfolio.begin();
        it != m_hedgedPortfolio.end(); ++it)
    {
        if (m_codeList.indexOf(it.key()) < 0)
        {
            m_codeList.append(it.key());
        }
    }    
}

DatabaseParam::DatabaseParam():
    m_dbhost("127.0.0.1"),
    m_databaseName("master")
{

}

DatabaseParam::DatabaseParam(QString dbhost, QString databaseName):
    m_dbhost(dbhost), m_databaseName(databaseName)
{

}
