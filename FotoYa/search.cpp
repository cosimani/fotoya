#include "search.h"
#include "ui_search.h"
#include <QDebug>
#include "adminDB.h"
#include "manager.h"
#include <QMessageBox>
#include <QWindow>
#include <QKeyEvent>

Search::Search(QWidget *parent) : QWidget(parent), ui(new Ui::Search), isOffline(false)  {
    ui->setupUi(this);

    confEvento = new ConfEvento;
    scene = new Scene;

    QIcon icon("../recursos/icon.jpg");
    this->setWindowIcon(icon);

    this->cargarEventosEnComboBox();

    connect(ui->pbCrearEvento, SIGNAL(clicked()), this, SLOT(slot_crearEventoBuscarFotos()));

    managerFoto = new QNetworkAccessManager(this);

    connect(managerFoto, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_descargaFotoFinalizada(QNetworkReply*)));

    timerScrollInstagram = new QTimer(this);
    timerScrollInstagram->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Instagram) )  // Para que no busque en Instagram
        connect(timerScrollInstagram, SIGNAL(timeout()), this, SLOT(slot_irFinalInstagram()));

    timerUpdateInstagram = new QTimer(this);
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateInstagram->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Instagram) )  // Para que no busque en Instagram
        connect(timerUpdateInstagram, SIGNAL(timeout()), this, SLOT(slot_updatePaginaInstagram()));

    timerScrollTwitter = new QTimer(this);
    timerScrollTwitter->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Twitter) )  // Para que no busque en Twitter
        connect(timerScrollTwitter, SIGNAL(timeout()), this, SLOT(slot_irFinalTwitter()));

    timerUpdateTwitter = new QTimer(this);
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateTwitter->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));  // timer web
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Twitter) )  // Para que no busque en Twitter
        connect(timerUpdateTwitter, SIGNAL(timeout()), this, SLOT(slot_updatePaginaTwitter()));

    timerScrollFacebook = new QTimer(this);
    timerScrollFacebook->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Facebook) )  // Para que no busque en Facebook
        connect(timerScrollFacebook, SIGNAL(timeout()), this, SLOT(slot_irFinalFacebook()));

    timerUpdateFacebook = new QTimer(this);
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateFacebook->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));  // timer web
    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Facebook) )  // Para que no busque en Facebook
        connect(timerUpdateFacebook, SIGNAL(timeout()), this, SLOT(slot_updatePaginaFacebook()));

    connect(ui->cbWebInstagram, SIGNAL(toggled(bool)), this, SLOT(slot_mostrarWebInstagram()));
    connect(ui->cbWebTwitter, SIGNAL(toggled(bool)), this, SLOT(slot_mostrarWebTwitter()));
    connect(ui->cbWebFacebook, SIGNAL(toggled(bool)), this, SLOT(slot_mostrarWebFacebook()));

    connect(ui->pbPublicador, SIGNAL(clicked()), this, SLOT(slot_abrirPublicador()));
    connect(ui->pbProyectar, SIGNAL(clicked()), this, SLOT(slot_mostrarProyectador()));

//    // Las siguientes lineas activan o desactivan de la interfaz los check boxes de los navegadores
//    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Instagram) )  ui->cbWebInstagram->setEnabled(true);
//    else  ui->cbWebInstagram->setEnabled(false);
//    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Twitter) )  ui->cbWebTwitter->setEnabled(true);
//    else  ui->cbWebTwitter->setEnabled(false);
//    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Facebook) )  ui->cbWebFacebook->setEnabled(true);
//    else  ui->cbWebFacebook->setEnabled(false);

//    connect(ui->galeria, SIGNAL(signal_indexFotoChanged(int)), ui->impresor, SLOT(slot_setIndexFoto(int)));
    connect(ui->impresor, SIGNAL(signal_fotoEnGaleria(QString)), ui->galeria, SLOT(slot_fotoEnGaleria(QString)));

    connect(ui->impresor, SIGNAL(signal_fotoOriginalEnGaleria(QString)),
            ui->galeria, SLOT(slot_fotoOriginalEnGaleria(QString)));

    connect(ui->leTimerScroll, SIGNAL(textChanged(QString)), this, SLOT(slot_actualizarTimerDesdeLineEdits()));
    connect(ui->leTimerWeb, SIGNAL(textChanged(QString)), this, SLOT(slot_actualizarTimerDesdeLineEdits()));

}

Search::~Search()  {
    delete ui;
}

/**
 * @brief Search::getIsOffline El atributo bool isOffline es para indicar que se utilizara sin internet,
 * todo off-line, cosa que no se requiera de internet y se pueda por ejemplo, cargar las tablas de todas
 * las fiestas a traves del combo box o bien, porder proyectar las fotos de alguna fiesta particular
 * en la segunda pantalla.
 * @return
 */
bool Search::getIsOffline() const  {
    return isOffline;
}

/**
 * @brief Search::setIsOffline Si se ingresa offline desactivamos el QLineEdit del hashtag, porque solo tendriamos
 * posibilidad de buscar hashtag de eventos ya creados.
 * @param value
 */
void Search::setIsOffline(bool value)  {
    isOffline = value;

    if (isOffline)  {  // acomoda todo para uso off-line
        ui->pbCrearEvento->setText("Ver Evento");
        ui->cbHashtags->setEnabled(true);
    }
    else  {  // acomoda todo para uso online
        ui->pbCrearEvento->setText("Crear Evento");
        ui->cbHashtags->setEnabled(false);
        ui->cbHashtags->setVisible(false);
    }
}

void Search::cargarEventosEnComboBox()  {
    QStringList hashtagList = AdminDB::getInstance()->getHashTags();

    ui->cbHashtags->addItems(hashtagList);
}

void Search::slot_descargaFotoFinalizada(QNetworkReply *reply)  {
    QImage imFoto = QImage::fromData(reply->readAll());

    if ( ! imFoto.isNull() )  {
        imFoto.save("../fotos/" + confEvento->getHashtag()  + "/" + reply->url().fileName());
        AdminDB::getInstance()->addFoto(confEvento->getHashtag(), reply->url());
    }
    else
        qDebug() << "El QImage es null. Se llamo a slot_descargaFotoFinalizada pero no se puedo crear un QImage";
}

// Se invoca con el boton de CrearEvento, recien luego de intentar una busqueda bloqueamos
// el QLineEdit para que no se busque otro hashtag, porque deberia ser una sola fiesta por notebook.
// Conectamos aqui por unica vez los QTimers a las actualizaciones de Scroll y actualizacion de pagina
void Search::slot_crearEventoBuscarFotos()  {

    ui->impresor->setEnabled(true);
    ui->pbPublicador->setEnabled(true);

    // Entra aca solo si esta online. En caso que se ingresa con el boton "Ingresar off-line", entocnes
    // no entrara a esta if y ningun QWebView buscara fotos.
    if ( ! isOffline )  {

        // Esto muestra la ventana de confEvento pero no retorna hasta que se haga Cancel u Ok. Esto tiene que ser asi
        // porque dentro de este metodo ya se requiere el valor de hashtag
        confEvento->exec();

        // Si entra a este if es porque no hay hashtag porque se cancelo en el confEvento. Directamente retornamos.
        if (confEvento->getHashtag().isEmpty())  {
            ui->leHashtagActual->clear();
            return;
        }
        else  {
            ui->leHashtagActual->setText(confEvento->getHashtag());
            ui->cbHashtags->setVisible(false);

            // Seteamos los dos valores que vienen de la configuracion del evento (la plantilla y el tamano de hoja)
            // Tambien se lo seteamos a impresor
            this->filePlantilla = confEvento->getFilePlantilla();
            this->sizeHoja = confEvento->getSizeHoja();
            ui->impresor->setFilePlantilla(this->filePlantilla);
            ui->impresor->setSizeHoja(this->sizeHoja);
            ui->galeria->setSizeHoja(this->sizeHoja);
            ui->galeria->setFilePlantilla(this->filePlantilla);
        }

        connect(Manager::webInstagram, SIGNAL(loadFinished(bool)), this, SLOT(slot_wvActualizadoInstagram(bool)));
        connect(Manager::webTwitter, SIGNAL(loadFinished(bool)), this, SLOT(slot_wvActualizadoTwitter(bool)));
        connect(Manager::webFacebook, SIGNAL(loadFinished(bool)), this, SLOT(slot_wvActualizadoFacebook(bool)));

        // Tambien conectamos una sola vez para detectar cambios en la pagina
        QWebFrame * frameInstagram = Manager::webInstagram->page()->mainFrame();
        connect(frameInstagram, SIGNAL(contentsSizeChanged(QSize)), this, SLOT(slot_contentChangeInstagram(QSize)));

        QWebFrame * frameTwitter = Manager::webTwitter->page()->mainFrame();
        connect(frameTwitter, SIGNAL(contentsSizeChanged(QSize)), this, SLOT(slot_contentChangeTwitter(QSize)));

        QWebFrame * frameFacebook = Manager::webFacebook->page()->mainFrame();
        connect(frameFacebook, SIGNAL(contentsSizeChanged(QSize)), this, SLOT(slot_contentChangeFacebook(QSize)));

        Manager::webInstagram->load(QUrl("https://www.instagram.com/explore/tags/" + confEvento->getHashtag()));
        Manager::webTwitter->load(QUrl("https://twitter.com/search?q=%23" + confEvento->getHashtag()));
        Manager::webFacebook->load(QUrl("https://www.facebook.com/hashtag/" + confEvento->getHashtag()));

        QDir dir("../fotos/" + confEvento->getHashtag());
        if ( ! dir.exists() )  {
            dir.setPath("../fotos/");
            dir.mkdir(confEvento->getHashtag());  // Creamos dir para la fotos
            dir.mkdir(confEvento->getHashtag() + "_impresiones");  // Creamos dir para las impresiones
            dir.mkdir(confEvento->getHashtag() + "_imagenes");  // Creamos dir para las imagenes
        }

        ui->impresor->setHashTag(confEvento->getHashtag());
        scene->setHashtag(confEvento->getHashtag());

        // Cada busqueda de hashtag produce una nueva fiesta. Por lo tanto, generamos directorio en disco y registros en la base
        AdminDB::getInstance()->hashTagBuscado(confEvento->getHashtag());

    }
    else  {  // Si entra a este else es porque estamos offline, todo lo hacemos con lo que se elija en el combo box

        confEvento->setHashtag(ui->cbHashtags->currentText());
        ui->leHashtagActual->setText(ui->cbHashtags->currentText());
        ui->impresor->setHashTag(ui->cbHashtags->currentText());
        scene->setHashtag(confEvento->getHashtag());
    }
}

// Solo actualizamos la pagina
void Search::slot_updatePaginaInstagram()  {

    // Este lo usamos para ir 5 veces haciael final y hacia arriba. pasado esto, y si no hay cambios en el tamano
    // de la pagina, entonces detenemos el ir arriba y al final, para activar el timer de actualizacion de pagina
    cantidadDeCiclosIrFinalIrArriba = 5;

    Manager::webInstagram->load(QUrl("https://www.instagram.com/explore/tags/" + confEvento->getHashtag()));
//    qDebug() << "SE carga en el wv la pagina de nueva";
}

void Search::slot_updatePaginaTwitter()  {
    Manager::webTwitter->load(QUrl("https://twitter.com/search?q=%23" + confEvento->getHashtag()));
}

void Search::slot_updatePaginaFacebook()  {
    Manager::webFacebook->load(QUrl("https://www.facebook.com/hashtag/" + confEvento->getHashtag()));
}

// Instagram tiene un boton Cargar Mas para presionar y que muestre mas fotos, pero despues el boton ya no se debe
// presionar mas para que muestre mas fotos. Se debe hacer scroll al igual que twitter
void Search::slot_irFinalInstagram()  {

    // Primer hacemos scroll
    QPoint posicionAntesDeDesplazar = Manager::webInstagram->page()->mainFrame()->scrollPosition();
//    qDebug() << posicionAntesDeDesplazar
//             << Manager::webInstagram->page()->mainFrame()->contentsSize()
//             << Manager::webInstagram->page()->viewportSize()
//             << Manager::webInstagram->page()->preferredContentsSize();

    int posicionYscroll = posicionAntesDeDesplazar.y();
    int altoVentanaVisible = Manager::webInstagram->page()->viewportSize().height();
    int altoPaginaCompleta = Manager::webInstagram->page()->mainFrame()->contentsSize().height();

    if ( posicionYscroll + altoVentanaVisible == altoPaginaCompleta )  {
        Manager::webInstagram->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
//        qDebug() << "posicionAntesDeDesplazar.y()" << posicionAntesDeDesplazar.y();

        cantidadDeCiclosIrFinalIrArriba--;
    }
    else  {
        Manager::webInstagram->page()->mainFrame()->scroll(0, Manager::webInstagram->page()->mainFrame()->contentsSize().height());
//        qDebug() << "Manager::webInstagram->page()->mainFrame()->contentsSize().height()" << Manager::webInstagram->page()->mainFrame()->contentsSize().height();
    }

    QWebFrame * frame = Manager::webInstagram->page()->mainFrame();
    QWebElement document = frame->documentElement();

    // Debemos buscar el siguiente tag. Para darle clic y que pueda mostrar mas fotos
//    <a href="/explore/tags/ffaquilmescordoba/?max_id=1143158359878693784"> Cargar mas </a>

    QWebElementCollection coleccion = document.findAll("a");

    foreach (QWebElement elemento, coleccion) {
        QString elementoCargasMas;

        if ( elemento.attribute("href").contains("explore/tags") )  {  // Encontramos el boton Cargas Mas
            elementoCargasMas = elemento.attribute("href");

            elemento.evaluateJavaScript("this.click()");  // Le hacemos clic
        }
    }

    if ( cantidadDeCiclosIrFinalIrArriba <= 0 )  {
        timerScrollInstagram->stop();
        timerUpdateInstagram->start();
    }

}

void Search::slot_irFinalTwitter()  {
    QPoint posicionAntesDeDesplazar = Manager::webTwitter->page()->mainFrame()->scrollPosition();
    Manager::webTwitter->page()->mainFrame()->scroll(0, Manager::webTwitter->page()->mainFrame()->contentsSize().height());

    if ( posicionAntesDeDesplazar == Manager::webTwitter->page()->mainFrame()->scrollPosition() )  {
        timerScrollTwitter->stop();
        timerUpdateTwitter->start();
//        qDebug() << "Se actualiza pagina Twitter";
    }
}

void Search::slot_irFinalFacebook()  {

    QPoint posicionAntesDeDesplazar = Manager::webFacebook->page()->mainFrame()->scrollPosition();
    Manager::webFacebook->page()->mainFrame()->scroll(0, Manager::webFacebook->page()->mainFrame()->contentsSize().height());

    if ( posicionAntesDeDesplazar == Manager::webFacebook->page()->mainFrame()->scrollPosition() )  {
        timerScrollFacebook->stop();
        timerUpdateFacebook->start();
//        qDebug() << "Se actualiza pagina facebook";
    }
}

void Search::slot_contentChangeInstagram(QSize)  {

    cuantasImagenesHastaAhoraInstagram = this->getImagenesInstagram().size();
//    qDebug() << "slot_contentChange Instagram - Se encontraron " + QString::number(cuantasImagenesHastaAhoraInstagram);

    timerScrollInstagram->start();  // Realiza scroll de la pagina cada tantos segundos
    timerUpdateInstagram->stop();

    cantidadDeCiclosIrFinalIrArriba = 5;
}

void Search::slot_contentChangeTwitter(QSize)  {

    cuantasImagenesHastaAhoraTwitter = this->getImagenesTwitter().size();
//    qDebug() << "slot_contentChange Twitter - Se encontraron " + QString::number(cuantasImagenesHastaAhoraTwitter);

    timerScrollTwitter->start();  // Realiza scroll de la pagina cada tantos segundos
    timerUpdateTwitter->stop();
}

void Search::slot_contentChangeFacebook(QSize)  {

    cuantasImagenesHastaAhoraFacebook = this->getImagenesFacebook().size();
//    qDebug() << "slot_contentChange facebook - Se encontraron " + QString::number(cuantasImagenesHastaAhoraFacebook);

    timerScrollFacebook->start();  // Realiza scroll de la pagina cada tantos segundos
    timerUpdateFacebook->stop();
}

void Search::slot_wvActualizadoInstagram(bool exito)  {

    if ( ! exito )  {
//        qDebug() << "No se cargo la pagina correctamente";
        return;
    }

    cuantasImagenesHastaAhoraInstagram = this->getImagenesInstagram().size();
//    qDebug() << " - Se encontraron " + QString::number(cuantasImagenesHastaAhoraInstagram);

    timerScrollInstagram->start();  // Realiza scroll de la pagina cada tantos segundos
}

void Search::slot_wvActualizadoTwitter(bool exito)  {

    if ( ! exito )  {
//        qDebug() << "No se cargo la pagina correctamente";
        return;
    }

    cuantasImagenesHastaAhoraTwitter = this->getImagenesTwitter().size();
//    qDebug() << " - Se encontraron " + QString::number(cuantasImagenesHastaAhoraTwitter);

    timerScrollTwitter->start();  // Realiza scroll de la pagina cada tantos segundos
}

void Search::slot_wvActualizadoFacebook(bool exito)  {

    if ( ! exito )  {
//        qDebug() << "No se cargo la pagina correctamente";
        return;
    }

    cuantasImagenesHastaAhoraFacebook = this->getImagenesFacebook().size();
//    qDebug() << " - Se encontraron " + QString::number(cuantasImagenesHastaAhoraFacebook);

    timerScrollFacebook->start();  // Realiza scroll de la pagina cada tantos segundos
}


QStringList Search::getImagenesInstagram()  {
    QNetworkRequest request;
    QStringList listaImagenes;

    QWebFrame * frame = Manager::webInstagram->page()->mainFrame();

    QWebElement document = frame->documentElement();

    QWebElementCollection coleccion = document.findAll("img");

    foreach (QWebElement elemento, coleccion) {
        if ( elemento.attribute("alt").contains(confEvento->getHashtag(), Qt::CaseInsensitive) )  {
            listaImagenes.append(elemento.attribute("src"));
        }
    }

    // Si el archivo ya existe directamente ni pedimos a internet
    for (int i=0 ; i<listaImagenes.size() ; i++)  {

        QUrl url(listaImagenes.at(i));

        if ( ! this->isFile(url.fileName()) )  {
            request.setUrl(url);
            managerFoto->get(request);
//            qDebug() << "se pide instagram " << listaImagenes.at(i);
        }
    }

    return listaImagenes;
}

QStringList Search::getImagenesTwitter()  {
    QNetworkRequest request;
    QStringList listaImagenes;

    QWebFrame * frame = Manager::webTwitter->page()->mainFrame();

    QWebElement document = frame->documentElement();
    QWebElementCollection coleccion = document.findAll("div[data-element-context=\"platform_photo_card\"]");

    foreach (QWebElement elemento, coleccion) {
        listaImagenes.append(elemento.attribute("data-image-url"));
    }

    // Si el archivo ya existe directamente ni pedimos a internet
    for (int i=0 ; i<listaImagenes.size() ; i++)  {

        QUrl url(listaImagenes.at(i));

        if ( ! this->isFile(url.fileName()) )  {
            request.setUrl(url);
            managerFoto->get(request);
//            qDebug() << "se pide twitter " << listaImagenes.at(i);
        }
    }

    return listaImagenes;
}

QStringList Search::getImagenesFacebook()  {
    QNetworkRequest request;
    QStringList listaImagenes;

    QWebFrame * frame = Manager::webFacebook->page()->mainFrame();

    QWebElement document = frame->documentElement();

    // Solo buscamos en la parte central de las publicaciones, es decir, descartamos los banners, publicidades, etc.
//    QWebElement contentArea = document.findFirst("div[id=\"contentArea\"]");
    QWebElement contentArea = document.findFirst("div[id=\"initial_browse_result\"]");

    // Luego las fotos estan dentro de un div id=initial_browse_result
    // Luego en un div data-bt={"session_id":"988bbe66d9d07a9d2bf423d171d316d5",
    //          "typeahead_sid":"","result_type":"browse_type_keyword","referrer":"unknown",
    //          "path":"\/hashtag\/facu1989","query_data":[],"experience_type":"grammar"}

    QWebElementCollection coleccion = contentArea.findAll("img");

    foreach (QWebElement elemento, coleccion) {
//        if ( elemento.attribute("alt").contains("Foto", Qt::CaseInsensitive) )  {  // antes usaba esto, lo cambiaron
        if ( elemento.attribute("width").toInt() > 50 && elemento.attribute("height").toInt() > 50 )  {
            listaImagenes.append(elemento.attribute("src"));
        }
    }

    // Si el archivo ya existe directamente ni pedimos a internet
    for (int i=0 ; i<listaImagenes.size() ; i++)  {

        QUrl url(listaImagenes.at(i));

        if ( ! this->isFile(url.fileName()) )  {
            request.setUrl(url);
            managerFoto->get(request);
//            qDebug() << "se pide facebook " << listaImagenes.at(i);
        }
    }

    return listaImagenes;
}


void Search::slot_mostrarWebInstagram()  {
    if ( ui->cbWebInstagram->isChecked() )
        Manager::webInstagram->show();
    else
        Manager::webInstagram->hide();
}

void Search::slot_mostrarWebTwitter()  {
    if ( ui->cbWebTwitter->isChecked() )
        Manager::webTwitter->show();
    else
        Manager::webTwitter->hide();
}

void Search::slot_mostrarWebFacebook()  {
    if ( ui->cbWebFacebook->isChecked() )
        Manager::webFacebook->show();
    else
        Manager::webFacebook->hide();
}

void Search::slot_abrirPublicador()  {
    // Para publicar detenemos los timers de actualizacion de intagram, facebook y twitter
    this->timerScrollFacebook->stop();
    this->timerScrollInstagram->stop();
    this->timerScrollTwitter->stop();
    this->timerUpdateFacebook->stop();
    this->timerUpdateInstagram->stop();
    this->timerUpdateTwitter->stop();

    publicador = new Publicador;
    publicador->setHashTag(confEvento->getHashtag());

    publicador->showMaximized();
}

void Search::slot_mostrarProyectador()  {

    if (ui->leHashtagActual->text().isEmpty())  {
        QMessageBox::information(this, "Elija evento", "Primero elija un evento para mostrar las fotos");
        return;
    }

    // Entra a este if cuando el proyector esta mostrando las fotos.
    if ( ui->pbProyectar->text() == "Detener proyector" )  {
        scene->setHashtag(confEvento->getHashtag());
        scene->hide();
        scene->detener();
        ui->pbProyectar->setText("Proyectar");
    }
    else  {

        QDesktopWidget * desktopWidget = QApplication::desktop();
        if ( desktopWidget->screenCount() == 1 )  {  // Entra aca cuando hay un solo monitor
            QMessageBox::information(this, "No hay proyector", "No se detecta un proyector o segunda pantalla."
                                     " Igualmente se va ha mostrar");
            scene->setHashtag(confEvento->getHashtag());
            scene->iniciar();
            ui->pbProyectar->setText("Detener proyector");
            scene->show();
        }
        else  {  // Aca cuando hay 2 monitores
            scene->setHashtag(confEvento->getHashtag());
            scene->iniciar();
            ui->pbProyectar->setText("Detener proyector");

            // Estas tres lineas son necesarias para mostrarlo directamente en la segunda pantalla
            scene->show();
            scene->windowHandle()->setScreen(qApp->screens()[1]);
            scene->showFullScreen();
        }
    }

}

/**
 * @brief Search::slot_actualizarTimerDesdeLineEdits Se invoca cuando cambia el valor de alguno de los de QLineEdits
 */
void Search::slot_actualizarTimerDesdeLineEdits()  {

    timerScrollInstagram->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateInstagram->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));
    timerScrollTwitter->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateTwitter->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));  // timer web
    timerScrollFacebook->setInterval(qMax(5000, ui->leTimerScroll->text().toInt() * 1000));  // timer para scroll
    // timer para buscar de nuevo, a partir de que no se haga mas scroll
    timerUpdateFacebook->setInterval(qMax(10000, ui->leTimerWeb->text().toInt() * 1000));  // timer web

}

/**
 * @brief Search::closeEvent Para cerrar la aplicacion si cerramos la ventana principal y esta visible la segunda pantalla
 */
void Search::closeEvent(QCloseEvent *)  {
    if ( scene->isVisible() )  {
        scene->close();
    }
}

void Search::keyPressEvent(QKeyEvent *e)  {
    switch (e->key())  {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        break;

    case Qt::Key_Left:
        qDebug() << "izquierda";
        break;
    case Qt::Key_Right:
        qDebug() << "derecha";
        break;

    default:;
    }
}


bool Search::isFile(QString archivo)  {
    QDir dir("../fotos/" + confEvento->getHashtag());
    QStringList filters;
    filters << "*.jpg" << "*.png";
    dir.setNameFilters(filters);
    QStringList list = dir.entryList();

    return list.contains(archivo, Qt::CaseInsensitive);
}


