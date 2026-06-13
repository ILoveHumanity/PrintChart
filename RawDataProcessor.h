#ifndef RAWDATAPROCESSOR_H
#define RAWDATAPROCESSOR_H

#include "ChartCreator.h"

class RawDataProcessor
{
public:
    virtual ~RawDataProcessor() = default;
    virtual DataTable getData(QString filePath, QString& Error) = 0;
};

class SQLiteRawDataProcessor : public RawDataProcessor
{
public:
    SQLiteRawDataProcessor() = default;
    ~SQLiteRawDataProcessor() = default;
    DataTable getData(QString filePath, QString& Error);
};

class JsonRawDataProcessor : public RawDataProcessor
{
public:
    JsonRawDataProcessor() = default;
    ~JsonRawDataProcessor() = default;
    DataTable getData(QString filePath, QString& Error);
};

#endif // RAWDATAPROCESSOR_H
