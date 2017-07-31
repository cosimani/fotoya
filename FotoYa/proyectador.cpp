#include "proyectador.h"
#include "ui_proyectador.h"
#include "adminDB.h"
#include <QDebug>

Proyectador::Proyectador(QWidget *parent) : QWidget(parent), ui(new Ui::Proyectador), indexMostrandoFotos(-1),
                                            cantidadFotos(0)  {
    ui->setupUi(this);

    timerActualizarFoto = new QTimer(this);
    timerActualizarFoto->setInterval(2000);  // Publica una nueva foto en el proyector cada 1 segundo

    timerGetListadoFotos = new QTimer(this);
    timerGetListadoFotos->setInterval(10000);  // Lee la base en busca de fotos nuevas. Lo hace cada 10 segundos

    connect(timerActualizarFoto, SIGNAL(timeout()), this, SLOT(slot_timerActualizarFoto()));
    connect(timerGetListadoFotos, SIGNAL(timeout()), this, SLOT(slot_timerGetListadoFotos()));
}

Proyectador::~Proyectador()  {
    delete ui;
}

void Proyectador::iniciar()  {
    slot_timerGetListadoFotos();  // Leemos primero las fotos
    timerActualizarFoto->start();
    timerGetListadoFotos->start();
}

void Proyectador::detener()  {
    timerActualizarFoto->stop();
    timerGetListadoFotos->stop();
}

void Proyectador::setHashtag(const QString &value)  {
    hashtag = value;
}


void Proyectador::paintEvent(QPaintEvent *)  {
    QPainter painter(this);
    painter.drawImage(this->width()/2-im.width()/2, this->height()/2-im.height()/2, im);
}

void Proyectador::slot_timerActualizarFoto()  {
    if (this->hashtag.isEmpty())  // No hay un hashtag seteado, retornamos.
        return;

    // Retorna si es la primera vez que entra aca o si volvio a cero
    if (indexMostrandoFotos < 0)  {
        indexMostrandoFotos = fotos.size();
    }

    indexMostrandoFotos--;

    if (indexMostrandoFotos == -1)
        return;

//    im.load("../fotos/" + this->hashtag + "_imagenes/" + fotos.at(indexMostrandoFotos));  // Con plantilla
    im.load("../fotos/" + this->hashtag + "/" + fotos.at(indexMostrandoFotos));  // Sin plantilla

    this->repaint();
}

void Proyectador::slot_timerGetListadoFotos()  {
    fotos = AdminDB::getInstance()->getFotos(this->hashtag);

    // Si son iguales es que no hay fotos nuevas, entonces no volvemos a reiniciar el index. Esto para que se muestren todas
    // las fotos, salvo llegue alguna nueva, entonces si, hacemos que se muestre.
    if ( cantidadFotos != fotos.size() )  {
        cantidadFotos = fotos.size();
        // Vamos de atras para adelante para que se muestren lo antes posible las ultimas fotos que publicaron
        indexMostrandoFotos = fotos.size();
        qDebug() << "Clase Proyector - Se encontraron" << indexMostrandoFotos;
    }
}


