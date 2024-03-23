#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "opencv2/opencv.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include <FreeImage.h>
#include <QPainter>
#include <QFileDialog>
#include <QIntValidator>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
#include <QVector>
#include <QMessageBox>
#include <QProgressDialog>
#include <QWindow>
#include <QIcon>

// Global Variables
QString importFileDir;
QString exportFileDir;
QString defaultExportFileDir;
QString defaultExportFileType;
QString fileName;
cv::Mat imageMat;
int currImageWidth;
int currImageHeight;
QPalette editablePalette;
QPalette uneditablePalette;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Easy Image File Converter by Alex Thompson");
    QPixmap windowIcon = QPixmap(":/images/image_converter_icon.ico");
    this->setWindowIcon(windowIcon);

    // Setup
    settingsPopUp = new Settings;
    settingsPopUp->loadSettings();

    // For menu buttons
    QAction *settingsAction = ui->menubar->addAction("Quick Export Settings");
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    QAction *aboutAction = ui->menubar->addAction("About");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    // Disable widgets
    ui->lengthBox->setReadOnly(true);
    ui->widthBox->setReadOnly(true);
    ui->applySizeButton->setEnabled(false);
    ui->exportImageButton->setEnabled(false);

    // Grey out boxes not usable
    editablePalette = ui->lengthBox->palette();
    uneditablePalette.setColor(QPalette::Base, Qt::gray);
    uneditablePalette.setColor(QPalette::Text, Qt::darkGray);
    ui->lengthBox->setPalette(uneditablePalette);
    ui->widthBox->setPalette(uneditablePalette);

    // Set restrictions
    ui->lengthBox->setValidator(new QIntValidator);
    ui->widthBox->setValidator(new QIntValidator);
    ui->lengthBox->setMaxLength(4);
    ui->widthBox->setMaxLength(4);
    ui->imageDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setAcceptDrops(true);

    // Set default display image
    QPixmap defaultImage = QPixmap(":/images/no_image_selected.png");
    defaultImage = defaultImage.scaled(ui->imageDisplay->width()-40, ui->imageDisplay->width()-40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->imageDisplay->setPixmap(defaultImage);

    // Tooltips
    //ui->settingsButton->setToolTip("Click to access the settings");
    ui->applySizeButton->setToolTip("Apply the new dimensions to the converted image.");
    ui->selectImageButton->setToolTip("Click to select an image to import.");
    ui->exportImageButton->setToolTip("Click to select a location to export the image.");
}

// Credit to eyllanesc on StackOverflow
static QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        //qDebug() << "1";
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        //qDebug() << "3";
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    // ICO uses this type, so convert to format (not from eyllanesc)
    if (mat.type() == CV_8UC4)
    {
        //qDebug() << "4";
        const uchar* qImageBuffer = mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);

        // If no alpha channel, keep it
        if (mat.channels() == 4)
            return img;

        // Add an alpha channel
        QImage newImg(img.size(), QImage::Format_ARGB32);
        newImg.fill(Qt::white);  // Set alpha channel to 255 (fully opaque)
        QPainter painter(&newImg);
        painter.drawImage(0, 0, img);
        return newImg;
    }

    return QImage();
}

void MainWindow::convertImage()
{
    cv::Mat currentImageMat = cv::imread(importFileDir.toStdString(), -1);
    imageMat = currentImageMat;

    fileName = QString::fromStdString(std::filesystem::path(importFileDir.toStdString()).filename().replace_extension().string());
}

void MainWindow::quickConvertExportImage(QString filePath)
{
    importFileDir = filePath;
    //qDebug() << "Dropped file path: " << filePath;
    //applyImage();
    convertImage();

    std::string fileType = "." + settingsPopUp->getDefaultExportFileType().toLower().toStdString();
    //qDebug() << fileType;

    std::filesystem::path exportPath = std::filesystem::path(importFileDir.toStdString());

    if (settingsPopUp->getDefaultExportLocationEnabled())
    {
        if (exportPath != "")
        {
            exportPath = std::filesystem::path(settingsPopUp->getDefaultExportLocation().toStdString());
            //qDebug() << settingsPopUp->getDefaultExportLocation().toStdString();
        }
    }
    else
    {
        exportPath = exportPath.parent_path().string();
    }

    cv::imwrite(exportPath.string() + "/" + fileName.toStdString() + fileType, imageMat);
}

void MainWindow::applyImage()
{
    cv::Mat currentImageMat = cv::imread(importFileDir.toStdString(), -1);
    imageMat = currentImageMat;

    fileName = QString::fromStdString(std::filesystem::path(importFileDir.toStdString()).filename().replace_extension().string());

    QImage currentImage = MatToQImage(currentImageMat);

    // Keep hold of imported image's width and height
    currImageHeight = currentImage.height();
    currImageWidth = currentImage.width();

    int w = ui->imageDisplay->width();
    int h = ui->imageDisplay->height();

    // Change UI Label to Image
    ui->imageDisplay->setPixmap(QPixmap::fromImage(currentImage).scaled(w, h, Qt::KeepAspectRatio));

    // Enable boxes
    ui->lengthBox->setReadOnly(false);
    ui->widthBox->setReadOnly(false);
    ui->applySizeButton->setEnabled(true);
    ui->exportImageButton->setEnabled(true);
    ui->applySizeButton->setToolTip("Click to apply the changes to the image size");
    ui->lengthBox->setPalette(editablePalette);
    ui->widthBox->setPalette(editablePalette);

    // Put current image size in box
    ui->lengthBox->setText(QString::number(currentImage.height()));
    ui->widthBox->setText(QString::number(currentImage.width()));
}

cv::Mat resizeMaterial(cv::Mat mat, int width, int height)
{
    cv::Mat newMat;

    cv::resize(mat, newMat, cv::Size(width, height));

    return newMat;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QStringList accepted_types;
    accepted_types << "png" << "jpg" << "jpeg" << "webp" << "tiff" << "bmp";

    int amountOfImages = e->mimeData()->urls().size();

    if (amountOfImages != 1)
    {
        int currentProgress = 0;

        QProgressDialog convertProgress;

        convertProgress.setLabelText("Converting " + QString::number(amountOfImages) + " images...");
        convertProgress.setRange(0, amountOfImages);
        convertProgress.open();

        foreach(const QUrl & url, e->mimeData()->urls())
        {
            QString fname = url.toLocalFile();

            QFileInfo info(fname);

            if (info.exists())
            {
                if (accepted_types.contains(info.suffix().trimmed(), Qt::CaseInsensitive))
                {
                    // do stuff
                    QString filePath = url.toLocalFile();

                    quickConvertExportImage(filePath);

                    /*importFileDir = filePath;
                qDebug() << "Dropped file path: " << filePath;
                //applyImage();
                convertImage();

                std::string fileType = ".png";

                std::filesystem::path exportPath = std::filesystem::path(importFileDir.toStdString());

                cv::imwrite(exportPath.parent_path().string() + "/" + fileName.toStdString() + fileType, imageMat);*/
                }
                else
                {
                    //qDebug() << "File " << url << "is not accepted due to unaccepted file type!";
                    QMessageBox msgBox;
                    QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
                    msgBox.setWindowIcon(windowIcon);
                    msgBox.setWindowTitle("File Type Not Accepted");
                    msgBox.setText("File " + url.toLocalFile() + " is not accepted due to unaccepted file type!");
                    msgBox.exec();
                }
            }

            convertProgress.setValue(currentProgress++);
        }

        e->acceptProposedAction();

        QMessageBox msgBox;
        QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
        msgBox.setWindowIcon(windowIcon);
        msgBox.setWindowTitle("Quick Export Complete");
        msgBox.setText("Quick Export completed successfully!");
        msgBox.exec();
    }
    else
    {
        QString fname = e->mimeData()->urls()[0].toLocalFile();

        QFileInfo info(fname);

        if (info.exists())
        {
            if (accepted_types.contains(info.suffix().trimmed(), Qt::CaseInsensitive))
            {
                // do stuff
                importFileDir = fname;
                applyImage();
            }
            else
            {
                //qDebug() << "File " << url << "is not accepted due to unaccepted file type!";
                QMessageBox msgBox;
                QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
                msgBox.setWindowIcon(windowIcon);
                msgBox.setWindowTitle("File Type Not Accepted");
                msgBox.setText("File " + fname + " is not accepted due to unaccepted file type!");
                msgBox.exec();
            }
        }
    }
}

void MainWindow::on_selectImageButton_clicked()
{
    QString importFileDirTemp = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Images (*.png *.jpg *.jpeg *.webp *.tiff *.bmp)"));

    // If no file is selected, do nothing, else, import file and set UI image
    if (importFileDirTemp != "")
    {
        importFileDir = importFileDirTemp;
        applyImage();
        ui->exportImageButton->setEnabled(true);
    }
}


void MainWindow::on_exportImageButton_clicked()
{
    // If no file has been imported, you can't export file
    if (importFileDir != "")
    {
        // Get the file export from the user
        exportFileDir = QFileDialog::getSaveFileName(this, tr("Export File"), fileName, tr("PNG (*.png);;JPG (*.jpg);;WEBP (*.webp);;TIFF (*.tiff);;BMP (*.bmp)"));

        // If no location is selected, do nothing, else, export the image
        if (exportFileDir != "")
        {

            // Resize image
            imageMat = resizeMaterial(imageMat, currImageWidth, currImageHeight);

            // Export
            cv::imwrite(exportFileDir.toStdString(), imageMat);

            QMessageBox msgBox;
            QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
            msgBox.setWindowIcon(windowIcon);
            msgBox.setWindowTitle("Image Export Success");
            msgBox.setText("The converted image has been exported successfully!");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
        msgBox.setWindowIcon(windowIcon);
        msgBox.setWindowTitle("No Image Selected");
        msgBox.setText("No image has been selected yet!");
        msgBox.exec();
    }
}


void MainWindow::on_applySizeButton_clicked()
{
    // Check if any of the input boxes are empty
    if (ui->lengthBox->text() != "" || ui->widthBox->text() != "")
    {
        if (ui->lengthBox->text().toInt() > 0 && ui->widthBox->text().toInt() > 0)
        {
            //imageMat = resizeMaterial(imageMat, ui->lengthBox->text().toInt(), ui->widthBox->text().toInt());

            // Set the global image size (only change on actual Mat when exporting to not constantly resize image)
            currImageWidth = ui->widthBox->text().toInt();
            currImageHeight = ui->lengthBox->text().toInt();

            // Get temp mat to resize to change display image look
            cv::Mat tempMat = imageMat;
            tempMat = resizeMaterial(tempMat, currImageWidth, currImageHeight);

            QImage currentImage = MatToQImage(tempMat);

            int w = ui->imageDisplay->width();
            int h = ui->imageDisplay->height();

            // Change UI Label to Image
            ui->imageDisplay->setPixmap(QPixmap::fromImage(currentImage).scaled(w, h, Qt::KeepAspectRatio));

            QMessageBox msgBox;
            QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
            msgBox.setWindowIcon(windowIcon);
            msgBox.setWindowTitle("New Image Size Success");
            msgBox.setText("Image size applied successfully!");
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox;
            QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
            msgBox.setWindowIcon(windowIcon);
            msgBox.setWindowTitle("Invalid Image Size");
            msgBox.setText("The new size is not a valid number for an image size!");
            msgBox.exec();
        }
    }
}

void MainWindow::startExeConvert(int argsAmount, QVector<QString> fileLocs)
{
    int amountOfImages = argsAmount - 1;

    for (int i = 0; i < amountOfImages; i++)
    {
        quickConvertExportImage(fileLocs[i]);
    }

    QMessageBox msgBox;
    QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
    msgBox.setWindowIcon(windowIcon);
    msgBox.setWindowTitle("Quick Export Success");
    msgBox.setText("Converted " + QLocale().toString(amountOfImages) + " images successfully!");
    msgBox.exec();
}

void MainWindow::showSettings()
{
    settingsPopUp->visuallyResetBoxes();
    settingsPopUp->show();
}

void MainWindow::showAbout()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
    msgBox.setWindowIcon(windowIcon);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("Version: 1.2<br/>Created by: Alex Thompson<br/>Website Link: <a href=\"https://github.com/AlexThomp1/Easy-Image-File-Type-Converter\">GitHub</a>");
    msgBox.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
}

