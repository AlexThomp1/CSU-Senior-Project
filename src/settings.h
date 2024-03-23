#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
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

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    QString getDefaultExportLocation();

    QString getDefaultExportFileType();

    void visuallyResetBoxes();

    bool getDefaultExportLocationEnabled();

    void loadSettings();

    void saveSettings();

    void applySettingsLoad();

private slots:
    void on_findExportLocation_clicked();

    void on_settingsAcceptChanges_clicked();

    void on_enableDefaultExport_stateChanged(int arg1);

    void on_quickExportInfoButton_clicked();

private:
    Ui::Settings *ui;
    QString defaultExportFileLoc = "";
    QString defaultExportFileType;
    bool defaultExportFileLocEnabled = false;
    QString settingsFile;
};

#endif // SETTINGS_H
