﻿#ifndef TEST_H
#define TEST_H
#include "toolfunc.h"

class Test
{
public:
    Test();

     void IsTradingDay();

     void get_future_data();

     void get_database_data();

     void test_extract_marketdata();

     void test_getWeightData();

     void test_getShortedStartEndDateList();

     void test_getDateByDate();
};

#endif // TEST_H
