#ifndef CHARTCREATOR_H
#define CHARTCREATOR_H

#include <QChart>
#include <QString>

using Data = QPair<QString, double>;
using DataTable = QList<Data>;

QT_CHARTS_USE_NAMESPACE
class ChartCreator
{
public:
    virtual ~ChartCreator() = default;
    QChart* createChart(DataTable dataTable);
protected:
    virtual bool setChartData(QChart* chart, DataTable dataTable) = 0;
};

class BarChartCreator : public ChartCreator
{
public:
    BarChartCreator() = default;
    ~BarChartCreator() = default;
protected:
    bool setChartData(QChart* chart, DataTable dataTable);
};

class PieChartCreator : public ChartCreator
{
public:
    PieChartCreator() = default;
    ~PieChartCreator() = default;
protected:
    bool setChartData(QChart* chart, DataTable dataTable);
};

#endif // CHARTCREATOR_H
