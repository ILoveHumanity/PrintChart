#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DisplayPrintChartWidget.h"
#include <QPushButton>
#include <QFileSystemModel>
#include <QTableView>

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QFileSystemModel *m_fileModel;
    QTableView *m_tableView;
    DisplayPrintChartWidget *m_displayPrintChartWidget;
private slots:
    //void on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected);
    void on_currentRowChangedSlot(const QModelIndex &current, const QModelIndex &previous);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
};
#endif // MAINWINDOW_H
