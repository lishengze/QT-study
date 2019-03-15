#include "widgetprocess.h"
#include <QDebug>

WidgetProcess::WidgetProcess():
    m_isWidgetProcessing(false)
{

}

WidgetProcess& WidgetProcess::Instance()
{
    static WidgetProcess widgetProcessObj;
    return widgetProcessObj;    
}

bool WidgetProcess::isProcessing()
{
    return m_isWidgetProcessing;
}

void WidgetProcess::setWidgetProcess(bool value)
{
    m_mutex.lock();

    m_isWidgetProcessing = value;

    m_mutex.unlock();
}

bool WidgetProcess::checkProcessState()
{
    m_mutex.lock();
    bool result = m_isWidgetProcessing;
    if (m_isWidgetProcessing)
    {
        m_isWidgetProcessing = false;
    } 
    m_mutex.unlock();
    return result;
}

void WidgetProcess::lock(QString chartName)
{
    m_mutex.lock();
    // qDebug() << chartName << " locked";
}

void WidgetProcess::unlock(QString chartName)
{
    // qDebug() << chartName << " unlocked";    
    m_mutex.unlock();
}
