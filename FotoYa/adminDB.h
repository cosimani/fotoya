#ifndef ADMINDB_H
#define ADMINDB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>
#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QUrl>

class AdminDB : public QObject  {
    Q_OBJECT

private:
    AdminDB(QObject * parent = 0);
    static AdminDB* unica_instancia;

public:
    ~AdminDB();

    enum Servicio  { Instagram, Twitter, Facebook, Imprimir };

    static AdminDB* getInstance()  {
        if(unica_instancia == NULL)
            unica_instancia = new AdminDB();

        return unica_instancia;
    }

public:
    bool conectar(QString archivoSqlite);
    QSqlDatabase getDB();

    void desctivarFiestas();
    QString getNombreFiesta();
    QString getIdFiesta();
    QString getIdFiesta(QString hashtag);
    void updateData(QVector<QStringList> *);
    bool hashTagBuscado(QString hashtag);
    bool addFoto(QString hashtag, QUrl url);
    bool addIdAlbum(QString hashtag, QString id_album);
    QStringList getHashTags();
    QStringList getFotos(QString hashtag);

    QString getFotoParaImprimir(QString hashtag);
    QString getFotoParaUpload(QString hashtag);

    bool setConfiguracionGlobal(QString instagram = "1",
                                QString twitter = "1",
                                QString facebook = "0",
                                QString imprimir = "0");

    QStringList getConfiguracionGlobalTodos();
    bool getConfiguracionGlobal(Servicio servicio);

private:
    QSqlDatabase db;

};

#endif // ADMINDB_H
