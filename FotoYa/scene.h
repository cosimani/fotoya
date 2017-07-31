#ifndef SCENE_H
#define SCENE_H

#define TEXTURES_FOLDER "../fotos/merylucas2015_imagenes"
#define DATABASE_NAME "fiesta-ubp-25"

#include "texture.h"
#include "video.h"

#include <QDir>
#include <cmath>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <GL/glu.h>
#include <QKeyEvent>
#include <QGLWidget>
#include <QMediaPlayer>
#include <QGLFunctions>
#include <QStringList>

class Scene : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

private:
    bool congratsShowed;
    bool isRaffling;

    QTimer *sceneTimer, *timerNuevaImagen;
    int videoCounter;
    Video *backgroundVideo;
    QVector< Texture * > *textures;
    int winnerId;
    QVector< int > *winnerIds;
    QStringList listaVideos;

    int state;
    int transitionIterator;

    float traslation;
    float traslationVariation;
    float translationSpeed;

    void loadTextures();

    void buscarVideos();

    QString hashtag;

public:
    Scene( QWidget *parent = 0 );
    void setHashtag(const QString &value);
    void iniciar();
    void detener();

protected:
    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();

    void keyPressEvent(QKeyEvent * e);

private slots:
    void slot_updateScene();
    void slot_mostrarNuevaImagen();
};

#endif // SCENE_H
