#include "magnitparser.h"
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

MagnitParser::MagnitParser()
{

}

QVector<QVariantMap> MagnitParser::loadProducts()
{
    QVector<QVariantMap> products;

    QNetworkAccessManager manager;

    QNetworkRequest request(
        QUrl("https://magnit.ru/webgate/recoms/v1/recommendations")
        );

    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");

    request.setRawHeader("accept", "application/json");
    request.setRawHeader("referer", "https://magnit.ru/");
    request.setRawHeader("origin", "https://magnit.ru");

    request.setRawHeader("user-agent",
                         "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                         "(KHTML, like Gecko) Chrome/148.0.0.0 Safari/537.36");

    request.setRawHeader("x-app-version", "2026.5.17-13.34");
    request.setRawHeader("x-client-name", "magnit");
    request.setRawHeader("x-device-platform", "Web");
    request.setRawHeader("x-device-id",
                         "12345678-1234-1234-1234-123456789012");
    request.setRawHeader("x-new-magnit", "true");

    request.setRawHeader("sec-ch-ua",
                         "\"Not/A)Brand\";v=\"99\", \"Chromium\";v=\"148\"");

    request.setRawHeader("sec-ch-ua-mobile", "?0");
    request.setRawHeader("sec-ch-ua-platform", "\"Linux\"");
    QJsonObject store;
    store["code"] = "384899";
    store["catalogType"] = "3";
    store["service"] = "express";

    QJsonArray stores;
    stores.append(store);

    QJsonObject bodyObj;
    bodyObj["catalogType"] = "3";
    bodyObj["goodsIds"] = QJsonArray();
    bodyObj["limit"] = 12;
    bodyObj["offset"] = 0;
    bodyObj["service"] = "express";
    bodyObj["stores"] = stores;
    bodyObj["type"] = "main_endless";

    QByteArray body =
        QJsonDocument(bodyObj)
            .toJson(QJsonDocument::Compact);

    qDebug() << body;

    QNetworkReply *reply = manager.post(request, body);

    QEventLoop loop;

    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     &QEventLoop::quit);

    loop.exec();

    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Ошибка:"
                 << reply->errorString();

        qDebug() << reply->readAll();

        return products;
    }

    QByteArray data = reply->readAll();
    qDebug() << data;
    QJsonDocument doc =
        QJsonDocument::fromJson(data);

    QJsonObject root = doc.object();

    QJsonArray items =
        root["items"].toArray();

    for(const QJsonValue &value : items)
    {
        QJsonObject obj = value.toObject();

        QString name =
            obj["name"].toString().toLower();


        // фильтрация мусора

        if (name.contains("корм") ||
            name.contains("kitekat") ||
            name.contains("whiskas") ||
            name.contains("napitok") ||
            name.contains("напиток") ||
            name.contains("cola") ||
            name.contains("pepsi") ||
            name.contains("sprite") ||
            name.contains("fanta"))
        {
            continue;
        }

        // очистка названия

        name.remove(
            QRegularExpression(
                "\\d+\\s?(г|кг|мл|л)"
                )
            );

        name.remove("(");
        name.remove(")");

        name = name.trimmed();


        // создание продукта

        QVariantMap product;

        product["name"] = name;

        product["price"] =
            obj["price"].toInt() / 100.0;

        product["category"] =
            "Магнит";

        product["calories"] = 0;
        product["protein"] = 0;
        product["fat"] = 0;
        product["carbs"] = 0;
        product["sugar"] = 0;
        product["salt"] = 0;
        product["tags"] = "";

        products.append(product);
    }

    return products;
}