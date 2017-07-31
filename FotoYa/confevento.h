#ifndef CONFEVENTO_H
#define CONFEVENTO_H

#include <QDialog>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QFileInfo>
#include <QList>

namespace Ui {
class ConfEvento;
}

class ConfEvento : public QDialog
{
    Q_OBJECT

public:
    explicit ConfEvento(QWidget *parent = 0);
    ~ConfEvento();

    QString getHashtag() const;
    void setHashtag(const QString &value);

    QString getFilePlantilla() const;
    QString getSizeHoja() const;

private:
    Ui::ConfEvento *ui;
    QString hashtag;

    QString filePlantilla;  // Es el nombre del archivo de la plantilla elegida
    QString sizeHoja;  // Es el tamano de hoja para las impresiones


private slots:
    void slot_aceptarCrearEvento();
    void slot_rechazarCrearEvento();
    void slot_cambioComboBoxDetectado();
    void slot_cambioLineEditDetectado();

};

#endif // CONFEVENTO_H
