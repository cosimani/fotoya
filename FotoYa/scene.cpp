#include "scene.h"

/**
 * Sitio de descarga: http://www.codecguide.com/download_kl.htm
 * Para Windows 10 se instalaron los codecs K-Lite_Codec_Pack_1205_Basic.exe con una instalacion personalizada
 * solo instalando los codecs de video (DirectShow video decoding filter). Tilde por defecto, LAV. En la instalacion
 * destilde todo para que no cree icons, etc.
 * Use Software decoding.
 * La instalacion quedo en: C:\Program Files (x86)\K-Lite Codec Pack
 *
 * Y no paso nada. Sigue sin funcionar el video
 *
 * Sigue tirando el mensaje:
 * C:\Program Files (x86)\Common Files\Adobe\DVA\COM\Rel1\ad2aacadec.dll
 * DirectShowPlayerService::doRender: Unresolved error code 80040266
 *
 * Instale tambien ac3filter_2_6_0b_lite.exe y nada
 *
 * Instale tambien media.player.codec.pack.v4.4.1.setup.exe y nada
 *
 * Bueno, por ahora la mejor solucion es pasar todos los videos al formato de Windows wmn
 * Se puede hacer con el programa Freemake Video Converter
 *
 *
 */

Scene::Scene( QWidget *parent ) : QGLWidget( parent ),
                                  congratsShowed( false ),
                                  isRaffling( false ),
                                  sceneTimer( new QTimer ),
                                  timerNuevaImagen( new QTimer ),
                                  videoCounter( 0 ),
                                  backgroundVideo( new Video() ),
                                  textures( new QVector< Texture * > ),
                                  winnerId( -1 ),
                                  winnerIds( new QVector< int >() ),
                                  state( 0 ),
                                  transitionIterator( 0 ),
                                  traslation( 0 ),
                                  traslationVariation( 0 ),
                                  translationSpeed( 0.0015 )
{
    this->setCursor( Qt::BlankCursor );

    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_updateScene() ) );
    connect( timerNuevaImagen, SIGNAL( timeout() ), SLOT( slot_mostrarNuevaImagen() ) );

    this->buscarVideos();

    if ( listaVideos.size() > 0 )  {
        QString videoUri = QDir::currentPath() + "/../videos/" + listaVideos.at(videoCounter);
        backgroundVideo->player->setMedia( QUrl::fromLocalFile( videoUri ) );

        backgroundVideo->player->setVolume( 0 );
    }

}

void Scene::setHashtag(const QString &value)  {
    hashtag = value;

    loadTextures();  // Estamos listos para buscar las fotos, ya que tenemos la cual es la carpeta
}

void Scene::iniciar()  {
    backgroundVideo->player->play();

    sceneTimer->start( 15 );

    // Este timer es cada cuanto quedan girando las fotos y cuanto queda presente una foto.
    // Es un timer para las dos cosas.
    timerNuevaImagen->start( 10000 );
}

void Scene::detener()  {
    sceneTimer->stop();
    timerNuevaImagen->stop();

    backgroundVideo->player->stop();
}

void Scene::loadTextures()
{
    // Se puede dar el intento de cargar fotos antes de setear el hashtag, por el timer. Se no se seteo, retornamos.
    if ( this->hashtag.isEmpty() )
        return;

    QString conPlantilla_o_sinPlantilla;
//    conPlantilla_o_sinPlantilla = "_imagenes/";
    conPlantilla_o_sinPlantilla = "/";

    QDir directory( "../fotos/" + this->hashtag + conPlantilla_o_sinPlantilla );
    QStringList fileFilter;
    fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";

    // QDir::Time los ordena por ultima modificacion
    QStringList imageFiles = directory.entryList( fileFilter, QDir::Files, QDir::Time );

    qDebug() << "imageFiles.size() <= textures->size()" << imageFiles.size() << textures->size();

    // Antes de cargar de nuevo las texturas, perguntamos si hay algun archivo nuevo.
    if ( imageFiles.size() <= textures->size() )
        return;

    textures->clear();

    qDebug() << "imageFiles" << imageFiles;

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        textures->append( new Texture( imageFiles.at( i ) ) );
        QString textureUri( "../fotos/" + this->hashtag + conPlantilla_o_sinPlantilla + imageFiles.at( i ) );

        textures->last()->image.load( textureUri );
        textures->last()->generateFromQImage();
    }

    int textureIndex = 0;
    for( int i = 0; i < ceil( textures->size() / 3 ); i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            glBindTexture( GL_TEXTURE_2D, textures->at( textureIndex )->id );
            textures->at( textureIndex )->x = -i * 3;
            textures->at( textureIndex )->y = j * 3;
            textureIndex++; if( textureIndex >= textures->size() ) break;
        }
    }
}

void Scene::buscarVideos()  {
    QDir directory( QDir::currentPath() + "/../videos/" );
    QStringList fileFilter;
    fileFilter << "*.wmv";

    listaVideos = directory.entryList( fileFilter, QDir::Files );
    qDebug() << listaVideos;
}

void Scene::initializeGL()
{
    initializeGLFunctions();

    glClearColor( 0, 0, 0, 0 );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );

    GLfloat lightAmbient[4]; lightAmbient[0] = 0.5f;  lightAmbient[1] = 0.5f;  lightAmbient[2] = 0.5f;  lightAmbient[3] = 1.0f;
    GLfloat lightDiffuse[4]; lightDiffuse[0] = 1.0f;  lightDiffuse[1] = 1.0f;  lightDiffuse[2] = 1.0f;  lightDiffuse[3] = 1.0f;
    GLfloat lightPosition[4];lightPosition[0]= 0.0f;  lightPosition[1]= 0.0f;  lightPosition[2]= 2.0f;  lightPosition[3]= 1.0f;

    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
    glLightfv( GL_LIGHT1, GL_POSITION,lightPosition ); glEnable( GL_LIGHT1 );

//    loadTextures();
}

void Scene::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluPerspective( 45, width / ( float )height, 1, 1000 );

    glMatrixMode( GL_MODELVIEW );
}

void Scene::paintGL()
{
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    ///##################################################################################################/// Video

    glLoadIdentity();

    if( backgroundVideo->player->state() == QMediaPlayer::StoppedState ) backgroundVideo->player->play();

    glEnable( GL_TEXTURE_2D );

        glBindTexture( GL_TEXTURE_2D, backgroundVideo->grabber->textureId );

        glBegin( GL_QUADS );

            glTexCoord2f( 0.0, 1.0 ); glVertex3f( - 480 * 1.4, - 270 * 1.4, - 900 );
            glTexCoord2f( 0.0, 0.0 ); glVertex3f( - 480 * 1.4, + 270 * 1.4, - 900 );
            glTexCoord2f( 1.0, 0.0 ); glVertex3f( + 480 * 1.4, + 270 * 1.4, - 900 );
            glTexCoord2f( 1.0, 1.0 ); glVertex3f( + 480 * 1.4, - 270 * 1.4, - 900 );

        glEnd();

    glDisable( GL_TEXTURE_2D );

    ///##################################################################################################/// Camara

    glLoadIdentity();

    glEnable( GL_TEXTURE_2D );

    float desplazamientoZdeCesar = 2.5f;

    /// Movimiento sobre la cinta
//    glTranslatef( -traslation + 7, 0, traslation );  // Del Emi
    glTranslatef( -traslation + 7, 0, traslation + desplazamientoZdeCesar );  // De Cesar
    /// La camara siempre centrada a la mitad de la altura
    glTranslatef( 0, -4, 0 );
    /// Rotamos la cinta 45 grados
    glRotatef( 45, 0, 1, 0 );

    /// Cuando pasamos la mitad de la cinta cambiamos el sentido de la aceleracion

    if( state == 0 )
    {
        traslation = 0;
        traslationVariation = 0;
        translationSpeed = 0.0015;

        transitionIterator = 0;

        isRaffling = false;

        state++;
    }

    if( state == 1 )
    {
        /// Nos movemos sobre la traslacion de la cinta
        traslation += traslationVariation;
        if( traslation < textures->size() / 3 ) traslationVariation += translationSpeed;
        else traslationVariation -= translationSpeed;

        isRaffling = false;
    }

    if( state == 2 )
    {
        /// Movimientos contrarios como para cargar la matriz identidad implicitamente

        float averages = 350; // Usar un numero par para que el alejamiento sea simetrico

        int zoomWinner = 8;
        if( transitionIterator < averages )
        {
            isRaffling = true;

            /// Rotar de vuelta
            glRotatef( ( -45 / averages ) * transitionIterator, 0, 1, 0 );

            /// Trasladar al ganador
            glTranslatef( ( ( textures->at( winnerId )->x - 1 ) / averages ) * transitionIterator,

                          ( ( -textures->at( winnerId )->y - 1 ) / averages ) * transitionIterator,

                          ( zoomWinner  / averages ) * transitionIterator );

            /// Trasladar al inicio
            glTranslatef( ( ( traslation - 7 ) / averages ) * transitionIterator,

                          ( 4 / averages ) * transitionIterator,

                          ( -traslation / averages ) * transitionIterator );


            if( transitionIterator < averages / 2 )
            {
                glTranslatef( 0, 0, ( -transitionIterator ) / ( float ) 2 );
            }
            else
            {
                glTranslatef( 0, 0, ( transitionIterator - averages )  / ( float ) 2 );
            }

            // glRotatef( -45, 0, 1, 0 );
            // glTranslatef( 0, 4, 0 );
            // glTranslatef( traslation - 5, 0, -traslation );

            transitionIterator++;
        }

        if( transitionIterator == averages )
        {
            glLoadIdentity();
//            glTranslatef( textures->at( winnerId )->x - 1,
//                          -textures->at( winnerId )->y - 1,
//                          zoomWinner + desplazamientoZdeCesar );  // Del Emi
            glTranslatef( textures->at( winnerId )->x - 1,
                          -textures->at( winnerId )->y - 1,
                          zoomWinner + desplazamientoZdeCesar );  // De Cesar
            isRaffling = false;

            if( !congratsShowed )
            {
                congratsShowed = true;
            }
        }
        else
        {
            congratsShowed = false;
        }
    }

    ///##################################################################################################///

    int textureIndex = 0;
    for( int i = 0; i < ceil( textures->size() / 3 ); i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            glBindTexture( GL_TEXTURE_2D, textures->at( textureIndex )->id );

            glBegin( GL_QUADS );

                glTexCoord2f( 0.0, 0.0 ); glVertex3f( ( i * 3 ),     ( j * 3 ),     -12 );
                glTexCoord2f( 0.0, 1.0 ); glVertex3f( ( i * 3 ),     ( j * 3 ) + 2, -12 );
                glTexCoord2f( 1.0, 1.0 ); glVertex3f( ( i * 3 ) + 2, ( j * 3 ) + 2, -12 );
                glTexCoord2f( 1.0, 0.0 ); glVertex3f( ( i * 3 ) + 2, ( j * 3 ),     -12 );

            glEnd();

            textureIndex++; if( textureIndex >= textures->size() ) break;
        }
    }

    glDisable( GL_TEXTURE_2D );

    ///##################################################################################################/// Fotos

    glFlush();
}

void Scene::keyPressEvent( QKeyEvent * e)
{
    if (e->key() == Qt::Key_Escape)  {
        this->close();
    }


}

void Scene::slot_updateScene()
{
    this->updateGL();
}

void Scene::slot_mostrarNuevaImagen()  {

    if( textures->size() < 3 ) return;  // Si hay menos de 3 imagenes, no muestra nada
    if( isRaffling ) return;  // Retorna si es que esta trayendo el elegido

    state++;
    if( state == 2 )
    {
        winnerId = rand() % ( textures->size() - ( textures->size() % 3 ) );

        if( winnerIds->size() == textures->size() - ( textures->size() % 3 ) )
        {
            winnerIds->clear();
        }

        while( winnerIds->contains( winnerId ) )
        {
            winnerId = rand() % ( textures->size() - ( textures->size() % 3 ) );
        }

        winnerIds->append( winnerId );

    }
    if( state == 3 )
    {
        videoCounter++;
        if( videoCounter >= listaVideos.size() ) videoCounter = 0;
        QString videoUri = QDir::currentPath() + "/../videos/" + listaVideos.at(videoCounter);
        backgroundVideo->player->setMedia( QUrl::fromLocalFile( videoUri ) );
        state = 0;

        loadTextures();  // Cargamos desde el disco las nuevas fotos que se hayan subido
    }
}
