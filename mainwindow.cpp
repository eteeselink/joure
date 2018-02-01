#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCameraInfo>
#include <QMessageBox>
#include <QBuffer>
#include <QFile>
#include <QTimer>

MainWindow::MainWindow(Team& team, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tempDir(),
    team(team)
{
    ui->setupUi(this);

    camera = new QCamera(QCameraInfo::defaultCamera());
    imageCapture = new QCameraImageCapture(camera);

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(imageCapture, SIGNAL(imageAvailable(int,QVideoFrame)), this, SLOT(imageAvailable(int,QVideoFrame)));
    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this,
            SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    QTimer::singleShot(1000, this, SLOT(capture()));
}

MainWindow::~MainWindow()
{
    delete imageCapture;
    delete camera;
    delete ui;
}

void MainWindow::capture() {

    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->start();
    camera->searchAndLock();

    // can't not save a file (!). see imageSaved for more comments.

    imageCapture->capture(tempDir.filePath("joure.jpg"));
}

void MainWindow::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}

void MainWindow::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);

    QImage scaled = img.scaledToWidth(300, Qt::FastTransformation);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "JPG");
    QByteArray base64jpg = ba.toBase64();

    upload(QString("egbert"), base64jpg);
}

void MainWindow::upload(const QString& name, QByteArray& buffer) {
    QFile file("C:/temp/some_name.html");
    file.open(QIODevice::WriteOnly);
    file.write("<img src=\"data:image/jpeg;base64,");
    file.write(buffer);
    file.write("\"/>");
    file.close();
}

void MainWindow::imageSaved(int id, const QString &fileName)
{
    /* Weird stuff: QCameraImageCaptureThingo insists on saving a file on Windows.
     * I somehow just can't get it to only give me a buffer. So, we let it save a
     * damn file into a temp directory, patiently wait until the file is really saved,
     * and only *then* unload the camera. */
    Q_UNUSED(id);
    Q_UNUSED(fileName);

    camera->unload();
    QTimer::singleShot(4000, this, SLOT(capture()));
    //QMessageBox::warning(this, tr("moo"), fileName);
}


void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
}
