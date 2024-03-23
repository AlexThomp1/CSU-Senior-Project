#include "settings.h"
#include "ui_settings.h"
#include <QDebug>
#include <QSettings>
#include <QApplication>

QString tempDefaultLoc;
QPalette uneditablePaletteSettings;
QPalette editablePaletteSettings;

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    QPixmap windowIcon = QPixmap(":/images/image_converter_icon.ico");
    this->setWindowIcon(windowIcon);

    settingsFile = QApplication::applicationDirPath() + "/settings.ini";

    // Disable boxes
    ui->defaultExportLocBox->setReadOnly(true);
    ui->findExportLocation->setEnabled(false);

    editablePaletteSettings = ui->findExportLocation->palette();
    uneditablePaletteSettings.setColor(QPalette::Base, Qt::gray);
    uneditablePaletteSettings.setColor(QPalette::Text, Qt::darkGray);

    ui->defaultExportLocBox->setPalette(uneditablePaletteSettings);

    ui->settingsLabel->setToolTip("Quick Export allows for users to drag & drop multiple files onto the program to convert at the same time.");
    ui->defaultExportLabel->setToolTip("Choose where the Quick Export will export the converted files automatically to.");
    ui->defaultExportTypeLabel->setToolTip("Choose what image type Quick Export will export the converted files automatically to.");
    ui->enableDefaultExport->setToolTip("If not Enabled, the converted files will automatically export to the same location as the original file.");

}

void Settings::loadSettings()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString defaultExportLocSettings = settings.value("defaultExportLoc", "").toString();
    QString defaultExportTypeSettings = settings.value("defaultExportType", "").toString();
    bool defaultExportLocEnabledSettings = settings.value("defaultExportLocEnabled", "").toBool();

    defaultExportFileLoc = defaultExportLocSettings;
    defaultExportFileType = defaultExportTypeSettings;
    tempDefaultLoc = defaultExportLocSettings;
    defaultExportFileLocEnabled = defaultExportLocEnabledSettings;

    applySettingsLoad();
}

void Settings::saveSettings()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.setValue("defaultExportLoc", defaultExportFileLoc);
    settings.setValue("defaultExportType", defaultExportFileType);
    settings.setValue("defaultExportLocEnabled", defaultExportFileLocEnabled);
}

void Settings::applySettingsLoad()
{
    // Apply text
    ui->defaultExportLocBox->setText(defaultExportFileLoc);
    ui->enableDefaultExport->setChecked(defaultExportFileLocEnabled);

    // If no settings exist, default to PNG
    if (defaultExportFileType == "")
    {
        ui->fileTypeBox->setCurrentIndex(0);
        defaultExportFileType = ui->fileTypeBox->currentText();
    }
    else
    {
        ui->fileTypeBox->setCurrentIndex(ui->fileTypeBox->findText(defaultExportFileType));
    }

    // Apply disabled
    if (!ui->enableDefaultExport->isEnabled())
    {
        ui->defaultExportLocBox->setPalette(uneditablePaletteSettings);
    }
}

void Settings::on_findExportLocation_clicked()
{
    tempDefaultLoc = QFileDialog::getExistingDirectory(this, tr("Select Default Export Location"), "/");

    // If no file is selected, do nothing, else, import file and set UI image
    if (tempDefaultLoc != "")
    {
        ui->defaultExportLocBox->setText(tempDefaultLoc);
    }
}


void Settings::on_settingsAcceptChanges_clicked()
{
    defaultExportFileType = ui->fileTypeBox->currentText();
    defaultExportFileLoc = tempDefaultLoc;
    defaultExportFileLocEnabled = ui->enableDefaultExport->isChecked();

    //qDebug() << defaultExportFileLocEnabled;

    saveSettings();

    QMessageBox msgBox;
    QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
    msgBox.setWindowIcon(windowIcon);
    msgBox.setWindowTitle("Settings Apply Success");
    msgBox.setText("Changes applied successfully!");
    msgBox.exec();
}

void Settings::on_enableDefaultExport_stateChanged(int arg1)
{
    if (arg1 != 0)
    {
        ui->defaultExportLocBox->setPalette(editablePaletteSettings);
        ui->findExportLocation->setEnabled(true);
    }
    else
    {
        ui->defaultExportLocBox->setPalette(uneditablePaletteSettings);
        ui->findExportLocation->setEnabled(false);
    }
}

void Settings::visuallyResetBoxes()
{
    // Visually reset the boxes if user closed out of the box when selected w/o hitting apply
    ui->defaultExportLocBox->setText(defaultExportFileLoc);
    ui->fileTypeBox->setCurrentIndex(ui->fileTypeBox->findText(defaultExportFileType));
    ui->enableDefaultExport->setChecked(defaultExportFileLocEnabled);
}

QString Settings::getDefaultExportLocation()
{
    return defaultExportFileLoc;
}

QString Settings::getDefaultExportFileType()
{
    return defaultExportFileType;
}

bool Settings::getDefaultExportLocationEnabled()
{
    return defaultExportFileLocEnabled;
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_quickExportInfoButton_clicked()
{
    QMessageBox msgBox;
    QIcon windowIcon = QPixmap(":/images/image_converter_icon.ico");
    msgBox.setWindowIcon(windowIcon);
    msgBox.setWindowTitle("What is Quick Export?");
    msgBox.setText("Quick Export allows the user to mass convert & export multiple image files.\n\nIf more than 1 file is drag & dropped on the program, they will convert to the file location and file type selected in the settings.\n\nIf the Default Export Location is not enabled, the image will export in the same location as the original image.");
    msgBox.exec();
}

