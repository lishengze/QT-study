#include <QDebug>
#include "test.h"
#include "datacontainer.h"

test::test()
{

}

void test::testMain() {
    testDataContainer ();
}

void test::testDataContainer() {
   int dataNumb = 10;
   QList<int> oriData;
   for (int i = 0; i < dataNumb; ++i) {
       oriData.append (i);
   }
   qDebug() << "oriData: " << oriData;
   DataContainer testObj(oriData);
}
