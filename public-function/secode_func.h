﻿#ifndef SECODE_FUNC_H
#define SECODE_FUNC_H
#include <QString>

bool isCodeInDatabase(QString codeName, QString databaseName, QString dbHost);

QString completeSecode(QString secode, QString style="tinysoft");

QString getCompleteSecode(QString secode, QString style="tinysoft", QString letterCase = "capital");

QList<QString> getIndexCode(QString style);

QString getWindSecode(QString secode);

QString getSimpleSecode(QString secode);

QString getCompleteIndexCode(QString secode, QString style="tinysoft");

bool isSecodeValid(QString secode);

QString pureSecode(QString secode);

#endif // SECODE_FUNC_H