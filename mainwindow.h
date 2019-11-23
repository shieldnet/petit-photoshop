#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QFile>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

enum IMAGE_SIZE {
    PORTRAIT,
    DIAPORAMA,
    SQUARE,
    //CUSTOM,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void    openImage(const QString &filename);
    void    openNewImage(const IMAGE_SIZE &format);
    void    setLabelImage(const QString &filename);
    void    setLabelToNew(const IMAGE_SIZE &format);
    void    errorMessage(const QString &error);

private slots:
    void on_label_image_linkActivated(const QString &link);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionNew_triggered();

    void on_actionPortrait_triggered();
    void on_actionDiaporama_triggered();
    void on_actionSquare_triggered();
    void on_actionBlack_White_triggered();

    void    updateLabel();
    void    doGrayScale();


    void    draw(const QPoint & pos);

    void mousePressEvent(QMouseEvent * ev) override {
        if (pen) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            lastPos = tmp;
            draw(tmp);
        }
    }

    void mouseMoveEvent(QMouseEvent * ev) override {
        if (pen) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            draw(tmp);
        }
    }

    void on_actionPen_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    QPixmap pm;
    //QFile currentFile;
    QString currentFile;
    QMessageBox messageBox;
    bool    pen = false;
    QPoint  lastPos;
};

#endif // MAINWINDOW_H
