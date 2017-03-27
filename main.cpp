#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QRectF>
#include <QtWidgets>
#include <vector>
#include <list>
#include <string>
#include <cmath>

#include "face_tree.h"
#include "geometry.hpp"

using namespace std;

class PlotArea : public QWidget{

private:
    vector<QPoint> qvertices;
    QLabel *mouse_pos_label;
    vector<vector<vector<double> > > edges;
    bool recalc_edges=true;

public:
    PlotArea(){
        this->setGeometry(0, 0, 500, 500);
        this->setStyleSheet("background-color:white;");
        this->mouse_pos_label = new QLabel(this);
        this->mouse_pos_label->setGeometry(450, 450, 50, 50);
        this->mouse_pos_label->setStyleSheet("background-color:gray;");
        this->setMouseTracking(true);
    }

protected:
    void paintEvent(QPaintEvent *){
        QPainter dot_painter(this);
        QPen pen(QPen(Qt::black, 10, Qt::SolidLine, Qt::RoundCap));
        dot_painter.setPen(pen);

        list<Vertex> vertices;
        vector<QPoint> :: iterator v;
        for(v=this->qvertices.begin(); v<this->qvertices.end(); ++v){
            dot_painter.drawPoint(*v);

            double x = double(v->x());
            double y = double(v->y());

            vertices.push_back(Vertex(2, x, y));
        }

        pen.setWidth(2);
        dot_painter.setPen(pen);

        if (vertices.size() >= 3){

            if (this->recalc_edges){
                VertexTree tree(vertices);
                this->edges = tree.get_edges();
            }

            for (auto coordinates=edges.begin(); coordinates!=edges.end(); ++coordinates){

                double x1 = (*coordinates)[0][0];
                double y1 = (*coordinates)[0][1];
                double x2 = (*coordinates)[1][0];
                double y2 = (*coordinates)[1][1];
                dot_painter.drawLine(x1,y1,x2,y2);
            }
        }
        this->recalc_edges = false;
    }

    void mousePressEvent(QMouseEvent* ev){
        const QPoint p = ev->pos();
        this->qvertices.push_back(p);
        this->recalc_edges = true;
        this->repaint();
    }

    void mouseMoveEvent(QMouseEvent* ev){
        const QPoint p = ev->pos();
        int x = p.x();
        int y = p.y();
        QString coord = QString::number(x) + QString(",") + QString::number(y);
        this->mouse_pos_label->setText(coord);
    }

};


int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    PlotArea plt;
    plt.show();
    return app.exec();
}
