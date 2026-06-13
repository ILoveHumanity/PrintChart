#include "MainWindow.h"
#include "RawDataProcessor.h"
#include <QLabel>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QListView>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    //Устанавливаем размер главного окна
    this->setGeometry(100, 100, 1500, 500);
    this->setStatusBar(new QStatusBar(this));

    QString dirPath = QFileDialog::getExistingDirectory( this, "Select Directory",
            QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dirPath.isEmpty()) {
        this->statusBar()->showMessage("Choosen Path: " + dirPath);
        QSplitter *splitter = new QSplitter(this);
        if(splitter){
            m_fileModel = new QFileSystemModel(this);
            if(m_fileModel){
                m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
                QModelIndex index = m_fileModel->setRootPath(dirPath); //Получили индекс из модели
                m_tableView = new QTableView;
                if(m_tableView){
                    m_tableView->setModel(m_fileModel);
                    m_tableView->setRootIndex(index);

                    splitter->addWidget(m_tableView);

                    QItemSelectionModel *selectionModel = m_tableView->selectionModel();
                    //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в m_tableView
                    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::on_selectionChangedSlot);
                    //connect(selectionModel, &QItemSelectionModel::currentRowChanged, this, &MainWindow::on_currentRowChangedSlot);
                }
                m_displayPrintChartWidget = new DisplayPrintChartWidget();
                if(m_displayPrintChartWidget){
                    splitter->addWidget(m_displayPrintChartWidget);

                    m_displayPrintChartWidget->isOk();
                }
            }
            setCentralWidget(splitter);
        }
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Не выбрана папка с данными");
    }
}

//void MainWindow::on_currentRowChangedSlot(const QModelIndex &current, const QModelIndex &previous)
//{
//    Q_UNUSED(previous);

//    if (current.isValid()) {
//        QString filePath = m_fileModel->filePath(current);
//        this->statusBar()->showMessage("Выбранный путь : " + filePath);

//        m_displayPrintChartWidget->setData(DataTable());
//    }
//}

void MainWindow::on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    //Q_UNUSED(selected);
    Q_UNUSED(deselected);

    //QModelIndex index = m_tableView->selectionModel()->currentIndex();

    QModelIndexList indexs =  selected.indexes();

    QString filePath = "";

    // Размещаем информацию в statusbar относительно выделенного модельного индекса
    /*
     * Смотрим, сколько индексов было выделено.
     * В нашем случае выделяем только один, следовательно всегда берем только первый.
    */
    if (indexs.count() >= 1) {
        QModelIndex ix = indexs.constFirst();
        filePath = m_fileModel->filePath(ix);
        this->statusBar()->showMessage("Выбранный путь : " + filePath);
        DataTable dataTable;
        QString suffix = QFileInfo(filePath).suffix();

        RawDataProcessor* rawDataProcessor = nullptr;
        QString Error;
        if(suffix == "sqlite") {
            rawDataProcessor = new SQLiteRawDataProcessor;
        } else if (suffix == "json") {
            rawDataProcessor = new JsonRawDataProcessor;
        } else {
            QMessageBox::critical(this, "Ошибка", "Неподдерживаемый формат данных.");
            return;
        }
        if(rawDataProcessor) {
            dataTable = rawDataProcessor->getData(filePath, Error);
            delete rawDataProcessor;
            if(Error.isEmpty()){
                m_displayPrintChartWidget->setData(dataTable);
            } else {
                QMessageBox::critical(this, "Ошибка обработки данных", Error);
            }
        }
    }
    return;
}


