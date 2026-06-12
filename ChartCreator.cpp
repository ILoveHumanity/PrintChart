#include "ChartCreator.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>

#include <QtCharts/QStackedBarSeries>
#include <QHorizontalStackedBarSeries>

#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

#include <QString>

QChart* ChartCreator::createChart(DataTable dataTable){
    QtCharts::QChart *chart = new QtCharts::QChart();
    if(chart) {
        setChartData(chart, dataTable);
    }
    return chart;
}


bool BarChartCreator::setChartData(QChart* chart, DataTable dataTable){
    chart->setTitle("Bar chart");
    QBarSeries *series = new QBarSeries(chart);

    for (const Data &data : dataTable) {
        QBarSet *set = new QBarSet(data.first);
        set->append(data.second);
        series->append(set);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
    return true;
}


bool PieChartCreator::setChartData(QChart* chart, DataTable dataTable){
    chart->setTitle("Pie chart");

    QPieSeries *series = new QPieSeries();

    for (const Data &data : dataTable) {
        series->append(data.first, data.second);
    }

    chart->addSeries(series);
    return true;
}
