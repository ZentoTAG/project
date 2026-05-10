#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_db.openDatabase("products.db");
    ui->tabWidget->setTabText(0, "Профиль");
    ui->tabWidget->setTabText(1, "План питания");
    ui->tabWidget->setTabText(2, "Бюджет");

    this->setMinimumSize(600, 400);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_calculateBtn_clicked(){
    // Считываем возраст, вес и рост из полей ввода
    int age = ui->ageInput->text().toInt();
    double weight = ui->weightInput->text().toDouble();
    double height = ui->heightInput->text().toDouble();

    if (age <= 0 || weight <= 0 || height <= 0) {
        ui->resultLabel->setText("Пожалуйста, заполните все поля корректно.");
        return;
    }

    double bmr = 10 * weight + 6.25 * height - 5 * age + 5;

    double calories = bmr * 1.375;

    double protein = calories * 0.30 / 4;    // 1 г белка = 4 ккал
    double fat = calories * 0.25 / 9;        // 1 г жира = 9 ккал
    double carbs = calories * 0.45 / 4;      // 1 г углеводов = 4 ккал

    // Выводим результат
    QString result = QString(
                         "Суточная норма: %1 ккал\n"
                         "Белки: %2 г\n"
                         "Жиры: %3 г\n"
                         "Углеводы: %4 г"
                         ).arg(calories, 0, 'f', 0)
                         .arg(protein, 0, 'f', 1)
                         .arg(fat, 0, 'f', 1)
                         .arg(carbs, 0, 'f', 1);

    ui->resultLabel->setText(result);
    m_db.saveUser(age, weight, height);
}

