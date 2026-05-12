#ifndef DIETOPTIMIZER_H
#define DIETOPTIMIZER_H

#include <QVector>
#include <QVariantMap>
#include <QPair>

struct DietPlan {
    QVector<QPair<QString, double>> items; // название, граммы
    double totalCalories = 0;
    double totalProtein = 0;
    double totalFat = 0;
    double totalCarbs = 0;
    double totalCost = 0;
};

class DietOptimizer
{
public:
    DietOptimizer();
    DietPlan optimize(const QVector<QVariantMap> &products, double targetCal, double targetProt,
                      double targetFat, double targetCarb, double budget, bool noDairy = false);
};

#endif