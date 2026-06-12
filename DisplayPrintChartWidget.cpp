#include "DisplayPrintChartWidget.h"
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QTime>
#include <QtCharts/QBarCategoryAxis>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPdfWriter>
#include <QPainter>
//#include <QPageSize>

DisplayPrintChartWidget::DisplayPrintChartWidget(QWidget *parent) : QWidget(parent),
    m_dataTable(),
    m_themeComboBox(createThemeBox()),
    m_someCheckBox(new QCheckBox("Some CheckBox", this)),
    m_legendComboBox(createLegendBox()),
    m_typeComboBox(createTypeBox()),
    m_chartView(new QtCharts::QChartView(this)),
    m_printButton(new QPushButton("Печать", this))
{
    connectSignals();
    // create layout
    QVBoxLayout *baseLayout = new QVBoxLayout(this);
    QHBoxLayout *settingsLayout = new QHBoxLayout(this);
    settingsLayout->addWidget(new QLabel("Theme:", this));
    settingsLayout->addWidget(m_themeComboBox);
    settingsLayout->addWidget(new QLabel("Type:", this));
    settingsLayout->addWidget(m_typeComboBox);
    settingsLayout->addWidget(new QLabel("Legend:", this));
    settingsLayout->addWidget(m_legendComboBox);
    settingsLayout->addWidget(m_someCheckBox);
    settingsLayout->addWidget(m_printButton);
    settingsLayout->addStretch();
    baseLayout->addLayout(settingsLayout);


    m_chartView->setChart(createBarChart());
    // Funny things happen if the pie slice labels do not fit the screen, so we ignore size policy
    m_chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    baseLayout->addWidget(m_chartView);

    setLayout(baseLayout);

    // Set defaults
    m_someCheckBox->setChecked(true);
    updateUI();
}

void DisplayPrintChartWidget::connectSignals()
{
    connect(m_themeComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &DisplayPrintChartWidget::updateUI);
    connect(m_someCheckBox, &QCheckBox::toggled, this, &DisplayPrintChartWidget::updateUI);
    connect(m_legendComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &DisplayPrintChartWidget::updateUI);

    connect(m_typeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &DisplayPrintChartWidget::updateChart);

    connect(m_printButton, &QPushButton::clicked, this, &DisplayPrintChartWidget::printChart);
}

DataTable DisplayPrintChartWidget::generateRandomData(int listCount, int valueMax, int valueCount)
{
    DataTable dataTable;

    // set seed for random stuff
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + (qreal)(qrand() % valueMax) / (qreal) valueCount;
            QPointF value((j + (qreal) rand() / (qreal) RAND_MAX) * ((qreal) valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

QComboBox *DisplayPrintChartWidget::createThemeBox()
{
    // settings layout
    QComboBox *themeComboBox = new QComboBox(this);
    themeComboBox->addItem("Light", QtCharts::QChart::ChartThemeLight);
    themeComboBox->addItem("Blue Cerulean", QtCharts::QChart::ChartThemeBlueCerulean);
    themeComboBox->addItem("Dark", QtCharts::QChart::ChartThemeDark);
    themeComboBox->addItem("Brown Sand", QtCharts::QChart::ChartThemeBrownSand);
    themeComboBox->addItem("Blue NCS", QtCharts::QChart::ChartThemeBlueNcs);
    themeComboBox->addItem("High Contrast", QtCharts::QChart::ChartThemeHighContrast);
    themeComboBox->addItem("Blue Icy", QtCharts::QChart::ChartThemeBlueIcy);
    themeComboBox->addItem("Qt", QtCharts::QChart::ChartThemeQt);
    return themeComboBox;
}

QComboBox *DisplayPrintChartWidget::createTypeBox()
{
    // settings layout
    QComboBox *typeComboBox = new QComboBox(this);
    typeComboBox->addItem("BarChart", "BarChart");
    typeComboBox->addItem("PieChart", "PieChart");
    return typeComboBox;
}

QComboBox *DisplayPrintChartWidget::createLegendBox()
{
    QComboBox *legendComboBox = new QComboBox(this);
    legendComboBox->addItem("No Legend ", 0);
    legendComboBox->addItem("Legend Top", Qt::AlignTop);
    legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    legendComboBox->addItem("Legend Left", Qt::AlignLeft);
    legendComboBox->addItem("Legend Right", Qt::AlignRight);
    return legendComboBox;
}

QtCharts::QChart *DisplayPrintChartWidget::createBarChart() const
{
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->setTitle("Bar chart");

    QtCharts::QStackedBarSeries *series = new QtCharts::QStackedBarSeries(chart);
    for (int i(0); i < m_dataTable.count(); i++) {
        QtCharts::QBarSet *set = new QtCharts::QBarSet("Bar set " + QString::number(i));
        for (const Data &data : m_dataTable[i])
            *set << data.first.y();
        series->append(set);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();

    return chart;
}

QtCharts::QChart *DisplayPrintChartWidget::createPieChart() const
{
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->setTitle("Pie chart");

    qreal pieSize = 1.0 / m_dataTable.count();
    for (int i = 0; i < m_dataTable.count(); i++) {
        QtCharts::QPieSeries *series = new QtCharts::QPieSeries(chart);
        for (const Data &data : m_dataTable[i]) {
            QtCharts::QPieSlice *slice = series->append(data.second, data.first.y());
            if (data == m_dataTable[i].first()) {
                slice->setLabelVisible();
                slice->setExploded();
            }
        }
        qreal hPos = (pieSize / 2) + (i / (qreal) m_dataTable.count());
        series->setPieSize(pieSize);
        series->setHorizontalPosition(hPos);
        series->setVerticalPosition(0.5);
        chart->addSeries(series);
    }

    return chart;
}

void DisplayPrintChartWidget::setData(DataTable dataTable)
{
    Q_UNUSED(dataTable);
    m_dataTable = generateRandomData(3, 10, 7);
    updateChart();

    //m_dataTable = dataTable;
}

void DisplayPrintChartWidget::updateUI()
{
    QtCharts::QChart::ChartTheme theme = static_cast<QtCharts::QChart::ChartTheme>(
        m_themeComboBox->itemData(m_themeComboBox->currentIndex()).toInt());
    m_chartView->chart()->setTheme(theme);

    // bool checked = m_someCheckBox->isChecked();
    // nothing


    Qt::Alignment alignment(m_legendComboBox->itemData(m_legendComboBox->currentIndex()).toInt());

    if (!alignment) {
        m_chartView->chart()->legend()->hide();
    } else {
        m_chartView->chart()->legend()->setAlignment(alignment);
        m_chartView->chart()->legend()->show();
    }
}

void DisplayPrintChartWidget::updateChart()
{
    QString newType = m_typeComboBox->itemData(m_typeComboBox->currentIndex()).toString();
    QtCharts::QChart* chart = m_chartView->chart();
    if(newType == "BarChart") {
        m_chartView->setChart(createBarChart());
    } else if (newType == "PieChart") {
        m_chartView->setChart(createPieChart());
    }
    delete chart;
    updateUI();
}

void DisplayPrintChartWidget::printChart()
{
    if (!m_chartView) {
        QMessageBox::warning(this, "Ошибка", "Указатель на график не задан.");
        return;
    }

    // 1. Вызываем стандартный диалог сохранения
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить диаграмму в PDF",
        QDir::homePath(),
        "PDF файлы (*.pdf);;Все файлы (*)"
        );

    if (filePath.isEmpty()) {
        return; // Пользователь нажал "Отмена"
    }

    // Гарантируем расширение .pdf
    if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
        filePath += ".pdf";
    }

    // 2. Настраиваем генератор PDF
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setResolution(300); // 300 DPI для четкого векторного/растрового вывода
    pdfWriter.setPageSize(QPagedPaintDevice::A4);

    // 3. Рендеринг в PDF
    QPainter painter(&pdfWriter);

    m_chartView->render(&painter);

    QMessageBox::information(this, "Успех", "Диаграмма успешно сохранена в PDF.");
    painter.end();
}
