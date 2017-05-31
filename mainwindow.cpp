#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCameraInfo>
#include <QMessageBox>
#include <QBuffer>
#include <QFile>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tempDir()
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

    //QMessageBox::warning(this, tr("omg!"), tr("size %1 x %2").arg(scaled.width()).arg(scaled.height()));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "JPG");
    QByteArray base64jpg = ba.toBase64();

    upload(QString("egbert"), base64jpg);

//    QImage scaledImage = img.scaled(ui->viewfinder->size(),
//                                    Qt::KeepAspectRatio,
//                                    Qt::SmoothTransformation);

//    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

//    // Display captured image for 4 seconds.
//    displayCapturedImage();
//    QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
}

void MainWindow::upload(QString& name, QByteArray& buffer) {
    QFile file("C:/temp/some_name.html");
    file.open(QIODevice::WriteOnly);
    file.write("<img src=\"data:image/jpeg;base64,");
    file.write(buffer);
    file.write("\"/>");
    file.close();
}

void MainWindow::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);

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
