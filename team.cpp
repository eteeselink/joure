#include "team.h"
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include "http.h"
#include "vendor/benlau/asyncfuture.h"

Team::Team(QString teamId, QString username, QString displayName) :
    teamId(teamId),
    username(username),
    displayName(displayName)
{

}


void Team::fetchData() {
    get<int>("/teams/" + teamId + ".json", [](auto reply) {
        qDebug() << "FETCHED" << reply->readAll();
        return 0;
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
        put<int>(path, payload, [](auto reply) {
            qDebug() << "BOO" << reply->readAll();
            return 0;
        });
    }

    {
        // i bet there's a way to stream this stuff into manager->put without copying the entire image over
        // but i don't know how.
        auto payload = "\"" + base64image + "\"";

        put<int>("/images/" + imageId + ".json?print=silent", payload, [](auto reply) {
            qDebug() << "MOO" << reply->readAll();
            return 0;
        });
    }
}
