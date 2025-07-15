#include <QApplication>
#include "videowindow.h"
#include "mainwindow.h"

int main(int argc ,char *argv[]) {
    QApplication a(argc, argv);

    VideoWindow videoWindow;
    MainWindow mainWindow;

    QObject::connect(&videoWindow, &VideoWindow::videoFinished, [&]() {
        videoWindow.hide();
        mainWindow.show();
    });

    videoWindow.playIntroVideo();
    videoWindow.show();

    return a.exec();
}
