#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "database.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_calculateBtn_clicked();

    void on_generateBtn_clicked();

    void on_budgetCalcBtn_clicked();

private:
    Database m_db;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
