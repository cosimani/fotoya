#include "confevento.h"
#include "ui_confevento.h"
#include "adminDB.h"
#include <QMessageBox>
#include <QDebug>

ConfEvento::ConfEvento(QWidget *parent) : QDialog(parent), ui(new Ui::ConfEvento)  {
    ui->setupUi(this);

    // Carga los hashtag existentes por si el usuario quiere seguir actualizando alguno de ellos
    QStringList hashtagList;
    hashtagList += "Elejir un HashTag";
    hashtagList += AdminDB::getInstance()->getHashTags();
    ui->cbHashtags->addItems(hashtagList);

    // Buscamos todas las imagenes de plantilla y las cargamos en el combo box
    QDir dir("../plantillas/");
    if ( dir.exists() )  {  // Si o si existe, pero por las dudas
        QStringList filters;
        filters << "*.jpg" << "*.png";

        QFileInfoList listaFileInfoList = dir.entryInfoList(filters);
        QStringList listaPlantillas;

        for (int i=0 ; i<listaFileInfoList.size() ; i++)  {
            listaPlantillas += listaFileInfoList.at(i).fileName();
        }
        ui->cbPlantillas->addItems(listaPlantillas);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slot_aceptarCrearEvento()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(slot_rechazarCrearEvento()));

    connect(ui->leHashtag, SIGNAL(textChanged(QString)), this, SLOT(slot_cambioLineEditDetectado()));
    connect(ui->cbHashtags, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_cambioComboBoxDetectado()));
}

ConfEvento::~ConfEvento()  {
    delete ui;
}

QString ConfEvento::getHashtag() const  {
    return hashtag;
}

void ConfEvento::setHashtag(const QString &value)  {
    hashtag = value;
}

QString ConfEvento::getFilePlantilla() const
{
    return filePlantilla;
}

QString ConfEvento::getSizeHoja() const
{
    return sizeHoja;
}

/**
 * @brief ConfigEvento::slot_crearEvento Crea un evento y luego bloquea la creacion de nuevo. Se debera iniciar de
 * nuevo la aplicacion para poder hacer otro evento.
 */
void ConfEvento::slot_aceptarCrearEvento()  {

    // Si no hay ningun hashtag, entonces no hacemos nada
    if (ui->leHashtagActual->text().isEmpty())  {
        QMessageBox::information(this, "Elegir un HashTag", "Debe asegurarse que aparezca un HashTag actual. Intente otra vez");
    }
    else  {
        QString hashtag_lineedit = ui->leHashtagActual->text();  // Si le pone o no el numeral # es indistinto
        hashtag_lineedit.replace("#", "");

        this->setHashtag(hashtag_lineedit);
        this->filePlantilla = ui->cbPlantillas->currentText();

        qDebug() << "Archivo de plantilla elegida:" << this->filePlantilla;

        if ( ui->cb15x10->isChecked() )
            this->sizeHoja = "15x10";
        else
            this->sizeHoja = "21x15";

        ui->leHashtag->setEnabled(false);
        ui->buttonBox->setEnabled(false);
        ui->cbPlantillas->setEnabled(false);
    }
}

void ConfEvento::slot_rechazarCrearEvento()  {
    this->setHashtag("");
}

void ConfEvento::slot_cambioComboBoxDetectado()  {
    if ( ui->cbHashtags->currentIndex() == 0 )  // retornamos porque el primer item es solo "Elejir un HashTag"
        return;
    ui->leHashtagActual->setText(ui->cbHashtags->currentText());
}

void ConfEvento::slot_cambioLineEditDetectado()  {
    ui->leHashtagActual->setText(ui->leHashtag->text());
}
