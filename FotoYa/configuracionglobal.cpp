#include "configuracionglobal.h"
#include "ui_configuracionglobal.h"
#include <QMessageBox>
#include <QDebug>

#include "adminDB.h"

ConfiguracionGlobal::ConfiguracionGlobal(QWidget *parent) : QWidget(parent), ui(new Ui::ConfiguracionGlobal)  {
    ui->setupUi(this);

    connect(ui->pbActualizar, SIGNAL(clicked(bool)), this, SLOT(slot_actualizar()));
}

ConfiguracionGlobal::~ConfiguracionGlobal()  {
    delete ui;
}

// Cuando este widget se muestra, entonces obtenemos los datos de la base y lo reflejamos en los combo boxes
void ConfiguracionGlobal::showEvent(QShowEvent *)  {
    QStringList valores = AdminDB::getInstance()->getConfiguracionGlobalTodos();

    // Si es 4 es porque tenemos valores de: instagram, twitter, facebbok y imprimir (en 0 y unos)
    if ( valores.size() == 4 )  {
        if ( valores.at(0) == "1" )  ui->cbInstagram->setChecked(true);
        else  ui->cbInstagram->setChecked(false);

        if ( valores.at(1) == "1" )  ui->cbTwitter->setChecked(true);
        else  ui->cbTwitter->setChecked(false);

        if ( valores.at(2) == "1" )  ui->cbFacebook->setChecked(true);
        else  ui->cbFacebook->setChecked(false);

        if ( valores.at(3) == "1" )  ui->cbImpresion->setChecked(true);
        else  ui->cbImpresion->setChecked(false);
    }
}

// Actualizamos la base y cerramos esta ventana tambien
void ConfiguracionGlobal::slot_actualizar()  {
    QString instagram;
    QString twitter;
    QString facebook;
    QString imprimir;

    if ( ui->cbInstagram->isChecked() )  instagram = "1";
    else  instagram = "0";

    if ( ui->cbTwitter->isChecked() )  twitter = "1";
    else  twitter = "0";

    if ( ui->cbFacebook->isChecked() )  facebook = "1";
    else  facebook = "0";

    if ( ui->cbImpresion->isChecked() )  imprimir = "1";
    else  imprimir = "0";

    bool exito = AdminDB::getInstance()->setConfiguracionGlobal(instagram, twitter, facebook, imprimir);

    if ( ! exito )  {
        QMessageBox::critical(this, "No se pudo actualizar", "Por problemas en la base de datos no se pudo, intente luego.");
    }

    this->hide();
}
