#ifndef DISPLAYPRINTCHARTWIDGET_H
#define DISPLAYPRINTCHARTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QChartView>
#include <QChart>

using Data = QPair<QPointF, QString>;
using DataList = QList<Data>;
using DataTable = QList<DataList>;

class DisplayPrintChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayPrintChartWidget(QWidget *parent = nullptr);
    ~DisplayPrintChartWidget() = default;
    QtCharts::QChart *createBarChart() const; // Метод для создания столбчатой диаграммы
    QtCharts::QChart *createPieChart() const; // Метод для создания круговой диаграммы
    void setData(DataTable);

private slots:
    void updateChart(); // Слот для обновления изменения типа диаграммы
    void updateUI();
    void printChart();

private:
    DataTable generateRandomData(int listCount, int valueMax, int valueCount);
    QComboBox *createThemeBox();
    QComboBox *createLegendBox();
    QComboBox *createTypeBox();

    void connectSignals();

private:
    DataTable m_dataTable;
    QComboBox *m_themeComboBox;
    QCheckBox *m_someCheckBox;
    QComboBox *m_legendComboBox;
    QComboBox *m_typeComboBox; // Указатель на комбобокс выбора типа диаграммы
    QtCharts::QChartView *m_chartView; // Указатель на виджет отображения диаграммы
    QPushButton *m_printButton;
};

#endif // DISPLAYPRINTCHARTWIDGET_H
