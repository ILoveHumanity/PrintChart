#include "MainWindow.h"
#include <QLabel>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDockWidget>


#include <QListView>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QHeaderView>

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

        m_fileModel = new QFileSystemModel(this);
        m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
        //Объявили модельный индекс
        QModelIndex index;
        index = m_fileModel->setRootPath(dirPath); //Получили индекс из модели

        m_tableView = new QTableView;
        m_tableView->setModel(m_fileModel);
        m_tableView->setRootIndex(index);

        m_displayPrintChartWidget = new DisplayPrintChartWidget();

        QSplitter *splitter = new QSplitter(this);
        splitter->addWidget(m_tableView);
        splitter->addWidget(m_displayPrintChartWidget);
        setCentralWidget(splitter);


        QItemSelectionModel *selectionModel = m_tableView->selectionModel();
        //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в m_tableView
        connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::on_selectionChangedSlot);
    }
    else {
        this->statusBar()->showMessage("Error");
    }
}

void MainWindow::on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    //Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndex index = m_tableView->selectionModel()->currentIndex();

    QModelIndexList indexs =  selected.indexes();

    QString filePath = "";

    // Размещаем информацию в statusbar относительно выделенного модельного индекса
    /*
     * Смотрим, сколько индексов было выделено.
     * В нашем случае выделяем только один, следовательно всегда берем только первый.
    */
    if (indexs.count() >= 1) {
        QModelIndex ix =  indexs.constFirst();
        filePath = m_fileModel->filePath(ix);
        this->statusBar()->showMessage("Выбранный путь : " + filePath);

        // get data

        m_displayPrintChartWidget->setData(DataTable());
    }
}


