#ifndef PROYECTADOR_H
#define PROYECTADOR_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QStringList>

namespace Ui {
class Proyectador;
}

class Proyectador : public QWidget
{
    Q_OBJECT

public:
    explicit Proyectador(QWidget *parent = 0);
    ~Proyectador();
    void iniciar();
    void detener();

    void setHashtag(const QString &value);

private:
    Ui::Proyectador *ui;
    QImage im;
    QTimer *timerActualizarFoto;
    QTimer *timerGetListadoFotos;
    QString hashtag;
    QStringList fotos;
    int indexMostrandoFotos;  // Un index para ir avanzando una por una las fotos a mostrar
    int cantidadFotos;

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void slot_timerActualizarFoto();
    void slot_timerGetListadoFotos();
};

#endif // PROYECTADOR_H
