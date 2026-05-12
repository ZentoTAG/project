#include "dietoptimizer.h"
#include <algorithm>

DietOptimizer::DietOptimizer() {}

DietPlan DietOptimizer::optimize(const QVector<QVariantMap> &products, double targetCal, double targetProt,
                                 double targetFatVal, double targetCarb, double budget, bool noDairy)
{
    DietPlan plan;
    (void)targetFatVal;  // пока не используется
    (void)targetCarb;    // пока не используется

    // фильтр продуктов по огр. (молочка)
    QVector<QVariantMap> filtered;
    for (const auto &p : products) {
        const QString name = p["name"].toString();
        if (noDairy && (name == QStringLiteral("Молоко") || name == QStringLiteral("Творог")))
            continue;
        filtered.append(p);
    }

    // 1. добор белка (нужен дешёвый белок)
    QVector<QVariantMap> byProt = filtered;
    std::sort(byProt.begin(), byProt.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return a["protein"].toDouble() / a["price"].toDouble()
        > b["protein"].toDouble() / b["price"].toDouble();
    });

    for (const auto &p : byProt) {
        if (plan.totalProtein >= targetProt) break;

        double price = p["price"].toDouble();
        double prot = p["protein"].toDouble();
        double cal = p["calories"].toDouble();
        double fat = p["fat"].toDouble();
        double carb = p["carbs"].toDouble();

        double grams = (targetProt - plan.totalProtein) / prot * 100.0;
        grams = qBound(50.0, grams, 300.0);

        double addCal = cal * (grams / 100.0);
        double addCost = price * (grams / 100.0);
        if (plan.totalCost + addCost > budget) continue;

        plan.items.append({p["name"].toString(), grams});
        plan.totalCalories += addCal;
        plan.totalProtein += prot * (grams / 100.0);
        plan.totalFat += fat * (grams / 100.0);
        plan.totalCarbs += carb * (grams / 100.0);
        plan.totalCost += addCost;
    }

    // 2. добор калорий (нужны дешёвые калории)
    QVector<QVariantMap> byCal = filtered;
    std::sort(byCal.begin(), byCal.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return a["calories"].toDouble() / a["price"].toDouble()
        > b["calories"].toDouble() / b["price"].toDouble();
    });

    for (const auto &p : byCal) {
        if (plan.totalCalories >= targetCal) break;

        double price = p["price"].toDouble();
        double cal = p["calories"].toDouble();
        double prot = p["protein"].toDouble();
        double fat = p["fat"].toDouble();
        double carb = p["carbs"].toDouble();

        double grams = (targetCal - plan.totalCalories) / cal * 100.0;
        grams = qBound(50.0, grams, 300.0);

        double addCal = cal * (grams / 100.0);
        double addCost = price * (grams / 100.0);
        if (plan.totalCost + addCost > budget) continue;

        plan.items.append({p["name"].toString(), grams});
        plan.totalCalories += addCal;
        plan.totalProtein += prot * (grams / 100.0);
        plan.totalFat += fat * (grams / 100.0);
        plan.totalCarbs += carb * (grams / 100.0);
        plan.totalCost += addCost;
    }

    return plan;
}