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


void MainWindow::on_generateBtn_clicked() {
    // Настраиваем таблицу: 3 строки, 4 колонки
    ui->planTable->setRowCount(3);
    ui->planTable->setColumnCount(4);
    ui->planTable->setHorizontalHeaderLabels({"Продукт", "Граммы", "Цена", "БЖУ"});

    // Временные примеры данных
    ui->planTable->setItem(0, 0, new QTableWidgetItem("Гречка"));
    ui->planTable->setItem(0, 1, new QTableWidgetItem("200 г"));
    ui->planTable->setItem(0, 2, new QTableWidgetItem("25 ₽"));
    ui->planTable->setItem(0, 3, new QTableWidgetItem("Б: 12, Ж: 3, У: 68"));

    ui->planTable->setItem(1, 0, new QTableWidgetItem("Курица"));
    ui->planTable->setItem(1, 1, new QTableWidgetItem("150 г"));
    ui->planTable->setItem(1, 2, new QTableWidgetItem("45 ₽"));
    ui->planTable->setItem(1, 3, new QTableWidgetItem("Б: 35, Ж: 5, У: 0"));

    ui->planTable->setItem(2, 0, new QTableWidgetItem("Молоко"));
    ui->planTable->setItem(2, 1, new QTableWidgetItem("400 мл"));
    ui->planTable->setItem(2, 2, new QTableWidgetItem("20 ₽"));
    ui->planTable->setItem(2, 3, new QTableWidgetItem("Б: 12, Ж: 10, У: 18"));

    ui->planStatusLabel->setText("Это демо-план. Позже будет реальный расчёт.");
}


void MainWindow::on_budgetCalcBtn_clicked()
{
    QString goalName = ui->goalNameInput->text();
    double goalAmount = ui->goalAmountInput->text().toDouble();
    double dailyBudget = 200.0;
    double monthlyIncome = 10000.0;
    double monthlyFoodCost = dailyBudget * 30;
    double monthlySavings = monthlyIncome - monthlyFoodCost;

    if (goalAmount <= 0) {
        ui->budgetResultLabel->setText("Введите корректную сумму цели.");
        return;
    }

    int months = static_cast<int>(goalAmount / monthlySavings) + 1;
    ui->budgetResultLabel->setText(
        QString("На цель «%1» потребуется %2 мес. (откладывая %3 ₽/мес.)")
            .arg(goalName)
            .arg(months)
            .arg(monthlySavings, 0, 'f', 0)
        );

    ui->budgetProgressBar->setRange(0, static_cast<int>(goalAmount));
    ui->budgetProgressBar->setValue(0);
    ui->budgetProgressLabel->setText("0 / " + QString::number(goalAmount, 'f', 0) + " ₽");
}