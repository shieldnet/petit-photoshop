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
#include <QtGui>
#include <QTranslator>
#include <QMouseEvent>
#include <QInputDialog>
#include <QSpinBox>
#include <QColorDialog>
#include <QPushButton>
#include <QTextEdit>

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
    void on_actionMosaic_triggered();
    void on_actionEdge_Detection_triggered();

    void    updateLabel();
    void    doGrayScale();
    void    doMosaic(int);  //Window Size
    void    doEdgeDetection(int);   //Threshold
    void    doBrightness(int);
    void    doGaussianBlur(int);
    void    doResize(int, int);
    void    doInverse();
    void    doContrast(int);

    void    draw(const QPoint & pos);
    void    erase(const QPoint & pos);
    void    write(const QPoint & pos);

    void mousePressEvent(QMouseEvent * ev) override {
        if (pen) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            lastPos = tmp;
            draw(tmp);
        } else if (eraser) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            lastPos = tmp;
            erase(tmp);
        }else if (text) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            lastPos = tmp;
            write(tmp);
        }
    }

    void mouseMoveEvent(QMouseEvent * ev) override {
        if (pen) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            draw(tmp);
        } else if (eraser) {
            QPoint tmp = ev->pos();
            tmp.setX(tmp.x() - 146);
            tmp.setY(tmp.y() - 130);
            erase(tmp);
        }
    }

    void on_actionPen_toggled(bool arg1);
    void on_actionEraser_toggled(bool arg1);

    void on_actionPen_triggered();

    void on_actionEraser_triggered();

    void on_actionColorPicker_triggered();

    void on_back_triggered();

    void on_actionbackRedo_triggered();

    void on_actionTextBox_triggered();

    void on_actionBrightness_triggered();

    void on_actionBlur_triggered();

    void on_actionResize_triggered();

    void on_actionInverse_triggered();

    void on_actionContrast_triggered();
private:
    void        savePm();
    void        undo();
    void        redo();
    Ui::MainWindow *ui;
    QPixmap pm;
    QPixmap _lastPm;
    QPixmap _nextPm;
    bool    _redo = false;
    //QFile currentFile;
    QString currentFile;
    QMessageBox messageBox;
    bool    pen = false;
    bool    eraser = false;
    bool    text = false;
    double  penSize = 2.0;
    QColorDialog *palette;
    QSpinBox *spinBoxToolBar;
    QPoint  lastPos;
    QTextEdit *textEdit;
};

#endif // MAINWINDOW_H
