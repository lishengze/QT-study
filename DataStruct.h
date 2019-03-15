#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QList>
#include <QMap>


class CSSParamAtom
{
    public:
        CSSParamAtom();
        CSSParamAtom(int aveNumb, int css12Rate, double cssRate1, double cssRate2,
                     double maxCSS, double minCSS, bool isPotentialEnergy);
        int                               m_aveNumb;
        int                               m_css12Rate;
        double                            m_cssRate1;
        double                            m_cssRate2;
        double                            m_maxCSS;
        double                            m_minCSS;
        bool                              m_isPotentialEnergy;
};

class CSSParam
{
    public:
        CSSParam();
        CSSParam(int mainAveNumb, int subAveNumb, int energyAveNumb,
                 double css12Rate, double mainCssRate1, double mainCssRate2,
                 double energyCssRate1, double energyCssRate2, double maxCSS,
                 double minCSS, QList<int> aveNumbList, QList<bool> isEMAList);

        int                                m_mainAveNumb;
        int                                m_subAveNumb;
        int                                m_energyAveNumb;
        double                             m_css12Rate;
        double                             m_mainCssRate1;
        double                             m_mainCssRate2;
        double                             m_energyCssRate1;
        double                             m_energyCssRate2;
        double                             m_maxCSS;
        double                             m_minCSS;

        QList<int>                         m_aveNumbList;
        QList<bool>                        m_isEMAList;        

        CSSParamAtom                       m_mainCSSParamAtom;
        CSSParamAtom                       m_subCSSParamAtom;
        CSSParamAtom                       m_energyCSSParamAtom;

        void setAtomCSSParam();
};

class HedgedParam
{
    public:
        HedgedParam();
        QString                     m_startDate;
        QString                     m_endDate;
        QString                     m_weightDate;
        QString                     m_oldStartDate;

        QString                     m_oriCode;
        QString                     m_hedgedCode;

        QMap<QString, double>       m_oriPortfolio;
        QMap<QString, double>       m_hedgedPortfolio;

        QString                     m_portfolioName;
        int                         m_hedgedType;       

        QList<int>                  m_macdTime; 
        QList<QString>              m_codeList;

        QString                     m_timeType;
        QStringList                 m_timeTypeList;

        int                         m_energyUpdateMSecs;
        int                         m_spreadUpdateMSecs;

        bool                        m_isRealTime;
        bool                        m_isPortfolio;
        bool                        m_isPortfolioHedge;
        bool                        m_isCSSChart;        
        bool                        m_isIndex;

        void setCodeList();
};

class DatabaseParam
{
    public:
        DatabaseParam();
        DatabaseParam(QString dbhost, QString databaseName="master");        
        QString                     m_dbhost;
        QString                     m_databaseName;
};

#endif // DATASTRUCT_H