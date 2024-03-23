#include "mainwindow.h"

#include <QApplication>
#include <QVector>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(700, 590);
    w.show();

    // To drag onto .exe to import files
    // argc is size of arguments (1 is always program name)
    // argv is passed in arguments (index 0 is the program name)
    if (argc > 1)
    {
        QVector<QString> fileLocs;
        for (int i = 1; i < argc; i++)
        {
            fileLocs.push_back(argv[i]);
        }
        w.startExeConvert(argc, fileLocs);
    }

    return a.exec();
}
