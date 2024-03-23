#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyImage();

    void convertImage();

    void quickConvertExportImage(QString filePath);

    void setDefaultExportLocation(QString loc);

    void setDefaultExportType(QString type);

    void startExeConvert(int argsAmount, QVector<QString> fileLocs);

private slots:
    void on_selectImageButton_clicked();

    void on_exportImageButton_clicked();

    void on_applySizeButton_clicked();

    void showSettings();

    void showAbout();

private:
    Ui::MainWindow *ui;
    Settings *settingsPopUp;

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

};
#endif // MAINWINDOW_H
