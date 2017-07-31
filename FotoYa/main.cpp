#include <QApplication>
#include <QNetworkProxyFactory>
#include <QWebSettings>
#include <QNetworkProxy>
#include <QDesktopWidget>
#include <QDebug>
#include <QRect>
#include <QSharedMemory>

#include "manager.h"
#include "publicador.h"

#include "scene.h"

int main(int argc, char ** argv)  {
    QApplication a(argc, argv);

    //---- Check for another instance code snippet ----

    // GUID : Generated once for your application
    // you could get one GUID here: http://www.guidgenerator.com/online-guid-generator.aspx
    QSharedMemory shared("1eecc105-3b51-4a05-ac38-a5d35abfff88");

    if( !shared.create( 512, QSharedMemory::ReadWrite) )  {
        // For a GUI application, replace this by :
        QMessageBox msgBox;
        msgBox.setText( "El programa ya estaba iniciado de antes." );
        msgBox.setIcon( QMessageBox::Critical );
        msgBox.exec();

        return 0;
    }
    //---- END OF Check for another instance code snippet ----


//    QRect screenres = QApplication::desktop()->screenGeometry(1/*screenNumber*/);
//    qDebug() << QPoint(screenres.x(), screenres.y());
//    qDebug() << screenres.width() << screenres.height();

//    QNetworkProxy proxy;
//    proxy.setType(QNetworkProxy::HttpProxy);
//    proxy.setHostName("192.168.6.253");
//    proxy.setPort(3128);
//    proxy.setUser("cosimani");
//    proxy.setPassword("c0simani");
//    QNetworkProxy::setApplicationProxy(proxy);

//    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QWebSettings * settings = QWebSettings::globalSettings();
    settings->setFontFamily(QWebSettings::StandardFont, "Times New Roman");
    settings->setFontSize(QWebSettings::DefaultFontSize, 12);

    Manager * manager = new Manager;
    manager->iniciar();

    return a.exec();
}
