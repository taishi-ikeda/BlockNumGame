#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    srand(time(nullptr)); // 乱数の初期化

    MainWindow game;
    game.setWindowTitle("Puyo Puyo Prototype");
    game.show();

    return app.exec();
}
