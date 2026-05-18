#include "dietoptimizer.h"
#include <algorithm>

DietOptimizer::DietOptimizer() {}

DietPlan DietOptimizer::optimize(const QVector<QVariantMap> &products,
                                 double targetCal, double targetProt,
                                 double targetFatVal, double targetCarb,
                                 double budget,
                                 HealthProfile profile,
                                 bool diverse)
{
    DietPlan plan;
    (void)targetFatVal;
    (void)targetCarb;

    // умная фильтрация и штрафы
    QVector<QVariantMap> filtered;

    for (const auto &p : products) {

        QVariantMap product = p;

        QString tags = product["tags"].toString();

        double sugar = product["sugar"].toDouble();
        double salt  = product["salt"].toDouble();
        double price = product["price"].toDouble();

        bool banned = false;
        double penalty = 1.0;

        // диабет
        if (profile == HealthProfile::Diabetic) {

            // очень сладкое запрещаем
            if (sugar > 15.0)
                banned = true;

            // умеренно сладкое делаем менее выгодным
            else if (sugar > 5.0)
                penalty = 1.5;
        }

        // гипертония
        if (profile == HealthProfile::Hypertension) {

            if (salt > 2.0)
                banned = true;

            else if (salt > 0.5)
                penalty = 1.4;
        }
        // без глютена
        if (profile == HealthProfile::GlutenFree &&
            !tags.contains("gluten_free")) {

            banned = true;
        }
        // без лактозы
        if (profile == HealthProfile::LactoseIntolerant &&
            !tags.contains("lactose_free")) {

            banned = true;
        }
        if (banned)
            continue;
        // применяем штраф
        product["price"] = price * penalty;

        filtered.append(product);
    }

    // категории уже использованных продуктов
    QStringList usedCategories;
    QMap<QString, int> categoryCount;
    // ограничение повторов
    QMap<QString, int> usageCount;

    // сортировка по выгодности
    std::sort(filtered.begin(), filtered.end(),
              [](const QVariantMap &a, const QVariantMap &b) {

        double bonusA = 1.0;
        double bonusB = 1.0;

        if (a["category"].toString() == "vegetables")
            bonusA = 1.3;

        if (a["category"].toString() == "protein")
            bonusA = 1.2;

        if (b["category"].toString() == "vegetables")
            bonusB = 1.3;

        if (b["category"].toString() == "protein")
            bonusB = 1.2;

        double scoreA =
            ((a["protein"].toDouble() * 4.0 +
              a["calories"].toDouble()) * bonusA)
            / a["price"].toDouble();

        double scoreB =
            ((b["protein"].toDouble() * 4.0 +
              b["calories"].toDouble()) * bonusB)
            / b["price"].toDouble();
                  return scoreA > scoreB;
              });

    // основной проход
    // ===== 1. ОБЯЗАТЕЛЬНЫЙ БЕЛОК =====

    for (const auto &p : filtered) {

        QString category = p["category"].toString();

        if (category != "protein")
            continue;

        double grams = 200.0;

        double cost =
            p["price"].toDouble() * grams / 100.0;

        if (plan.totalCost + cost > budget)
            continue;

        plan.items.append({
            p["name"].toString(),
            grams
        });

        plan.totalCalories +=
            p["calories"].toDouble() * grams / 100.0;

        plan.totalProtein +=
            p["protein"].toDouble() * grams / 100.0;

        plan.totalFat +=
            p["fat"].toDouble() * grams / 100.0;

        plan.totalCarbs +=
            p["carbs"].toDouble() * grams / 100.0;

        plan.totalCost += cost;

        break;
    }

    // ===== 2. УГЛЕВОДЫ =====

    for (const auto &p : filtered) {

        QString category = p["category"].toString();

        if (category != "carbs")
            continue;

        double grams = 150.0;

        double cost =
            p["price"].toDouble() * grams / 100.0;

        if (plan.totalCost + cost > budget)
            continue;

        plan.items.append({
            p["name"].toString(),
            grams
        });

        plan.totalCalories +=
            p["calories"].toDouble() * grams / 100.0;

        plan.totalProtein +=
            p["protein"].toDouble() * grams / 100.0;

        plan.totalFat +=
            p["fat"].toDouble() * grams / 100.0;

        plan.totalCarbs +=
            p["carbs"].toDouble() * grams / 100.0;

        plan.totalCost += cost;

        break;
    }

    // ===== 3. ОВОЩИ ИЛИ ФРУКТЫ =====

    for (const auto &p : filtered) {

        QString category = p["category"].toString();

        if (category != "vegetables" &&
            category != "fruits")
            continue;

        double grams = 150.0;

        double cost =
            p["price"].toDouble() * grams / 100.0;

        if (plan.totalCost + cost > budget)
            continue;

        plan.items.append({
            p["name"].toString(),
            grams
        });

        plan.totalCalories +=
            p["calories"].toDouble() * grams / 100.0;

        plan.totalProtein +=
            p["protein"].toDouble() * grams / 100.0;

        plan.totalFat +=
            p["fat"].toDouble() * grams / 100.0;

        plan.totalCarbs +=
            p["carbs"].toDouble() * grams / 100.0;

        plan.totalCost += cost;

        break;
    }

    // ===== 4. ДОБОР КАЛОРИЙ =====

    for (const auto &p : filtered) {

        if (plan.totalCalories >= targetCal * 0.9)
            break;

        QString name = p["name"].toString();

        bool alreadyUsed = false;

        for (const auto &item : plan.items) {

            if (item.first == name) {
                alreadyUsed = true;
                break;
            }
        }

        if (alreadyUsed)
            continue;

        double grams = 100.0;

        QString category = p["category"].toString();

        if (category == "fats")
            grams = 10.0;

        double cost =
            p["price"].toDouble() * grams / 100.0;

        if (plan.totalCost + cost > budget)
            continue;

        plan.items.append({
            name,
            grams
        });

        plan.totalCalories +=
            p["calories"].toDouble() * grams / 100.0;

        plan.totalProtein +=
            p["protein"].toDouble() * grams / 100.0;

        plan.totalFat +=
            p["fat"].toDouble() * grams / 100.0;

        plan.totalCarbs +=
            p["carbs"].toDouble() * grams / 100.0;

        plan.totalCost += cost;
    }
    return plan;
}