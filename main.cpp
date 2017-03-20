#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QRectF>
#include <vector>
#include <list>
#include <string>

#include "face_tree.h"
#include "geometry.hpp"

using namespace std;

class Line{

public:
    double x_start, y_start, x_end, y_end;

    Line(double x_start, double y_start, double x_end, double y_end){
        this->x_start = x_start;
        this->y_start = y_start;
        this->x_end = x_end;
        this->y_end = y_end;
    }
};


class PlotArea : public QWidget{
private:
    double x_min, y_min, x_max, y_max;
    int plot_size_hight, plot_size_width;
    vector<Line> lines;

public:
    PlotArea(QWidget *parent, int plot_size_space, int plot_size_width, int plot_size_hight) : QWidget(parent) {

        this->setGeometry(plot_size_space, plot_size_space, plot_size_width, plot_size_hight);
        this->setStyleSheet("background-color:white;");

        this->plot_size_width = plot_size_width;
        this->plot_size_hight = plot_size_hight;

        this->x_min = 0.;
        this->y_min = 0.;
        this->x_max = 10.0;
        this->y_max = 10.0;
    }

    void set_extents(double x_min, double y_min, double x_max, double y_max){
        this->x_min = x_min;
        this->y_min = y_min;
        this->x_max = x_max;
        this->y_max = y_max;
    }

    void draw_lines(vector<double> x, vector<double> y){
        vector<double> :: iterator xi, yi;

        double x_begin_pixels, y_begin_pixels, x_end_pixels, y_end_pixels;
        double x_begin, y_begin;

        for (xi=x.begin(), yi=y.begin(); xi<x.end(), yi<y.end(); ++xi, ++yi){

            if (xi!=x.begin()){

                // x_begin = x_min -> x_begin_pixels = 0
                // x_begin = x_max -> x_begin_pixels = plot_size_width

                x_begin_pixels = (x_begin - this->x_min) / (this->x_max - this->x_min) * this->plot_size_width;
                x_end_pixels = (*xi - this->x_min) / (this->x_max - this->x_min) * this->plot_size_width;
                y_begin_pixels = (y_begin - this->y_min) / (this->y_max - this->y_min) * this->plot_size_hight;
                y_end_pixels =  (*yi - this->y_min) / (this->y_max - this->y_min) * this->plot_size_hight;

                Line l(x_begin_pixels, y_begin_pixels, x_end_pixels, y_end_pixels);
                this->lines.push_back(l);
            }

            x_begin = *xi;
            y_begin = *yi;
        }
    }

    void draw_shape(vector<vector<double> > xy){

        vector<vector<double> > :: iterator ixy;
        double x_begin_pixels, y_begin_pixels, x_end_pixels, y_end_pixels;
        double x_begin, y_begin;

        xy.push_back(xy[0]);

        for(ixy=xy.begin(); ixy<xy.end(); ++ixy){

            if (ixy!=xy.begin()){

                x_begin_pixels = (x_begin - this->x_min) / (this->x_max - this->x_min) * this->plot_size_width;
                x_end_pixels = ((*ixy)[0] - this->x_min) / (this->x_max - this->x_min) * this->plot_size_width;
                y_begin_pixels = (y_begin - this->y_min) / (this->y_max - this->y_min) * this->plot_size_hight;
                y_end_pixels =  ((*ixy)[1] - this->y_min) / (this->y_max - this->y_min) * this->plot_size_hight;

                Line l(x_begin_pixels, y_begin_pixels, x_end_pixels, y_end_pixels);
                this->lines.push_back(l);
            }
            x_begin = (*ixy)[0];
            y_begin = (*ixy)[1];
        }
    }

protected:
    void paintEvent(QPaintEvent *){
        QPainter painter(this);
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));

        vector<Line>:: iterator iline;
        for(iline=this->lines.begin(); iline<this->lines.end(); ++iline){
            painter.drawLine(iline->x_start, iline->y_start, iline->x_end, iline->y_end);
        }
    }
};

class SimplePlot : public QWidget
{
private:
    PlotArea *plot_area;
    QLabel *label_x_min, *label_x_max, *label_y_min, *label_y_max;

public:
    SimplePlot(int window_size_width, int window_size_hight){

        int plot_size_width = window_size_width / 10 * 9;
        int plot_size_hight = window_size_hight / 10 * 9;
        int plot_size_space = window_size_hight / 20;

        plot_area = new PlotArea(this, plot_size_space, plot_size_width, plot_size_hight);

        label_y_max = new QLabel(this);
        label_y_max->setGeometry(0, 0, plot_size_space, plot_size_space);

        label_y_min = new QLabel(this);
        label_y_min->setGeometry(0, plot_size_hight, plot_size_space, plot_size_space);

        label_x_min = new QLabel(this);
        label_x_min->setGeometry(plot_size_space, plot_size_hight + plot_size_space, plot_size_space, plot_size_space);

        label_x_max = new QLabel(this);
        label_x_max->setGeometry(plot_size_space + plot_size_width, plot_size_hight + plot_size_space, plot_size_space, plot_size_space);

        this->setGeometry(0, 0, window_size_width, window_size_hight);
        this->setStyleSheet("background-color:gray;");
    }

    ~SimplePlot(){
        delete plot_area;
    }

    void set_extents(double x_min, double y_min, double x_max, double y_max){
        this->plot_area->set_extents(x_min, y_min, x_max, y_max);
        this->label_y_max->setText(QString::number(y_max));
        this->label_y_min->setText(QString::number(y_min));
        this->label_x_max->setText(QString::number(x_max));
        this->label_x_min->setText(QString::number(x_min));

    }

    void draw_lines(vector<double> x, vector<double> y){

        plot_area->draw_lines(x, y);
        plot_area->repaint();
    }

    void draw_shape(vector<vector<double> > xy){
        plot_area->draw_shape(xy);
        plot_area->repaint();
    }
};

int main(int argc, char *argv[])
{

    list<Vertex*> root_vertices;

    Vertex v1(2, 0.0, 0.0);
    root_vertices.push_back(&v1);

    Vertex v2(2, 3.0, 0.0);
    root_vertices.push_back(&v2);

    Vertex v3(2, 0.0, 3.0);
    root_vertices.push_back(&v3);

    VertexTree tree(root_vertices);

    Vertex v_body1(2, 1.0, 1.0);
    tree.add_vertex(v_body1);

    Vertex v_body2(2, 1.0, 0.5);
    tree.add_vertex(v_body2);

    Vertex v_body3(2, 0.75, 0.6);
    tree.add_vertex(v_body3);

    QApplication app(argc, argv);

    SimplePlot w(500, 500);
    w.set_extents(-5, -5, 5, 5);


    auto leaves = tree.get_leaves();
    int count=0;

    for (auto leaf=leaves.begin(); leaf!=leaves.end(); ++leaf, count++){
        vector<vector<double> > coordinates = (*leaf)->coordinates();
        w.draw_shape(coordinates);

    }
    w.show();
    return app.exec();
}
