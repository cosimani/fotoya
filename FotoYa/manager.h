#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QWebView>
#include "search.h"
#include "login.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();

    static QWebView * webInstagram;  // De este QWebView se obtienen todos los datos de Instagram
    static QWebView * webTwitter;  // De este QWebView se obtienen todos los datos de Twitter
    static QWebView * webFacebook;  // De este QWebView se obtienen todos los datos de Facebook

    void iniciar();

private:
    Search * search;
    Login * login;

private slots:
    void slot_todosLogueados();
    void slot_ningunoLogueado_todoOffline();

signals:

public slots:
};

#endif // MANAGER_H
