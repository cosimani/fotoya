#include "adminDB.h"
#include <QDebug>

AdminDB* AdminDB::unica_instancia = NULL;

AdminDB::AdminDB(QObject *parent) : QObject(parent)  {
    qDebug() << QSqlDatabase::drivers();
    db = QSqlDatabase::addDatabase("QSQLITE");
}

AdminDB::~AdminDB()  {
    db.close();
}

bool AdminDB::conectar(QString archivoSqlite)  {
    db.setDatabaseName(archivoSqlite);

    if(db.open())
        return true;

    return false;
}

QSqlDatabase AdminDB::getDB()  {
    return db;
}

// Pone en 0 el campo activo de todos los registros que puedan haber
void AdminDB::desctivarFiestas()  {
    QSqlQuery query("SELECT * FROM fiestas WHERE activo = '1'", db);
    int index_id = query.record().indexOf("id");
    int index_nombre = query.record().indexOf("nombre");
    int index_fecha = query.record().indexOf("fecha");
    int index_activo = query.record().indexOf("activo");

    while (query.next()) {
        QString id = query.value(index_id).toString();
        QString nombre = query.value(index_nombre).toString();
        QString fecha = query.value(index_fecha).toString();
        QString activo = query.value(index_activo).toString();

        QSqlQuery queryUpdate(db);
        queryUpdate.exec("UPDATE fiestas SET activo='0' WHERE id='" + id +"'");

    }
}

QString AdminDB::getNombreFiesta()  {
    QString res;
    QSqlQuery query("SELECT * FROM fiestas WHERE activo = '1'", db);
    int index_nombre = query.record().indexOf("nombre");
    int index_fecha = query.record().indexOf("fecha");

    // Deberia entrar una sola vez aca
    while (query.next())  {

        QString nombre = query.value(index_nombre).toString();
        QString fecha = query.value(index_fecha).toString();

        res = nombre + " - " + fecha;
    }

    return res;
}

QString AdminDB::getIdFiesta()  {
    QString res;
    QSqlQuery query("SELECT * FROM fiestas WHERE activo = '1'", db);
    int index_id = query.record().indexOf("id");

    // Deberia entrar una sola vez aca
    while (query.next())  {
        res = query.value(index_id).toString();
    }

    return res;
}

QString AdminDB::getIdFiesta(QString hashtag)  {
    QSqlQuery query("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'", db);
    int index_id = query.record().indexOf("id");

    // Parece que en esta base SQLite no funcion el metodo size() para saber la cantidad de filas retornadas
    // Usamos entonces el sigueitne contador
    int contadorRows = 0;

    while (query.next())  {
        contadorRows++;
    }

    if ( contadorRows == 0 )  {  // No existe una fiesta con ese hashatg
        return QString("No existe");
    }
    else if ( contadorRows == 1 )  {
        query.first();  // Volvemos al unico registro

        return query.value(index_id).toString();
    }
    else  {  // entra aca si hay mas de 1. Esto seria inconsistencia, seguro no sucedera nunca
        qDebug() << "Hay mas de 1 hashtag iguales en la base";
        return QString();
    }
}


//
void AdminDB::updateData(QVector<QStringList> *)  {
    // Primero analizaremos la consistencia de los datos
    // 1- Todos los Tweets dentro del vector tienen el mismo id_fiesta?
//    QString idFiestaComparador;
//    bool ok = true;
//    for (int i=0 ; i<v->size() ; i++)  {
//        if ( i == 0 )
//            idFiestaComparador = v->at(i).getId_fiesta();
//        else  {
//            if ( idFiestaComparador != v->at(i).getId_fiesta() )
//                ok = false;
//        }
//    }

    // Extraemos todos los datos de la base para id_fiesta y lo almacenamos en un QVector<Tweet> para mejor manipulacion
//    QVector<Tweet> *vBase = new QVector<Tweet>;

    QSqlQuery query("SELECT * FROM fotos WHERE id_fiesta = '" + this->getIdFiesta() + "'", db);
//    qDebug() << query.lastError() << query.lastQuery();
//    int index_id = query.record().indexOf("id");
//    int index_filename_foto = query.record().indexOf("filename_foto");
//    int index_time1970 = query.record().indexOf("time1970");
//    int index_tweet = query.record().indexOf("tweet");
//    int index_media_url = query.record().indexOf("media_url");
//    int index_is_impreso = query.record().indexOf("is_impreso");
//    int index_id_tweet = query.record().indexOf("id_tweet");

    while (query.next())  {
//        QString id = query.value(index_id).toString();
//        QString filename_foto = query.value(index_filename_foto).toString();
//        QString time1970 = query.value(index_time1970).toString();
//        QString tweet = query.value(index_tweet).toString();
//        QString media_url = query.value(index_media_url).toString();
//        QString is_impreso = query.value(index_is_impreso).toString();
//        QString id_tweet = query.value(index_id_tweet).toString();

//        Tweet t;
//        t.setId(id);
//        t.setFilename_foto(filename_foto);
//        t.setTime1970(time1970);
//        t.setTweet(tweet);
//        t.setMedia_url(media_url);
//        t.setIs_impreso(is_impreso);
//        t.setId_tweet(id_tweet);

//        vBase->push_back(t);
    }

    // Hasta aca tenemos los datos leidos de la base y desde Twitter


//    if ( v->size() != vBase->size() )  {

//    }


    // Almacenamos en la base todos los Tweet que estan en internet, controlamos que no se duplique con id_str

//    for (int i=0 ; i<v->size() ; i++)  {
//        QSqlQuery querySelect(db);
//        querySelect.exec("SELECT * FROM fotos WHERE id_tweet = '" + v->at(i).getId_tweet() + "'");

//        // Esto hace que no se agregue el mismo tweet en la base
//        if ( ! querySelect.first() )  {
//            QSqlQuery query(db);
//            query.exec("INSERT INTO fotos (id_fiesta, filename_foto, time1970, tweet, media_url, is_impreso, id_tweet) "
//                       "VALUES ('" + this->getIdFiesta() + "', '" + v->at(i).getFilename_foto() + "', '" +
//                       v->at(i).getTime1970() + "', '" + v->at(i).getTweet() + "', '" + v->at(i).getMedia_url() + "', '" +
//                       v->at(i).getIs_impreso() + "', '" + v->at(i).getId_tweet() + "')");
//        }
    //    }
}

// Cada vez que se busca un hashtag, sea nuevo o ya existente. Se invoca a este metodo y este se encarga de ver si en
// la base ya existe este hashtag y se encarga de acomodar todo.
// Devuelve true cuando se asegura que el hashtag ya esta en la base
bool AdminDB::hashTagBuscado(QString hashtag)  {
    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'");

//    int index_id = querySelect.record().indexOf("id");
//    int index_nombre = querySelect.record().indexOf("nombre");
//    int index_fecha = querySelect.record().indexOf("fecha");

    // Parece que en esta base SQLite no funcion el metodo size() para saber la cantidad de filas retornadas
    // Usamos entonces el sigueitne contador
    int contadorRows = 0;

    while (querySelect.next())  {
        contadorRows++;
    }

    if ( contadorRows == 0 )  {  // No existe, entonces agregamos registro
        QDate fecha = QDate::currentDate();
        QString str_fecha = QString::number(fecha.day()) + "-" +
                QString::number(fecha.month()) + "-" +
                QString::number(fecha.year());

        QSqlQuery queryInsert(db);

        return queryInsert.exec("INSERT INTO fiestas (nombre, fecha, hashtag) VALUES ('Fiesta de "
                                + hashtag + "', '" + str_fecha + "', '" + hashtag + "')");
    }
    else if ( contadorRows == 1 )  {
        qDebug() << "Ya existe";
        return true;
    }
    else  {  // entra aca si hay mas de 1. Esto seria inconsistencia, seguro no sucedera nunca
        qDebug() << "Hay mas de 1 hashtag iguales en la base";
    }

    return false;
}

bool AdminDB::addFoto(QString hashtag, QUrl url)  {

    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'");

    int index_id = querySelect.record().indexOf("id");
//    int index_nombre = querySelect.record().indexOf("nombre");
//    int index_fecha = querySelect.record().indexOf("fecha");

    // Parece que en esta base SQLite no funcion el metodo size() para saber la cantidad de filas retornadas
    // Usamos entonces el sigueitne contador
    int contadorRows = 0;

    QString id;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        contadorRows++;
    }

    // Si se cumple es porque hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro
    // con el mismo hashtag
    if ( contadorRows != 1)  {
        qDebug() << "Hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro igual";
        return false;
    }
    else  {
        // Antes de agregar una registro de foto, verificamos si no existe ya.
        QSqlQuery querySelectFoto(db);
        querySelectFoto.exec("SELECT * FROM fotos WHERE filename_foto = '" + url.fileName() + "'");

        contadorRows = 0;

        while (querySelectFoto.next())  {
            contadorRows++;
        }

        if ( contadorRows == 0)  {
            unsigned int tiempo = QDateTime::currentDateTime().toTime_t();

            QSqlQuery queryInsert(db);
            return queryInsert.exec("INSERT INTO fotos (id_fiesta, filename_foto, url, time1970, is_impreso) VALUES ('"
                                    + id + "', '" + url.fileName() + "', '" + url.toString() +
                                    "', '" + QString::number(tiempo) + "', '0')");
        }
    }
    return false;
}

bool AdminDB::addIdAlbum(QString hashtag, QString id_album)  {

    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'");

    int index_id = querySelect.record().indexOf("id");

    // Parece que en esta base SQLite no funcion el metodo size() para saber la cantidad de filas retornadas
    // Usamos entonces el sigueitne contador
    int contadorRows = 0;

    QString id;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        contadorRows++;
    }

    // Si se cumple es porque hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro
    // con el mismo hashtag
    if ( contadorRows != 1)  {
        qDebug() << "Hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro igual";
        return false;
    }
    else  {
        // Antes de hacer UPDATE con este id_album a esta fiesta, verificamos si no existe ya.
        QSqlQuery querySelectFoto(db);
        querySelectFoto.exec("SELECT * FROM fiestas WHERE id_album = '" + id_album + "'");

        contadorRows = 0;

        while (querySelectFoto.next())  {
            contadorRows++;
        }

        if ( contadorRows == 0)  {
            QSqlQuery queryUpdate(db);
            return queryUpdate.exec("UPDATE fiestas SET id_album='" + id_album + "' WHERE hashtag = '" + hashtag + "'");
        }
    }
    return false;
}

QStringList AdminDB::getHashTags()  {
    QStringList hashtagList;
    QSqlQuery query("SELECT * FROM fiestas", db);
    int index_hashtag = query.record().indexOf("hashtag");

    // Aca se van agregando cada hashatg de cada fiesta en el QStringList
    while (query.next())  {
        QString hashtag = query.value(index_hashtag).toString();
        hashtagList += hashtag;
    }

    return hashtagList;
}

QStringList AdminDB::getFotos(QString hashtag)  {
    QString idFiesta = this->getIdFiesta(hashtag);

    QStringList fotos;
    QSqlQuery query("SELECT * FROM fotos WHERE id_fiesta = '" + idFiesta + "'", db);
    int index_filename_foto = query.record().indexOf("filename_foto");

    // Aca se van agregando cada hashatg de cada fiesta en el QStringList
    while (query.next())  {
        QString filename_foto = query.value(index_filename_foto).toString();
        fotos += filename_foto;
    }

    return fotos;
}

QString AdminDB::getFotoParaImprimir(QString hashtag)  {
    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'");

    int index_id = querySelect.record().indexOf("id");

    int contadorRows = 0;
    QString id;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        contadorRows++;
    }

    // Si se cumple es porque hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro
    // con el mismo hashtag
    if ( contadorRows != 1)  {
        qDebug() << "Hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro igual";
        return QString();
    }
    else  {
        // Buscamos la la foto mas antigua sin imprimir.
        QSqlQuery querySelectFoto(db);
        querySelectFoto.exec("SELECT * FROM fotos WHERE is_impreso = '0' AND id_fiesta = '" + id + "' ORDER BY time1970");

        int index_filename_foto = querySelectFoto.record().indexOf("filename_foto");

        // Lo hacemos con un if para que sea solo una foto. Siempre y cuando exita al menos una.
        if ( querySelectFoto.next() )  {
            QString archivoFoto = querySelectFoto.value(index_filename_foto).toString();

            QSqlQuery queryUpdate(db);
            queryUpdate.exec("UPDATE fotos SET is_impreso='1' WHERE filename_foto='" + archivoFoto + "'");

            return archivoFoto;
        }
        else  {
            return QString();
        }
    }
}


QString AdminDB::getFotoParaUpload(QString hashtag)  {
    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM fiestas WHERE hashtag = '" + hashtag + "'");

    int index_id = querySelect.record().indexOf("id");

    int contadorRows = 0;
    QString id;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        contadorRows++;
    }

    // Si se cumple es porque hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro
    // con el mismo hashtag
    if ( contadorRows != 1)  {
        qDebug() << "Hay inconsistencia, hay que agregar primero el registro a fiestas o hay mas de un registro igual";
        return QString();
    }
    else  {
        // Buscamos la foto mas antigua sin upload.
        QSqlQuery querySelectFoto(db);
        querySelectFoto.exec("SELECT * FROM fotos WHERE is_upload = '0' AND is_impreso = '1' "
                             "AND id_fiesta = '" + id + "' ORDER BY time1970");

        int index_filename_foto = querySelectFoto.record().indexOf("filename_foto");

        // Lo hacemos con un if para que sea solo una foto. Siempre y cuando exita al menos una.
        if ( querySelectFoto.next() )  {
            QString archivoFoto = querySelectFoto.value(index_filename_foto).toString();

            QSqlQuery queryUpdate(db);
            queryUpdate.exec("UPDATE fotos SET is_upload='1' WHERE filename_foto='" + archivoFoto + "'");

            return archivoFoto;
        }
        else  {
            return QString();
        }
    }
}

/**
 * @brief AdminDB::setConfiguracionGlobal En la base hay una tabla configuracion que solo tendra un unico registro para
 * almacenar la configuracion global, que tiene la posibilidad de habilitar o no la busqueda en instagram, twitter o
 * facebook (con 0 se deshabilita y con 1 se habilita). Tambien esta la posibilidad de tirar las impresiones a la
 * impresora por defecto (con 1) o mandarla solo al PDF (con 0).
 * @param instagram
 * @param twitter
 * @param facebook
 * @param imprimir
 * @return Devuelve true si se pudo setear la configuracion en la base
 */
bool AdminDB::setConfiguracionGlobal(QString instagram, QString twitter, QString facebook, QString imprimir)  {
    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM configuracion");

    int index_id = querySelect.record().indexOf("id");

    int contadorRows = 0;
    QString id;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        contadorRows++;
    }

    // Si no hay ningun registro, entonces se agrega el registro con los valores que se desean.
    if ( contadorRows == 0)  {

        QSqlQuery queryInsert(db);

        bool exito = queryInsert.exec("INSERT INTO configuracion (instagram, twitter, facebook, imprimir) VALUES ('"
                                      + instagram + "', '" + twitter + "', '" + facebook + "', '" + imprimir + "')");

        qDebug() << queryInsert.lastError();

        return exito;
    }

    // Si hay mas de uno, borramos todos los registros y seteamos los valores en un unico registro
    if ( contadorRows > 1)  {

        QSqlQuery queryTruncate(db);

        bool truncated = queryTruncate.exec("TRUNCATE TABLE configuracion");

        if ( truncated )  {
            QSqlQuery queryInsert(db);

            return queryInsert.exec("INSERT INTO configuracion (instagram, twitter, facebook, imprimir) VALUES ('"
                                    + instagram + "', '" + twitter + "', '" + facebook + "', '" + imprimir + "')");
        }
        else
            return false;
    }

    // Hay un unico registro, esto esta bien, entonces actualizamos los valores de ese registro
    if ( contadorRows == 1)  {
        QSqlQuery queryUpdate(db);

        return queryUpdate.exec("UPDATE configuracion SET instagram='" + instagram + "', twitter='" + twitter +
                         "', facebook='" + facebook +"', imprimir='" + imprimir + "' WHERE id='" + id + "'");
    }

    return false;
}

/**
 * @brief AdminDB::getConfiguracionGlobal Devuelve un QStringList con: instagram, twitter, facebook y imprimir (en 0 y unos)
 * @return
 */
QStringList AdminDB::getConfiguracionGlobalTodos()  {
    QStringList valores;

    QSqlQuery querySelect(db);
    querySelect.exec("SELECT * FROM configuracion");

    int index_id = querySelect.record().indexOf("id");
    int index_instagram = querySelect.record().indexOf("instagram");
    int index_twitter = querySelect.record().indexOf("twitter");
    int index_facebook = querySelect.record().indexOf("facebook");
    int index_imprimir = querySelect.record().indexOf("imprimir");

    int contadorRows = 0;
    QString id;
    QString instagram;
    QString twitter;
    QString facebook;
    QString imprimir;

    while (querySelect.next())  {
        id = querySelect.value(index_id).toString();
        instagram = querySelect.value(index_instagram).toString();
        twitter = querySelect.value(index_twitter).toString();
        facebook = querySelect.value(index_facebook).toString();
        imprimir = querySelect.value(index_imprimir).toString();

        contadorRows++;
    }

    // Si no hay registro devolvemos la configuracion por defecto
    if ( contadorRows==0 )  {
        valores << "1" << "1" << "0" << "0";
    }
    // Hay mas de uno, no sabriamos cual registro devolver. Borramos la tabla, devolvemos por defecto y despues
    // el metodo setConfiguracionGlobal se hara cargo de agregar un registro correctamente
    else if ( contadorRows > 1 )  {
        QSqlQuery queryTruncate(db);

        queryTruncate.exec("TRUNCATE TABLE configuracion");
        valores << "1" << "1" << "0" << "0";
    }
    else if ( contadorRows == 1 )  {  // Todo bien, devolvemos los valores que estan seteados

        valores << instagram << twitter << facebook << imprimir;
    }

    return valores;
}

/**
 * @brief AdminDB::getConfiguracionGlobal
 * @param servicio
 * @return Devuelve un bool del servicio particular
 */
bool AdminDB::getConfiguracionGlobal(Servicio servicio)  {
    QStringList valores = this->getConfiguracionGlobalTodos();

    // Si es 4 es porque tenemos valores de: instagram, twitter, facebbok y imprimir (en 0 y unos)
    if ( valores.size() == 4 )  {

        if ( servicio == Instagram )  {
            if ( valores.at(0) == "1" )  return true;
            else return false;
        }

        if ( servicio == Twitter )  {
            if ( valores.at(1) == "1" )  return true;
            else return false;
        }

        if ( servicio == Facebook )  {
            if ( valores.at(2) == "1" )  return true;
            else return false;
        }

        if ( servicio == Imprimir )  {
            if ( valores.at(3) == "1" )  return true;
            else return false;
        }
    }

    return false;
}

