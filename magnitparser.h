#ifndef MAGNITPARSER_H
#define MAGNITPARSER_H

#include <QVector>
#include <QVariantMap>

class MagnitParser
{
public:
    MagnitParser();

    QVector<QVariantMap> loadProducts();
};

#endif // MAGNITPARSER_H