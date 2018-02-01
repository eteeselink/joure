#include "team.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>

#include "vendor/benlau/asyncfuture.h"
using namespace AsyncFuture;

Team::Team(QString teamId, QString username, QString displayName) :
    teamId(teamId),
    username(username),
    displayName(displayName)
{

}

struct Req {
    QNetworkRequest request;
    QNetworkAccessManager *manager;
};

void fetch(
        QString path,
        std::function<void (QNetworkAccessManager*, QNetworkRequest&)> exec,
        std::function<void (QNetworkReply*)> handle
    ) {
    QNetworkRequest request;

    QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
    ssl.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(ssl);
    request.setHeader(QNetworkRequest::ServerHeader, "application/json");
    request.setUrl(QUrl("https://joure-public.firebaseio.com" + path));

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    exec(manager, request);

    QObject::connect(
        manager, &QNetworkAccessManager::finished,
        [=]( QNetworkReply* reply ) {
            handle(reply);
            reply->deleteLater();
            manager->deleteLater();
        }
   );

}

void put(const QString& path, QByteArray& data, std::function<void (QNetworkReply*)> handle) {
    fetch(
        path,
        [&](auto manager, auto request) { manager->put(request, data); },
        handle
    );
}

void get(const QString& path, std::function<void (QNetworkReply*)> handle) {
    fetch(
        path,
        [](auto manager, auto request) { manager->get(request); },
        handle
    );
}

void Team::fetchData() {
    get("/teams/" + teamId + ".json", [](auto reply) {
        qDebug() << "FETCHED" << reply->readAll();
    });
}

void Team::upload(QByteArray& base64image) {

    // image id is just a hash of the team id and the username (and thus just as secret as the actual username)
    auto imageIdSource = (teamId.replace("|","||") + "|" + teamId.replace("|","||")).toLocal8Bit();
    auto hash = QCryptographicHash::hash(imageIdSource, QCryptographicHash::Sha1);
    auto imageId = QString(hash.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));

    {
        QJsonObject member;
        member.insert("lastShot", QJsonDocument::fromJson(R"({".sv": "timestamp"})").object());
        member.insert("displayName", displayName);
        member.insert("imageId", imageId);
        auto payload = QJsonDocument(member).toJson();

        auto path = "/teams/" + teamId + "/members/" + username + ".json";
        put(path, payload, [](auto reply) {
            qDebug() << "BOO" << reply->readAll();
        });
    }

    {
        // i bet there's a way to stream this stuff into manager->put without copying the entire image over
        // but i don't know how.
        auto payload = "\"" + base64image + "\"";

        put("/images/" + imageId + ".json?print=silent", payload, [](auto reply) {
            qDebug() << "MOO" << reply->readAll();
        });
    }
}
