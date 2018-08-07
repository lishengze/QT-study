#include "id_func.h"
#include <QList>
#include <QMutex>

QMutex g_databaseConnectIdMutex;
QList<int> g_databaseIdList;
int g_databaseIdNumb = 0;

void initDatabaeConnectId(int idNumb) {
    g_databaseConnectIdMutex.lock();

    for (int i = 0; i < idNumb; ++i) {
        g_databaseIdList.append(i);
    }
    g_databaseIdNumb = g_databaseIdList.size();
    g_databaseConnectIdMutex.unlock();
}

int getDatabaseConnectId() {
    g_databaseConnectIdMutex.lock();
    if (g_databaseIdList.empty()) {
        int addedNumb = 1000;
        for (int i = 0; i < addedNumb; ++i) {
            g_databaseIdList.append(i + g_databaseIdNumb);
        }
        g_databaseIdNumb += addedNumb;
    }

    int currId = g_databaseIdList.last();
    g_databaseIdList.pop_back();
    g_databaseConnectIdMutex.unlock();
    return currId;
}

void releaseDatabaseConnectId(int id) {
    g_databaseConnectIdMutex.lock();

    g_databaseIdList.append(id);

    g_databaseConnectIdMutex.unlock();
}
