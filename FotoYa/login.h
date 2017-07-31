#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QWebView>
#include <QString>
#include <QByteArray>
#include <QUrlQuery>
#include <QMessageBox>
#include <QWebFrame>
#include <QWebElement>
#include <QWebPage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>
#include <QThread>
#include "configuracionglobal.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private:
    Ui::Login *ui;

    QNetworkAccessManager * manager;
    QImage imLogo;
    bool isTwitterListo, isFacebookListo;
    ConfiguracionGlobal * configuracionGlobal;

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void slot_loginTwitterCargado(bool);
    void slot_verSiTwitterLogueado(bool exito);

    void slot_loginFacebookCargado(bool);
    void slot_verSiFacebookLogueado(bool exito);

    void slot_autenticar();
    void slot_ingresarOffline();

    void slot_verWebTwitter();
    void slot_verWebFacebook();

signals:
    void signal_todosLogueados();
    void signal_ingresarOffline();
};

#endif // LOGIN_H
