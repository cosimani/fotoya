#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QImage>
#include <QIcon>
#include <QShowEvent>
#include <QColor>
#include <QVector>
#include <QFile>
#include <QWebView>
#include <QSize>
#include <QWebFrame>

#include <QByteArray>
#include <QUrlQuery>
#include <QMessageBox>
#include <QWebFrame>
#include <QWebElement>
#include <QWebPage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>
#include <QWebElementCollection>
#include <QRect>
#include <QSize>
#include <QTimer>
#include <QDir>
#include "publicador.h"
#include "confevento.h"
#include "proyectador.h"
#include <QDesktopWidget>
#include "scene.h"


namespace Ui {
class Search;
}

class Search : public QWidget
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = 0);
    ~Search();

    bool getIsOffline() const;
    void setIsOffline(bool value);

private:
    Ui::Search *ui;

    bool isOffline;
    QString hashtag;  // Almacena el hashtag actual

    QNetworkAccessManager * managerFoto;

    void cargarEventosEnComboBox();

    QStringList getImagenesInstagram();
    QStringList getImagenesTwitter();
    QStringList getImagenesFacebook();

    bool isFile(QString archivo);

    int cuantasImagenesHastaAhoraInstagram, cuantasImagenesHastaAhoraTwitter, cuantasImagenesHastaAhoraFacebook;
    QTimer * timerScrollInstagram, * timerUpdateInstagram;
    QTimer * timerScrollTwitter, * timerUpdateTwitter;
    QTimer * timerScrollFacebook, * timerUpdateFacebook;

    int cantidadDeCiclosIrFinalIrArriba;

    Publicador * publicador;
    ConfEvento * confEvento;
    Scene * scene;

    QString filePlantilla;  // Es el nombre del archivo de la plantilla elegida
    QString sizeHoja;  // Es el tamano de hoja para las impresiones

private slots:
    void slot_descargaFotoFinalizada(QNetworkReply *reply);
    void slot_crearEventoBuscarFotos();

    void slot_wvActualizadoInstagram(bool exito);
    void slot_contentChangeInstagram(QSize);

    void slot_wvActualizadoTwitter(bool exito);
    void slot_contentChangeTwitter(QSize);

    void slot_wvActualizadoFacebook(bool exito);
    void slot_contentChangeFacebook(QSize);

    void slot_irFinalInstagram();
    void slot_updatePaginaInstagram();
    void slot_irFinalTwitter();
    void slot_updatePaginaTwitter();
    void slot_irFinalFacebook();
    void slot_updatePaginaFacebook();


    void slot_mostrarWebInstagram();
    void slot_mostrarWebTwitter();
    void slot_mostrarWebFacebook();

    void slot_abrirPublicador();

    void slot_mostrarProyectador();

    void slot_actualizarTimerDesdeLineEdits();

protected:
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *e);
};

#endif // SEARCH_H
