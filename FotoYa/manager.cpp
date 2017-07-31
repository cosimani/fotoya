#include "manager.h"
#include "adminDB.h"

QWebView * Manager::webInstagram = NULL;
QWebView * Manager::webTwitter = NULL;
QWebView * Manager::webFacebook = NULL;

Manager::Manager(QObject *parent) : QObject(parent)  {

    qDebug() << "La base se conecto" << AdminDB::getInstance()->conectar("../db/baseFotoYa.sqlite");

    // Si bien es estatico, lo creamos aca porque sino dice: QWidget: Must construct a QApplication before a QWidget
    Manager::webInstagram = new QWebView;
    Manager::webTwitter = new QWebView;
    Manager::webFacebook = new QWebView;

    search = new Search;

    login = new Login;
    connect(login, SIGNAL(signal_todosLogueados()), this, SLOT(slot_todosLogueados()));

    connect(login, SIGNAL(signal_ingresarOffline()), this, SLOT(slot_ningunoLogueado_todoOffline()));
}

Manager::~Manager()  {

}

void Manager::iniciar()  {
    login->move(0, 0);
    login->show();
//    Manager::webTwitter->show();
//    Manager::webFacebook->show();
}

void Manager::slot_todosLogueados()  {
    search->setIsOffline(false);
    login->hide();
    search->showMaximized();

    // Aca si algun checkbox para ver los navegadores se activo, entonces los cerramos a todos para ir a Search
    if ( webTwitter->isVisible() )
        webTwitter->hide();
    if ( webFacebook->isVisible() )
        webFacebook->hide();
}

void Manager::slot_ningunoLogueado_todoOffline()  {
    search->setIsOffline(true);
    login->hide();
    search->showMaximized();
}



