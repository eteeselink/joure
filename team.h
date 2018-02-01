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
    void upload(QByteArray& pixels);
    QString teamId;
    QString username;
    QString displayName;
};

#endif // TEAM_H
