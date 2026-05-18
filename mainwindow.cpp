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
    // считываем возраст, вес и рост из полей ввода
    int age = ui->ageInput->text().toInt();
    double weight = ui->weightInput->text().toDouble();
    double height = ui->heightInput->text().toDouble();

    if (age <= 0 || weight <= 0 || height <= 0) {
        ui->resultLabel->setText("Пожалуйста, заполните все поля корректно.");
        return;
    }

    double bmr = 10 * weight + 6.25 * height - 5 * age + 5;

    double calories = bmr * 1.375;

    double protein = calories * 0.20 / 4;
    double fat = calories * 0.25 / 9;
    double carbs = calories * 0.55 / 4;

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


void MainWindow::on_generateBtn_clicked()
{
    QVector<QVariantMap> products = m_db.getProducts();
    if (products.isEmpty()) {
        ui->planStatusLabel->setText("База продуктов пуста.");
        return;
    }

    QVariantMap user = m_db.getLastUser();
    double dailyCalories = 2200.0;
    double dailyProtein, dailyFat, dailyCarbs; // объявление без инициализации

    if (!user.isEmpty()) {
        int age = user["age"].toInt();
        double weight = user["weight"].toDouble();
        double height = user["height"].toDouble();

        if (age > 0 && weight > 0 && height > 0) {
            double bmr = 10 * weight + 6.25 * height - 5 * age + 5;
            dailyCalories = bmr * 1.375;
        }
    }

    // единый правильный расчёт для обоих случаев
    dailyProtein = dailyCalories * 0.20 / 4.0;   // 20% белки
    dailyFat    = dailyCalories * 0.25 / 9.0;    // 25% жиры
    dailyCarbs  = dailyCalories * 0.55 / 4.0;    // 55% углеводы

    double dailyBudget = ui->foodBudgetInput->text().isEmpty()
                             ? 200.0
                             : ui->foodBudgetInput->text().toDouble();

    // определяем профиль
    HealthProfile profile = HealthProfile::Normal;
    QString hp = ui->healthCombo->currentText();
    if (hp == "Диабет")           profile = HealthProfile::Diabetic;
    else if (hp == "Гипертония")  profile = HealthProfile::Hypertension;
    else if (hp == "Без глютена") profile = HealthProfile::GlutenFree;
    else if (hp == "Без лактозы") profile = HealthProfile::LactoseIntolerant;

    bool diverse = ui->diverseCheck->isChecked();
    qDebug() << "diverse:" << diverse;
    DietPlan planResult = m_optimizer.optimize(products, dailyCalories, dailyProtein,
                                               dailyFat, dailyCarbs, dailyBudget, profile, diverse);

    ui->planTable->setRowCount(planResult.items.size());
    ui->planTable->setColumnCount(3);
    ui->planTable->setHorizontalHeaderLabels({"Продукт", "Граммы", "Цена"});

    for (int i = 0; i < planResult.items.size(); ++i) {
        ui->planTable->setItem(i, 0, new QTableWidgetItem(planResult.items[i].first));
        double grams = planResult.items[i].second;
        ui->planTable->setItem(i, 1, new QTableWidgetItem(QString::number(grams, 'f', 0) + " г"));

        double pricePer100g = 0.0;
        for (const auto &p : products) {
            if (p["name"].toString() == planResult.items[i].first) {
                pricePer100g = p["price"].toDouble();
                break;
            }
        }
        double cost = pricePer100g * (grams / 100.0);
        ui->planTable->setItem(i, 2, new QTableWidgetItem(QString::number(cost, 'f', 1) + " ₽"));
    }

    ui->planStatusLabel->setText(
        QString("План на день. Калорий: %1, Белков: %2 г, Жиров: %3 г, Углеводов: %4 г, Бюджет: %5 ₽")
            .arg(dailyCalories, 0, 'f', 0)
            .arg(dailyProtein, 0, 'f', 1)
            .arg(dailyFat, 0, 'f', 1)
            .arg(dailyCarbs, 0, 'f', 1)
            .arg(planResult.totalCost, 0, 'f', 0));
}


void MainWindow::on_budgetCalcBtn_clicked()
{
    QString goalName = ui->goalNameInput->text();
    double goalAmount = ui->goalAmountInput->text().toDouble();
    double monthlyIncome = ui->incomeInput->text().toDouble();
    double dailyFoodBudget = ui->foodBudgetInput->text().toDouble();

    if (goalAmount <= 0 || monthlyIncome <= 0 || dailyFoodBudget <= 0) {
        ui->budgetResultLabel->setText("Заполните все поля корректными числами.");
        return;
    }

    double monthlyFoodCost = dailyFoodBudget * 30;
    double monthlySavings = monthlyIncome - monthlyFoodCost;

    if (monthlySavings <= 0) {
        ui->budgetResultLabel->setText("С такими расходами накопить не получится.");
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