#include "painter.h"

void Painter::draw(const QPoint & pos) {
        QPainter painter{&m_pixmap};
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen({Qt::blue, 2.0});
        painter.drawLine(m_lastPos, pos);
        m_lastPos = pos;
        update();
}

void    Painter::setPixmap(QPixmap &pm){
    m_pixmap = pm;
    std::cout<< "lolol";
}

void    Painter::setLabel(QLabel *label){
    m_label = label;
}

void    Painter::setPen(bool &pen) {
    m_pen = pen;
}
