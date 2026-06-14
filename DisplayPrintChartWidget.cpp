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
    state(true),
    m_dataTable(),
    m_themeComboBox(createThemeBox()),
    m_legendComboBox(createLegendBox()),
    m_typeComboBox(createTypeBox()),
    m_chartView(new QtCharts::QChartView(this)),
    m_printButton(new QPushButton("Печать", this))
{
    if(!m_themeComboBox || !m_legendComboBox || !m_typeComboBox || !m_chartView || !m_printButton){
        state = false;
    } else {
        connectSignals();
        // create layout
        QVBoxLayout *baseLayout = new QVBoxLayout(this);
        if(baseLayout){
            QHBoxLayout *settingsLayout = new QHBoxLayout();
            if(settingsLayout){
                settingsLayout->addWidget(new QLabel("Theme:", this));
                settingsLayout->addWidget(m_themeComboBox);
                settingsLayout->addWidget(new QLabel("Type:", this));
                settingsLayout->addWidget(m_typeComboBox);
                settingsLayout->addWidget(new QLabel("Legend:", this));
                settingsLayout->addWidget(m_legendComboBox);
                settingsLayout->addWidget(m_printButton);
                settingsLayout->addStretch();
                baseLayout->addLayout(settingsLayout);
            } else {
                state = false;
            }
            m_chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            baseLayout->addWidget(m_chartView);
        } else {
            state = false;
        }
        m_printButton->setEnabled(false);
    }
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

QComboBox *DisplayPrintChartWidget::createThemeBox()
{
    // settings layout
    QComboBox *themeComboBox = new QComboBox(this);
    if(themeComboBox){
        themeComboBox->addItem("Light", QtCharts::QChart::ChartThemeLight);
        themeComboBox->addItem("Blue Cerulean", QtCharts::QChart::ChartThemeBlueCerulean);
        themeComboBox->addItem("Dark", QtCharts::QChart::ChartThemeDark);
        themeComboBox->addItem("Brown Sand", QtCharts::QChart::ChartThemeBrownSand);
        themeComboBox->addItem("Blue NCS", QtCharts::QChart::ChartThemeBlueNcs);
        themeComboBox->addItem("High Contrast", QtCharts::QChart::ChartThemeHighContrast);
        themeComboBox->addItem("Blue Icy", QtCharts::QChart::ChartThemeBlueIcy);
        themeComboBox->addItem("Qt", QtCharts::QChart::ChartThemeQt);
        themeComboBox->addItem("Black and White", 8);
    }
    return themeComboBox;
}

QComboBox *DisplayPrintChartWidget::createTypeBox()
{
    // settings layout
    QComboBox *typeComboBox = new QComboBox(this);
    if(typeComboBox){
        typeComboBox->addItem("BarChart", "BarChart");
        typeComboBox->addItem("PieChart", "PieChart");
    }
    return typeComboBox;
}

QComboBox *DisplayPrintChartWidget::createLegendBox()
{
    QComboBox *legendComboBox = new QComboBox(this);
    if(legendComboBox){
        legendComboBox->addItem("No Legend ", 0);
        legendComboBox->addItem("Legend Top", Qt::AlignTop);
        legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
        legendComboBox->addItem("Legend Left", Qt::AlignLeft);
        legendComboBox->addItem("Legend Right", Qt::AlignRight);
    }
    return legendComboBox;
}

bool DisplayPrintChartWidget::isOk()
{
    return state;
}

void DisplayPrintChartWidget::setData(DataTable dataTable)
{
    if(!state){
        return;
    }
    m_dataTable = dataTable;
    m_printButton->setEnabled(true);
    updateChart();
}

void DisplayPrintChartWidget::updateUI()
{
    if(!state || !m_chartView->chart()){
        return;
    }

    int themeData = m_themeComboBox->itemData(m_themeComboBox->currentIndex()).toInt();
    // Проверяем, выбрана ли черно-белая тема
    if (themeData == 8) {
        QChart *chart = m_chartView->chart();

        // Устанавливаем белый фон
        chart->setBackgroundBrush(QBrush(Qt::white));
        chart->setPlotAreaBackgroundBrush(QBrush(Qt::white));

        // Настраиваем цвета серий
        for (QAbstractSeries *series : chart->series()) {

            if (auto barSeries = qobject_cast<QBarSeries*>(series)) {
                for (QBarSet *barSet : barSeries->barSets()) {
                    barSet->setColor(Qt::gray);
                    barSet->setLabelColor(Qt::black);
                }
            } else if (auto pieSeries = qobject_cast<QPieSeries*>(series)) {
                for (QPieSlice *slice : pieSeries->slices()) {
                    slice->setColor(Qt::gray);
                    slice->setLabelColor(Qt::black);
                    slice->setBorderColor(Qt::black);
                }
            }

            series->setVisible(true);
        }
    } else {
        QtCharts::QChart::ChartTheme theme = static_cast<QtCharts::QChart::ChartTheme>(themeData);
        m_chartView->chart()->setTheme(theme);
    }

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
    if(!state || m_dataTable.empty()){
        return;
    }
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
    if(chart) {
        delete chart;
    }
    updateUI();
}

void DisplayPrintChartWidget::printChart()
{
    if(!state){
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
