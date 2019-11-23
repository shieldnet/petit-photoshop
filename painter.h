#ifndef PAINTER_H
#define PAINTER_H

#include <QtWidgets>
#include <iostream>

class Painter : public QWidget {
    public:
    using QWidget::QWidget;
        void paintEvent(QPaintEvent *) override {
            QPainter painter{this};
            painter.drawPixmap(0, 0, m_pixmap);
        }
        void mousePressEvent(QMouseEvent * ev) override {
            std::cout << "zebi is PRESSED" << std::endl;
            if (m_pen) {
                std::cout << "pen is good" << std::endl;
                m_lastPos = ev->pos();
                draw(ev->pos());
            }
        }
        void mouseMoveEvent(QMouseEvent * ev) override {
            if (m_pen) {
                draw(ev->pos());
            }
        }
        void    setPixmap(QPixmap &pm);
        void    setLabel(QLabel *label);
        void    setPen(bool &pen);
        void    draw(const QPoint & pos);
    private:
        QPixmap m_pixmap;
        QPoint m_lastPos;
        QLabel *m_label;
        bool   m_pen;
};

#endif // PAINTER_H
