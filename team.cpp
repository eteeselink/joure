#include "team.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>

Team::Team(QString teamId, QString username, QString displayName) :
    teamId(teamId),
    username(username),
    displayName(displayName)
{

}

void Team::fetchData() {
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request;

    QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
    ssl.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(ssl);
    request.setUrl(QUrl("https://joure-public.firebaseio.com/teams/" + teamId + ".json"));
    request.setHeader(QNetworkRequest::ServerHeader, "application/json");

    manager->get(request);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
                     SLOT(gotData(QNetworkReply*)));
}

void Team::gotData(QNetworkReply* reply)
{
  qDebug() << reply->readAll();
}
