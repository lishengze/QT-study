#include "counter.h"
#include <iostream>
#include <string>
using namespace std;

Counter::Counter(string objName) {
    m_value = 0;
    m_objName = objName;
}

int Counter::value () {
    return m_value;
}

void Counter::setValue (int value) {
    cout << "ObjName: " << m_objName << endl;
//    if (m_value != value) {
//        m_value = value;
//        emit valueChanged (value);
//    }
    m_value = value;
    emit valueChanged (value);
}

Counter::~Counter (){

}
