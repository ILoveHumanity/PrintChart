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
#include "ChartCreator.h"

DisplayPrintChartWidget::DisplayPrintChartWidget(QWidget *parent) : QWidget(parent),
    m_dataTable(),
    m_themeComboBox(createThemeBox()),
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
    settingsLayout->addWidget(m_printButton);
    settingsLayout->addStretch();
    baseLayout->addLayout(settingsLayout);

    // Funny things happen if the pie slice labels do not fit the screen, so we ignore size policy
    m_chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    baseLayout->addWidget(m_chartView);
    setLayout(baseLayout);
}

void DisplayPrintChartWidget::connectSignals()
{
    connect(m_themeComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &DisplayPrintChartWidget::updateUI);
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

    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 24; ++i) {
        Data point;
        point.first = now.addDays(i); // Каждый день
        point.second = (qreal)(qrand() % valueMax);      // Какие-то тестовые данные
        dataTable.append(point);
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

void DisplayPrintChartWidget::setData(DataTable dataTable)
{
    Q_UNUSED(dataTable);
    m_dataTable = generateRandomData(3, 10, 7);
    //m_dataTable = dataTable;
    updateChart();
}

void DisplayPrintChartWidget::updateUI()
{
    QtCharts::QChart::ChartTheme theme = static_cast<QtCharts::QChart::ChartTheme>(
        m_themeComboBox->itemData(m_themeComboBox->currentIndex()).toInt());
    m_chartView->chart()->setTheme(theme);

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
    ChartCreator* chartCreator = nullptr;
    if(newType == "BarChart") {
        chartCreator = new BarChartCreator;
    } else if (newType == "PieChart") {
        chartCreator = new PieChartCreator;
    }
    if(chartCreator) {
        m_chartView->setChart(chartCreator->createChart(m_dataTable));
        delete chartCreator;
    }
    delete chart;
    updateUI();
}

void DisplayPrintChartWidget::printChart()
{
    if (!m_chartView) {
        QMessageBox::warning(this, "Ошибка", "График не задан.");
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
    if (!filePath.endsWith(".pdf")) {
        filePath += ".pdf";
    }

    // 2. Настраиваем генератор PDF
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPagedPaintDevice::A4);

    // 3. Рендеринг в PDF
    QPainter painter(&pdfWriter);
    m_chartView->render(&painter);
    painter.end();
}
