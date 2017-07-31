#ifndef PUBLICADOR_H
#define PUBLICADOR_H

#include <QWidget>
#include <QByteArray>
#include <QUrlQuery>
#include <QWebFrame>
#include <QWebElement>
#include <QWebPage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QImage>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDataStream>
#include <QFileInfo>

namespace Ui {
class Publicador;
}

class Publicador : public QWidget
{
    Q_OBJECT

public:
    explicit Publicador(QWidget *parent = 0);
    ~Publicador();
    void setHashTag(QString hashtag);

private:
    Ui::Publicador *ui;
    QNetworkAccessManager * manager;
    QString access_token, client_id, client_secret, redirect_uri, page_id, user_id, id_album;
    QString hashtag;
    void publicarProximaFoto();

private slots:
    void slot_loginFacebookCargado(bool);
    void slot_autenticar();
    void slot_autenticacionCargada(bool exito);
    void slot_envioCodeFinalizada(QNetworkReply *reply);
    void slot_crearAlbum();
    void slot_creacionAlbumFinalizada(QNetworkReply *reply);
    void slot_publicacionFotoFinalizada(QNetworkReply *reply);
};

#endif // PUBLICADOR_H
