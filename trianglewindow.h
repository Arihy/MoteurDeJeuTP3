#ifndef TRIANGLEWINDOW_H
#define TRIANGLEWINDOW_H
#include <QtGui/QGuiApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include "openglwindow.h"
#include "camera.h"
#include "point.h"

#include <time.h>

#include <QTcpSocket>
#include <QTimer>

class TriangleWindow : public OpenGLWindow
{
    Q_OBJECT
public:
    TriangleWindow(quint16 port);
    TriangleWindow(int maj, quint16 port);
    void initialize();
    void render();
    bool event(QEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void displayTriangles();
    void displayLines();
    void displayTrianglesC();
    void displayPoints();
    void displayTrianglesTexture();

    void displayColor(float);

    void loadMap(QString localPath);
    void connectToServer(quint16 port);
    Camera* c;

    QTcpSocket *client;

private:
    bool master = false;
    int m_frame;
    QImage m_image;
    Point *p;


    int carte=1;


    int maj = 20;

    QTimer *timer;

public slots:
    void connected();
    void disconnected();
    void readyRead();

};

#endif // TRIANGLEWINDOW_H
