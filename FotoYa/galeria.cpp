#include "galeria.h"
#include "ui_galeria.h"
#include <QPainter>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QPrinter>
#include <QFileInfo>
#include <QRectF>
#include <QBrush>
#include <QColor>
#include "adminDB.h"

Galeria::Galeria(QWidget *parent) : QWidget(parent), ui(new Ui::Galeria)  {
    ui->setupUi(this);

//    this->resize(parent->width()/2, parent->height());
    this->setFixedWidth(parent->width()/2);

    imageOriginal.load("../recursos/twitter_logo.png");

    connect(ui->pbImprimir, SIGNAL(clicked(bool)), this, SLOT(slot_imprimir()));
    connect(ui->pbImprimir10x15, SIGNAL(clicked(bool)), this, SLOT(slot_imprimir10x15()));
    connect(ui->pbImprimir15x21, SIGNAL(clicked(bool)), this, SLOT(slot_imprimir15x21()));
}

Galeria::~Galeria()  {
    delete ui;
}

QString Galeria::getSizeHoja() const
{
    return sizeHoja;
}

void Galeria::setSizeHoja(const QString &value)
{
    qDebug() << "Se cargo tamano hoja en Galeria. Se invoco a setSizeHoja:" << value;
    sizeHoja = value;
}
QString Galeria::getFilePlantilla() const
{
    return filePlantilla;
}

void Galeria::setFilePlantilla(const QString &value)
{
    qDebug() << "Se cargo la plantilla en Galeria. Se invoco a setFilePlantilla:" << value;
    filePlantilla = value;
}


void Galeria::paintEvent(QPaintEvent *)  {
    QPainter painter(this);

    QImage image = imageOriginal.scaled(this->width(), this->height());
    painter.drawImage(0, 0, image);
}

void Galeria::slot_fotoEnGaleria(QString archivoImagen)  {
    this->archivoImagen = archivoImagen;  // Por si se desea imprimir de nuevo, para mantener cual archivo es.
    imageOriginal.load(archivoImagen);
    this->repaint();
}

void Galeria::slot_fotoOriginalEnGaleria(QString archivoImagenOriginal)  {
    this->archivoImagenOriginal = archivoImagenOriginal;
}


void Galeria::slot_imprimir()  {

    // Si esta vacio es porque no hay foto para imprimir, o la carpeta esta vacia o se
    // imprimieron todas, o inconsistencia
    if ( archivoImagen.isEmpty() )
        return;

    QPrinter printer(QPrinter::ScreenResolution);

    if ( this->sizeHoja == "15x10" )
        printer.setPageSize(QPrinter::A6);  // Es la de 15x10
    else
        printer.setPageSize(QPrinter::A5);  // Es la de 21x15

    printer.setOrientation(QPrinter::Portrait);
    printer.setFullPage(false);

    // Desde galeria solo se puede mandar a la impresora
    printer.setOutputFormat(QPrinter::NativeFormat);
//    printer.setOutputFileName("../fotos/ejemplo.pdf");
//    printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat

    QPainter painterImpresora(&printer); // Es el painter para la impresora
    QImage im(archivoImagen);

    painterImpresora.drawImage(0, 0, im);
    painterImpresora.end();
}

void Galeria::slot_imprimir10x15()  {
    QPrinter printer(QPrinter::PrinterResolution);

//    printer.setPageSize(QPrinter::A6);  // Es la de 15x10
    printer.setPageSizeMM(QSizeF(100, 148));
    printer.setOrientation(QPrinter::Portrait);
    printer.setFullPage(false);  // Para utilizar margenes, quizas sea la mejor opcion. Hay que probar con la impresora
//    printer.setFullPage(true);  // Para que imprimir en la hoja completa

    printer.setOutputFormat(QPrinter::NativeFormat);

    QPainter painterImpresora(&printer); // Es el painter para la impresora

    QImage imPlantilla;
    if ( ! this->filePlantilla.isEmpty() )  {
        bool isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "JPG");
        qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo JPG ???" << isCargado;

        if ( isCargado == false )  {
            isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "PNG");
            qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo PNG ???" << isCargado;
        }
    }

    // Dibujala plantilla en la impresora
    imPlantilla = imPlantilla.scaled(printer.width(), printer.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painterImpresora.drawImage(0, 0, imPlantilla);

    QImage im(archivoImagenOriginal);

    /// La medidas son:
    /// La foto debe imprimirse comenzando a 1/4 del largo del papel ( printer.height() / 4 )
    /// La foto esta a 1/15 de cada borde del papel ( printer.width()/15 de cada lado )
    /// Abajo de la foto debe sobrar 1/8 del papel ( printer.height() / 8 )
    /// Entonces el ancho de la foto debe ajustarse a ( printer.width() - 2*printer.width()/15 )

    int topFoto = printer.height() / 4;
    int anchoFoto = printer.width() - 2*printer.width()/15;
    int altoFoto = printer.height() - ( printer.height() / 4 ) - ( printer.height() / 8 );
//    int bottomFoto = printer.height() / 8;

    // Lo ajusta de ancho o de alto
    im = im.scaled(anchoFoto, altoFoto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painterImpresora.drawImage(printer.width()/2 - im.width()/2, topFoto, im);

    painterImpresora.end();
}


void Galeria::slot_imprimir15x21()  {
    QPrinter printer(QPrinter::PrinterResolution);

    printer.setPageSizeMM(QSizeF(149, 211));
    printer.setOrientation(QPrinter::Portrait);
    printer.setFullPage(false);  // Para utilizar margenes, quizas sea la mejor opcion. Hay que probar con la impresora
//    printer.setFullPage(true);  // Para que imprimir en la hoja completa

    printer.setOutputFormat(QPrinter::NativeFormat);

    QPainter painterImpresora(&printer); // Es el painter para la impresora

    QImage imPlantilla;
    if ( ! this->filePlantilla.isEmpty() )  {
        bool isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "JPG");
        qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo JPG ???" << isCargado;

        if ( isCargado == false )  {
            isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "PNG");
            qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo PNG ???" << isCargado;
        }
    }

    // Dibujala plantilla en la impresora
    imPlantilla = imPlantilla.scaled(printer.width(), printer.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painterImpresora.drawImage(0, 0, imPlantilla);

    QImage im(archivoImagenOriginal);

    /// La medidas son:
    /// La foto debe imprimirse comenzando a 1/4 del largo del papel ( printer.height() / 4 )
    /// La foto esta a 1/15 de cada borde del papel ( printer.width()/15 de cada lado )
    /// Abajo de la foto debe sobrar 1/8 del papel ( printer.height() / 8 )
    /// Entonces el ancho de la foto debe ajustarse a ( printer.width() - 2*printer.width()/15 )

    int topFoto = printer.height() / 4;
    int anchoFoto = printer.width() - 2*printer.width()/15;
    int altoFoto = printer.height() - ( printer.height() / 4 ) - ( printer.height() / 8 );
//    int bottomFoto = printer.height() / 8;

    // Lo ajusta de ancho
    im = im.scaled(anchoFoto, altoFoto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painterImpresora.drawImage(printer.width()/2 - im.width()/2, topFoto, im);

    painterImpresora.end();
}

