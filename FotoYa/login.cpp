#include "login.h"
#include "ui_login.h"

#include <QPainter>
#include <QMouseEvent>
#include "manager.h"
#include <QIcon>
#include "adminDB.h"


Login::Login(QWidget *parent) : QWidget(parent), ui(new Ui::Login),
                                isTwitterListo(false), isFacebookListo(false)  {
    ui->setupUi(this);

    QIcon icon("../recursos/icon.jpg");
    this->setWindowIcon(icon);

    imLogo.load("../recursos/logo.png");

    configuracionGlobal = new ConfiguracionGlobal;

    manager = new QNetworkAccessManager(this);

//    Manager::webTwitter->load(QUrl("https://www.twitter.com"));
    Manager::webTwitter->load(QUrl("https://twitter.com/login"));
    ui->lInfoTwitter->setText("Accediendo...");

    Manager::webFacebook->load(QUrl("https://www.facebook.com"));
    ui->lInfoFacebook->setText("Accediendo...");

    connect(Manager::webTwitter, SIGNAL(loadFinished(bool)), this, SLOT(slot_loginTwitterCargado(bool)));
    connect(Manager::webFacebook, SIGNAL(loadFinished(bool)), this, SLOT(slot_loginFacebookCargado(bool)));

    connect(ui->leUsuarioTwitter, SIGNAL(returnPressed()), this, SLOT(slot_autenticar()));
    connect(ui->leClaveTwitter, SIGNAL(returnPressed()), this, SLOT(slot_autenticar()));
    connect(ui->leUsuarioFacebook, SIGNAL(returnPressed()), this, SLOT(slot_autenticar()));
    connect(ui->leClaveFacebook, SIGNAL(returnPressed()), this, SLOT(slot_autenticar()));
    connect(ui->pbIngresar, SIGNAL(clicked()), this, SLOT(slot_autenticar()));

    connect(ui->cbWebTwitter, SIGNAL(toggled(bool)), this, SLOT(slot_verWebTwitter()));
    connect(ui->cbWebFacebook, SIGNAL(toggled(bool)), this, SLOT(slot_verWebFacebook()));

    connect(ui->pbOffline, SIGNAL(clicked()), this, SLOT(slot_ingresarOffline()));

    connect(ui->pbConfiguracion, SIGNAL(clicked(bool)), configuracionGlobal, SLOT(show()));
}

Login::~Login()  {
    delete ui;
}

void Login::paintEvent(QPaintEvent *)  {
    QPainter painter(this);
    QImage im = imLogo;  // Copiamos para que no se pierda calidad si se redimensiona varias veces
    // Aunque en realidad no hace falta porque lo dejamos de tamano fijo a este Login
    im = im.scaled(this->width(), this->height());
    painter.drawImage(0, 0, im);
}


void Login::slot_loginTwitterCargado(bool exito)  {
    qDebug() << "login twitter cargado" << exito;
    if (exito)  {
        QWebFrame * frame = Manager::webTwitter->page()->mainFrame();
        QWebElement document = frame->documentElement();

        QWebElement usuario = document.findFirst("input[name=\"session[username_or_email]\"]");
        QWebElement clave = document.findFirst("input[name=\"session[password]\"]");

        if ( ! usuario.isNull() && ! clave.isNull() )  {  // Si se cumple es porque encontro el login
            ui->lInfoTwitter->setText("Login cargado!!");
            // Desconectamos porque es solo para obtener el login de Twitter
            disconnect(Manager::webTwitter, SIGNAL(loadFinished(bool)), this, SLOT(slot_loginTwitterCargado(bool)));

            connect(Manager::webTwitter, SIGNAL(loadFinished(bool)), this, SLOT(slot_verSiTwitterLogueado(bool)));

            ui->leUsuarioTwitter->setEnabled(true);
            ui->leClaveTwitter->setEnabled(true);

            if ( ui->leClaveFacebook->isEnabled() && ui->leUsuarioFacebook->isEnabled() )  {
                ui->pbIngresar->setEnabled(true);
                ui->leClaveTwitter->setFocus();
            }
        }
    }
    else  {
        QMessageBox::critical(this, "No se pudo contactar con Twitter", "Es probable sea el accesso a internet.");
        return;
    }
}


void Login::slot_loginFacebookCargado(bool exito)  {
    qDebug() << "login facebook cargado" << exito;
    if (exito)  {
        QWebFrame * frame = Manager::webFacebook->page()->mainFrame();
        QWebElement document = frame->documentElement();

        QWebElement usuario = document.findFirst("input[name=\"email\"]");
        QWebElement clave = document.findFirst("input[name=\"pass\"]");

        if ( ! usuario.isNull() && ! clave.isNull() )  {  // Si se cumple es porque encontro el login
            ui->lInfoFacebook->setText("Login cargado!!");

            // Desconectamos porque es solo para obtener el login de Twitter
            disconnect(Manager::webFacebook, SIGNAL(loadFinished(bool)), this, SLOT(slot_loginFacebookCargado(bool)));

            connect(Manager::webFacebook, SIGNAL(loadFinished(bool)), this, SLOT(slot_verSiFacebookLogueado(bool)));

            ui->leUsuarioFacebook->setEnabled(true);
            ui->leClaveFacebook->setEnabled(true);

            if ( ui->leClaveTwitter->isEnabled() && ui->leUsuarioTwitter->isEnabled() )  {
                ui->pbIngresar->setEnabled(true);
                ui->leClaveFacebook->setFocus();
            }
        }
    }
    else  {
        QMessageBox::critical(this, "No se pudo contactar con Faccebook", "Es probable sea el accesso a internet.");
        return;
    }
}


void Login::slot_autenticar()  {
    ui->pbIngresar->setEnabled(false);
    ui->leUsuarioTwitter->setEnabled(false);
    ui->leUsuarioFacebook->setEnabled(false);
    ui->leClaveTwitter->setEnabled(false);
    ui->leClaveFacebook->setEnabled(false);

    QWebFrame * frameTwitter = Manager::webTwitter->page()->mainFrame();
    QWebElement documentTwitter = frameTwitter->documentElement();

    QWebElement usuarioTwitter = documentTwitter.findFirst("input[class=\"js-username-field email-input js-initial-focus\"]");
    QWebElement claveTwitter = documentTwitter.findFirst("input[class=\"js-password-field\"]");

    usuarioTwitter.setAttribute("value", ui->leUsuarioTwitter->text());
    claveTwitter.setAttribute("value", ui->leClaveTwitter->text());

    QWebElement permitirTwitter = documentTwitter.findFirst("button[type=\"submit\"]");
    permitirTwitter.evaluateJavaScript("this.click()");
    ui->lInfoTwitter->setText("Credenciales enviadas!!");

    QWebFrame * frameFacebook = Manager::webFacebook->page()->mainFrame();
    QWebElement documentFacebook = frameFacebook->documentElement();

    QWebElement usuarioFacebook = documentFacebook.findFirst("input[name=\"email\"]");
    QWebElement claveFacebook = documentFacebook.findFirst("input[name=\"pass\"]");

    usuarioFacebook.setAttribute("value", ui->leUsuarioFacebook->text());
    claveFacebook.setAttribute("value", ui->leClaveFacebook->text());

    QWebElement permitirFacebook = documentFacebook.findFirst("label[id=\"loginbutton\"]");
    permitirFacebook.evaluateJavaScript("this.click()");
    ui->lInfoFacebook->setText("Credenciales enviadas!!");
}

void Login::slot_ingresarOffline()  {
    ui->pbIngresar->setEnabled(false);
    ui->leUsuarioTwitter->setEnabled(false);
    ui->leUsuarioFacebook->setEnabled(false);
    ui->leClaveTwitter->setEnabled(false);
    ui->leClaveFacebook->setEnabled(false);

    emit signal_ingresarOffline();
}


void Login::slot_verSiTwitterLogueado(bool exito)  {

    if (exito)  {
        QWebFrame * frame = Manager::webTwitter->page()->mainFrame();
        QWebElement document = frame->documentElement();

        QWebElement usuario = document.findFirst("input[name=\"session[username_or_email]\"]");
        QWebElement clave = document.findFirst("input[name=\"session[password]\"]");
        QWebElement tagExtra = document.findFirst("input[id=\"search-query\"]");  // Es el cuadro de busqueda de twitter

        if ( usuario.isNull() && clave.isNull() && ! tagExtra.isNull() )  {  // Si se cumple estamos logueados
            disconnect(Manager::webTwitter, SIGNAL(loadFinished(bool)), this, SLOT(slot_verSiTwitterLogueado(bool)));

            this->isTwitterListo = true;
            qDebug() << "twitter logueado";
            ui->lInfoTwitter->setText("Logueado!!");

            if ( isTwitterListo && isFacebookListo )
                emit signal_todosLogueados();
        }
    }
}

void Login::slot_verSiFacebookLogueado(bool exito)  {
    qDebug() << "facebook exito " << exito;
    if (exito)  {
        QWebFrame * frame = Manager::webFacebook->page()->mainFrame();
        QWebElement document = frame->documentElement();

        QWebElement usuario = document.findFirst("input[name=\"email\"]");
        QWebElement clave = document.findFirst("input[name=\"pass\"]");

        QWebElement tagExtra = document.findFirst("input[name=\"q\"]");  // Es el cuadro de busqueda de facebook

        if ( usuario.isNull() && clave.isNull() && ! tagExtra.isNull() )  {  // Si se cumple estamos logueados
            disconnect(Manager::webFacebook, SIGNAL(loadFinished(bool)), this, SLOT(slot_verSiFacebookLogueado(bool)));

            this->isFacebookListo = true;
            qDebug() << "facebook logueado";
            ui->lInfoFacebook->setText("Logueado!!");

            if ( isTwitterListo && isFacebookListo )
                emit signal_todosLogueados();
        }
    }
}

void Login::slot_verWebTwitter()  {
    if ( ui->cbWebTwitter->isChecked() )  {
        Manager::webTwitter->show();
        Manager::webTwitter->move(530, 50);
    }
    else
        Manager::webTwitter->hide();
}

void Login::slot_verWebFacebook()  {
    if ( ui->cbWebFacebook->isChecked() )  {
        Manager::webFacebook->show();
        Manager::webFacebook->move(560, 20);
    }
    else
        Manager::webFacebook->hide();
}
