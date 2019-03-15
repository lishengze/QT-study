#ifndef _WIDGET_PROCESS_H_
#define _WIDGET_PROCESS_H_

#include <mutex>
#include <QString>

class WidgetProcess
{
public:
    WidgetProcess();

    bool isProcessing();
    void setWidgetProcess(bool);

    bool checkProcessState();

    void lock(QString chartName="");
    
    void unlock(QString chartName="");

    static WidgetProcess& Instance();

private:
    std::mutex                 m_mutex;
    bool                       m_isWidgetProcessing;
};

#endif // !_WIDGET_PROCESS_H_
