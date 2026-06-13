#ifndef DISPLAYPRINTCHARTWIDGET_H
#define DISPLAYPRINTCHARTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QChartView>
#include <QChart>
#include "ChartCreator.h"

class DisplayPrintChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayPrintChartWidget(QWidget *parent = nullptr);
    ~DisplayPrintChartWidget() = default;
    void setData(DataTable);
    bool isOk();

private slots:
    void updateChart(); // Слот для обновления изменения типа диаграммы
    void updateUI();
    void printChart();

private:
    QComboBox *createThemeBox();
    QComboBox *createLegendBox();
    QComboBox *createTypeBox();

    void connectSignals();

private:
    bool state;
    DataTable m_dataTable;
    QComboBox *m_themeComboBox;
    QComboBox *m_legendComboBox;
    QComboBox *m_typeComboBox; // Указатель на комбобокс выбора типа диаграммы
    QtCharts::QChartView *m_chartView; // Указатель на виджет отображения диаграммы
    QPushButton *m_printButton;
};

#endif // DISPLAYPRINTCHARTWIDGET_H
