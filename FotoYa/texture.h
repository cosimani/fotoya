#ifndef TEXTURE_H
#define TEXTURE_H

#include <QGLWidget>
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>

//using namespace cv;

// La clase textura sirve para tener en una sola estructura todos los datos
// Nombre, mat y id que es utilizado para graficar en la escena de OpenGL

class Texture : public QObject
{
    Q_OBJECT

public:

    // Con este nombre se identificara a la textura para las busquedas
    QString name;
    int x, y;

    // Este mat tiene que ser asignado previamente a usar la textura
    // Mat mat;

    QImage image;

    // El id se genera automaticamente, pero para usarlo antes se debe llamar a generateFromMat
    // O en su defecto, generar la textura de algun otro modo
    GLuint id;

    Texture( QString name = "", QObject *parent = 0 ) : QObject( parent ), name( name ), id( 0 )
    {
        // Automaticamente generamos un id para la textura, aunque todavia no se crea la textura.

        glGenTextures( 1, &id );
    }

    void generateFromMat()
    {
        // Esta funcion toma el mat correspondiente que tiene que ser cargado previamente en otra parte del programa
        // Luego crea la textura y la asocia con el id que habiamos definido primero.

//        glBindTexture( GL_TEXTURE_2D, id );
//        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//        glTexImage2D( GL_TEXTURE_2D, 0, GL_BGR, mat.cols, mat.rows, 1, GL_BGR, GL_UNSIGNED_BYTE, mat.data );
    }

    void generateFromQImage()
    {
        // Esta funcion toma la QImage correspondiente que tiene que ser cargada previamente en otra parte del programa
        // Luego crea la textura y la asocia con el id que habiamos definido primero.

        QImage glImage = QGLWidget::convertToGLFormat( image );

        glBindTexture( GL_TEXTURE_2D, id );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, glImage.width(), glImage.height(),
                      0, GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits() );
    }
};

#endif // TEXTURE_H
