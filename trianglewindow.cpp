#include "trianglewindow.h"

TriangleWindow::TriangleWindow(quint16 port)
{
    QString s ("FPS : ");
    s += QString::number(1000/maj);
    s += "(";
    s += QString::number(maj);
    s += ")";
    setTitle(s);

    timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()),this, SLOT(renderNow()));
    timer->start(maj);
    master = true;

    allSeasons = new QString[4];
    allSeasons[0] = "SUMMER";
    allSeasons[1] = "AUTUMN";
    allSeasons[2] = "WINTER";
    allSeasons[3] = "SPRING";

    currentSeason = 0;

    connectToServer(port);
}
TriangleWindow::TriangleWindow(int _maj, quint16 port)
{

    maj = _maj;
    QString s ("FPS : ");
    s += QString::number(1000/maj);
    s += "(";
    s += QString::number(maj);
    s += ")";
    setTitle(s);

    timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()),this, SLOT(renderNow()));
    timer->start(maj);

    timerParticule = new QTimer();
    timerParticule->start(200);

    particules = new Particule*[MAX_PARTICULES];

    allSeasons = new QString[4];
    allSeasons[0] = "SUMMER";
    allSeasons[1] = "AUTUMN";
    allSeasons[2] = "WINTER";
    allSeasons[3] = "SPRING";

    currentSeason = 0;

    light = false;

    connectToServer(port);
}

void facettisationSphere(float rayon, int nbMeridien, int nbParallele, Point centre)
{
  for(int i = 0; i <= nbParallele; i++)
  {
    double phy = PI * ((double) i / nbParallele);
    glBegin(GL_LINE_STRIP);
    for(int j = 0; j <= nbMeridien; j++)
    {
      double tetha = 2 * PI * ((double) j / nbMeridien);
      Point tmp;
      tmp.x = rayon * sin(phy) * cos(tetha);
      tmp.y = rayon * sin(phy) * sin(tetha);
      tmp.z = rayon * cos(phy);
      glVertex3f(tmp.x + centre.x, tmp.y + centre.y, tmp.z + centre.z);
    }
    glEnd();
  }
}

void TriangleWindow::connectToServer(quint16 port)
{
    client = new QTcpSocket();
    connect(client, SIGNAL(connected()),this, SLOT(connected()));
    connect(client, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(client, SIGNAL(readyRead()),this, SLOT(readyRead()));
    client->connectToHost("localhost", port);

    if(!client->waitForConnected(5000))
    {
        qDebug() << "Impossible de se connecter";
    }
}

void TriangleWindow::connected()
{
    qDebug() << "un client vient de se connecter";
}

void TriangleWindow::disconnected()
{
    qDebug() << "un client vient de se déconnecter";
}

void TriangleWindow::readyRead()
{
    //read something
    QString message = client->readAll();
    if(message == "nextSeason")
    {
        currentSeason++;
        currentSeason %= 4;
    }
    else
        currentSeason = message.toInt();
}

void TriangleWindow::initialize()
{
    const qreal retinaScale = devicePixelRatio();


    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0);


    loadMap(":/heightmap-2.png");

    initParticules();
}

void TriangleWindow::loadMap(QString localPath)
{

    if (QFile::exists(localPath)) {
        m_image = QImage(localPath);
    }

    uint id = 0;
    p = new Point[m_image.width() * m_image.height()];

    QRgb pixel;
    for(int i = 0; i < m_image.width(); i++)
    {
        for(int j = 0; j < m_image.height(); j++)
        {
            pixel = m_image.pixel(i,j);

            id = i*m_image.width() +j;

            p[id].x = (float)i/(m_image.width()) - ((float)m_image.width()/2.0)/m_image.width();
            p[id].y = (float)j/(m_image.height()) - ((float)m_image.height()/2.0)/m_image.height();
            p[id].z = 0.001f * (float)(qRed(pixel));
        }
    }
}

void TriangleWindow::initParticules()
{
    for(int k = 0; k < MAX_PARTICULES; k++)
    {
        int i = rand() % m_image.width();
        int j = rand() % m_image.height();

        uint id = i*m_image.width() +j;
        particules[k] = new Particule(p[id].x, p[id].y, p[id].z, timerParticule);
    }
}

void TriangleWindow::render()
{

    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat LightAmbient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat LightAmbientSummer[] = { 0.6f, 0.6f, 0.3f, 1.0f };
    GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat LightPosition[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

    if(allSeasons[currentSeason] == "SUMMER")
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientSummer);
    }

    glEnable( GL_LIGHTING );
    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);

    glLoadIdentity();
    glScalef(c->ss,c->ss,c->ss);

    glRotatef(c->rotX,1.0f,0.0f,0.0f);
    if(c->anim == 0.0f)
    {
        glRotatef(c->rotY,0.0f,0.0f,1.0f);
    }
    else
    {
        glRotatef(c->anim,0.0f,0.0f,1.0f);
        if(master)
            c->anim +=0.05f;
    }

//    if( light )
//      glEnable( GL_LIGHTING );
//    else
//      glDisable( GL_LIGHTING );

    switch(c->etat)
    {
    case 0:
        displayTrianglesTexture();
        break;
    case 1:
        displayLines();
        break;
    case 2:
        displayPoints();
        break;
    case 3:
        displayTrianglesC();
        break;
    case 4:
        displayTrianglesTexture();
        break;
    case 5:
        displayTriangles();
        displayLines();
        break;
    default:
        displayPoints();
        break;
    }

    drawParticules();
    Point centre;
    centre.x = -0.6;
    centre.y = -0.4;
    centre.z = 0.6;
    glColor3f(1, 1, 0);
    if(allSeasons[currentSeason] == "SUMMER")
        facettisationSphere(0.1, 30, 30, centre);
    ++m_frame;
}

bool TriangleWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void TriangleWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
            close();
            break;
    case 'C':
        if(c->anim == 0.0f)
            c->anim = c->rotY;
        else
            c->anim = 0.0f;
        break;
    case 'Z':
        c->ss += 0.10f;
        break;
    case 'S':
        c->ss -= 0.10f;
        break;
    case 'A':
        c->rotX += 1.0f;
        break;
    case 'E':
        c->rotX -= 1.0f;
        break;
    case 'Q':
        c->rotY += 1.0f;
        break;
    case 'D':
        c->rotY -= 1.0f;
        break;
    case 'W':
        c->etat ++;
        if(c->etat > 5)
            c->etat = 0;
        break;
    case 'P':
        maj++;
        timer->stop();
        timer->start(maj);
        break;
    case 'O':
        maj--;
        if(maj < 1)
            maj = 1;
        timer->stop();
        timer->start(maj);
        break;
    case Qt::Key_L:
        light = !light;
        break;
    case 'X':
        carte ++;
        if(carte > 3)
            carte = 1;
        QString depth (":/heightmap-");
        depth += QString::number(carte) ;
        depth += ".png" ;

        loadMap(depth);
        break;
    }
    QString s ("FPS : ");
    s += QString::number(1000/maj);
    s += "(";
    s += QString::number(maj);
    s += ")";
    setTitle(s);
}

void TriangleWindow::getNormal(Point O, Point X, Point Y)
{
    // vecteur OX
    Point vx;
    vx.x=X.x-O.x;
    vx.y=X.y-O.y;
    vx.z=X.z-O.z;
    // norme de OX
    float length=sqrt(vx.x*vx.x+vx.y*vx.y+vx.z*vx.z);
    // normalisation
    vx.x/=length;
    vx.y/=length;
    vx.z/=length;

    // vecteur OY
    Point vy;
    vy.x=Y.x-O.x;
    vy.y=Y.y-O.y;
    vy.z=Y.z-O.z;
    // norme de OX
    length=sqrt(vy.x*vy.x+vy.y*vy.y+vy.z*vy.z);
    // normalisation
    vy.x/=length;
    vy.y/=length;
    vy.z/=length;

    // Calcul de la normale
    Point normal;
    normal.x=vx.y*vy.z-vy.y*vx.z;
    normal.y=vx.z*vy.x-vx.x*vy.z;
    normal.z=vx.x*vy.y-vx.y*vy.x;

    glNormal3f(normal.x,normal.y,normal.z);
}

void TriangleWindow::drawParticules()
{

    glColor3f(1, 0, 0.8);

    glBegin(GL_POINT);
    glVertex3f(-0.19f, 0.09f, 0.4f);
    glEnd();
    if(allSeasons[currentSeason] == "WINTER")
    {
        glColor3f(1, 1, 1);
        glPointSize(4.0f);
    }
    else if(allSeasons[currentSeason] == "AUTUMN")
    {
        glColor3f(0, 0, 1);
        glPointSize(2.0f);
    }
    else
        return;
    glBegin(GL_POINTS);
    for(int i = 0; i < MAX_PARTICULES; i++)
    {
        glVertex3f(particules[i]->getX(), particules[i]->getY(), particules[i]->getZ());
    }
    glEnd();
}

void TriangleWindow::displayPoints()
{
    glBegin(GL_POINTS);
    uint id = 0;
    for(int i = 0; i < m_image.width(); i++)
    {
        for(int j = 0; j < m_image.height(); j++)
        {
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

        }
    }
    glEnd();
}


void TriangleWindow::displayTriangles()
{
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);



            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }

    glEnd();
}

void TriangleWindow::displayTrianglesC()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {
            glColor3f(0.0f, 1.0f, 0.0f);
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);


            glColor3f(1.0f, 1.0f, 1.0f);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }
    glEnd();
}


void TriangleWindow::displayLines()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +(j);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }

    glEnd();
}

void TriangleWindow::displayTrianglesTexture()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            displayColor(p[id].z);
            getNormal(p[id],p[i*m_image.width() +(j+1)],p[(i+1)*m_image.width() +j]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            displayColor(p[id].z);
            getNormal(p[id],p[(i+1)*m_image.width() +j],p[i*m_image.width() +j]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            displayColor(p[id].z);
            getNormal(p[id],p[i*m_image.width() +j],p[i*m_image.width() +(j+1)]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);



            id = i*m_image.width() +(j+1);
            displayColor(p[id].z);
            getNormal(p[id],p[(i+1)*m_image.width() +j+1],p[(i+1)*m_image.width() +j]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            displayColor(p[id].z);
            getNormal(p[id],p[(i+1)*m_image.width() +j],p[i*m_image.width() +(j+1)]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            displayColor(p[id].z);
            getNormal(p[id],p[i*m_image.width() +(j+1)],p[i*m_image.width() +(j+1)]);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }
    glEnd();
}


void TriangleWindow::displayColor(float alt)
{
    if (alt > 0.2)
    {
        glColor3f(01.0f, 1.0f, 1.0f);
    }
    else if (alt > 0.08)
    {
        glColor3f(0.6f, 0.4f, 0.1f);
    }
    else if (alt > 0.03f)
    {
        glColor3f(0.2, 0.5f, 0.14f);
    }
    else
    {
        glColor3f(0.0f, 0.0f, 1.0f);
    }

}
