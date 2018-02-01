#ifndef TEAM_H
#define TEAM_H

#include <QNetworkReply>
#include <QObject>

class Team : public QObject
{
    Q_OBJECT

public:
    Team(QString teamId, QString username, QString displayName);
    void fetchData();
    QString teamId;
    QString username;
    QString displayName;
private slots:

    void gotData(QNetworkReply *reply);
};

#endif // TEAM_H
