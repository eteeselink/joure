#include "mainwindow.h"
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include "team.h"

void msg(QString str) {
    QMessageBox::information(NULL, "Joure", str);
}

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

void createStandardIniFile(QString& iniFn) {
    QFile iniFile(iniFn);
    if(iniFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&iniFile);
        stream << "# Joure configuration. Please fill this out and then restart Joure." << endl
               << endl
               << "[main]" << endl
               << "# team-id must be a secret shared only by your team. it's like a password, " << endl
               << "# so leaking it means other people can see your team!" << endl
               << "team-id=SOMETHING_HERE" << endl
               << endl
               << "# your unique username, which must never change" << endl
               << "username=SOMETHING_HERE" << endl
               << endl
               << "# your name as shown to others. you may want to vary this across devices." << endl
               << "displayname=SOMETHING_HERE" << endl;
    }
    iniFile.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto iniFn = QApplication::applicationDirPath() + "/joure.ini";
    auto iniUrl = QUrl::fromLocalFile(iniFn);

    if(!fileExists(iniFn)) {
        createStandardIniFile(iniFn);
        QDesktopServices::openUrl(iniUrl);
        return 1;
    }

    // validate settings file access
    QSettings settings(iniFn, QSettings::IniFormat);
    if(settings.status() != QSettings::NoError) {
        msg("Error opening or parsing joure.ini.\n\nWill now try to launch default text editor so you can try to fix it.");
        QDesktopServices::openUrl(iniUrl);
        return 1;
    }

    // validate settings keys
    for(auto key : QVector<QString>{"main/team-id", "main/username", "main/displayname"} ) {
        if(!settings.contains(key)) {
            msg("Can't find key "+key+" in joure.ini.\n\nWill now try to launch default text editor so you can try to fix it.");
            QDesktopServices::openUrl(iniUrl);
            return 1;
        }
    }

    Team team(
        settings.value("main/team-id").toString(),
        settings.value("main/username").toString(),
        settings.value("main/displayname").toString()
    );

    qDebug() << "hello!";
    team.fetchData();

    MainWindow w(team, iniFn);
    w.show();

    return a.exec();
}

