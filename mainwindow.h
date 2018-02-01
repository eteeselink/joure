#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraImageCapture>
#include <QTemporaryDir>
#include "team.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Team& team, QString iniFn, QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void setCamera(const QCameraInfo &cameraInfo);
    void displayCameraError();
    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);
    void processCapturedImage(int requestId, const QImage &img);
    void imageSaved(int id, const QString &fileName);
    void capture();

    void on_actionEdit_settings_triggered();

private:
    Team& team;
    QString iniFn;
    Ui::MainWindow *ui;

    QCamera *camera;
    QCameraImageCapture *imageCapture;

    void upload(const QString& name, QByteArray& buffer);

    QTemporaryDir tempDir;
};

#endif // MAINWINDOW_H
