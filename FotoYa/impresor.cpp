#include "impresor.h"
#include "ui_impresor.h"
#include "adminDB.h"

Impresor::Impresor(QWidget *parent) : QWidget(parent), ui(new Ui::Impresor), selectedIndex(0)  {
    ui->setupUi(this);

    timerDir = new QTimer(this);
    connect(timerDir, SIGNAL(timeout()), this, SLOT(slot_timerDir()));
    timerDir->setInterval(5000);

    timerImpresion = new QTimer(this);
    connect(timerImpresion, SIGNAL(timeout()), this, SLOT(slot_timerImpresion()));
    timerImpresion->setInterval(10000);  // Cada este tiempo tira una impresion
    // Busca en la base, ordena las fotos de mas viejas a mas nuevas y va eligiendo una por una para imprimir.
    // Imprime y coloca en la base como impreso

    ui->tabla->setSelectionBehavior(QAbstractItemView::SelectRows);

//    QItemSelectionModel *select = table->selectionModel();


//    currentChanged(const QModelIndex & current, const QModelIndex & previous)
//    selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)

}

Impresor::~Impresor()  {
    delete ui;
}

// Desde la clase Search se setea el hashtag para luego poder deducir el directorio de las fotos.
void Impresor::setHashTag(QString hashtag)  {
    this->hashtag = hashtag;
    timerDir->start();
    timerImpresion->start();
}

void Impresor::setFilePlantilla(const QString &value)
{
    filePlantilla = value;
}

void Impresor::setSizeHoja(const QString &value)
{
    sizeHoja = value;
}

/**
 * @brief Impresor::slot_itemChanged Se llama cada vez que se modifica la seleccion. Nos basaremos en esto para tomar el
 * nombre de la foto y mostrarla. Mostraremos la foto con plantilla siempre que ya se haya impreso, de lo contrario
 * mostraremos la foto original. Consultaremos el campo is_impreso directamente desde esta QTableView. Emitiremos una signal
 * para que se muestre la foto en el otro widget. En la signal enviaremos la path de la imagen que debe imprimir Galeria.
 * @param current
 */
void Impresor::slot_itemChanged(const QModelIndex &current, const QModelIndex &)  {
    int columnaTableView = 2;  // Esto corresponde a la ubicacion en la base de datos
    int columna_is_impreso = 5;  // Esto corresponde a la ubicacion en la base de datos
    QString nombreFoto = current.sibling(current.row(), columnaTableView).data().toString();
    QString is_impreso = current.sibling(current.row(), columna_is_impreso).data().toString();
    qDebug() << "is_impreso"  << is_impreso;

    QString conPlantilla = "_imagenes/";
    QString sinPantilla = "/";

    QString archivoImagen;

    if ( is_impreso == "1" )  {
        archivoImagen = "../fotos/" + this->hashtag + conPlantilla + nombreFoto;
    }
    else  {
        archivoImagen = "../fotos/" + this->hashtag + sinPantilla + nombreFoto;
    }

    emit signal_fotoEnGaleria(archivoImagen);
    emit signal_fotoOriginalEnGaleria("../fotos/" + this->hashtag + "/" + nombreFoto);

    // Esto es para mantener la fila seleccionada despues de que se actualice la QTableView. Ver slot_updateTabla()
    selectedIndex = current.row();

    qDebug() << "currentChanged" << archivoImagen;
}

// Es invocado para detectar cambios en el directorios de fotos
void Impresor::slot_timerDir()  {
    QStringList filters;
    filters << "*.jpg" << "*.png";
    QDir dir("../fotos/" + this->hashtag);
    dir.setNameFilters(filters);
    QStringList listaActual = dir.entryList();

    if ( listaFotos.size() != listaActual.size() )  {  // Comparamos segun el size pensando que alcanza para compararlos
        listaFotos = listaActual;
        this->slot_updateTabla();
    }
}

void Impresor::slot_timerImpresion()  {

    QString fotoParaImprimir = AdminDB::getInstance()->getFotoParaImprimir(this->hashtag);
    this->slot_updateTabla();

    // Si esta vacio es porque no hay foto para imprimir, o la carpeta esta vacia o se imprimieron todas, o inconsistencia
    if ( fotoParaImprimir.isEmpty() )
        return;

    QString file = "../fotos/" + this->hashtag + "/" + fotoParaImprimir;
    QFileInfo fileInfo(file);
    qDebug() << "se imprime" << fileInfo.baseName();

    QPrinter printer(QPrinter::PrinterResolution);

    if ( this->sizeHoja == "15x10" )
        printer.setPageSizeMM(QSizeF(100, 148));  // Es la de 15x10
    else
        printer.setPageSizeMM(QSizeF(149, 211));  // Es la de 21x15

    printer.setOrientation(QPrinter::Portrait);
//    printer.setPageMargins (15, 15, 15, 15, QPrinter::Millimeter);
    printer.setFullPage(false);  // Para utilizar margenes, quizas sea la mejor opcion. Hay que probar con la impresora
//    printer.setFullPage(true);  // Para que imprimir en la hoja completa

    if ( AdminDB::getInstance()->getConfiguracionGlobal(AdminDB::Imprimir) )  {  // Para que imprima
        printer.setOutputFormat(QPrinter::NativeFormat);
    }
    else   {  // Para que solo guarde en PDF
        printer.setOutputFileName("../fotos/" + this->hashtag + "_impresiones/" + fileInfo.baseName() + ".pdf");
        printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat
    }

    QImage papel(printer.width(), printer.height(), QImage::Format_ARGB32_Premultiplied);

    QPainter painterImage(&papel); // Es el painter para la imagen en disco y que sale proyectada
    QPainter painterImpresora(&printer); // Es el painter para la impresora

    QImage imPlantilla;
    if ( ! this->filePlantilla.isEmpty() )  {
        bool isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "JPG");
        qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo JPG ???" << isCargado;

        if ( isCargado == false )  {
            isCargado = imPlantilla.load("../plantillas/" + this->filePlantilla, "PNG");
            qDebug() << "Plantilla " << "../plantillas/" + this->filePlantilla << "  // Se cargo PNG ???" << isCargado;
        }
    }

    // Dibujala plantilla en la impresora
    painterImpresora.drawImage(QRectF(0, 0, printer.width(), printer.height()),
                               imPlantilla, QRectF(0, 0, imPlantilla.width(), imPlantilla.height()));

    // Dibujala plantilla en la imagen para el pproyector
    painterImage.drawImage(QRectF(0, 0, printer.width(), printer.height()),
                               imPlantilla, QRectF(0, 0, imPlantilla.width(), imPlantilla.height()));

    QImage im(file);

    /// La medidas son:
    /// La foto debe imprimirse comenzando a 1/4 del largo del papel ( printer.height() / 4 )
    /// La foto esta a 1/15 de cada borde del papel ( printer.width()/15 de cada lado )
    /// Abajo de la foto debe sobrar 1/8 del papel ( printer.height() / 8 )
    /// Entonces el ancho de la foto debe ajustarse a ( printer.width() - 2*printer.width()/15 )

    int topFoto = printer.height() / 4;
    int anchoFoto = printer.width() - 2*printer.width()/15;
    int altoFoto = printer.height() - ( printer.height() / 4 ) - ( printer.height() / 8 );
//    int bottomFoto = printer.height() / 8;

    // Lo ajusta de ancho o de alto
    im = im.scaled(anchoFoto, altoFoto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painterImpresora.drawImage(printer.width()/2 - im.width()/2, topFoto, im);
    painterImage.drawImage(printer.width()/2 - im.width()/2, topFoto, im);

    painterImpresora.end();
    painterImage.end();

    papel.save("../fotos/" + this->hashtag + "_imagenes/" + fileInfo.baseName() + ".jpg");
}

void Impresor::slot_updateTabla()  {
    // Porque se instancia cada vez que entra a este método el objeto tableModelProductos
//    delete garbageCollectorTableModelProductos;

    // Instanciamos el objeto pasando como segundo parametro el QSqlDatabase
    tableModel = new QSqlRelationalTableModel(this, AdminDB::getInstance()->getDB());

    // Hacemos esta copia de la dirección de memoria del puntero para poder hacerle un delete. Porque esto
    // está mal hecho esto. Se deber´+ia instanciar una sola vez. No lo hacemos asi ahora porque lleva a que
    // tengamos que replantear todo. Es porque en los constructores se instancian los objetos, y de acuerdo al
    // orden es que algunos objetos no se crean aún. En fin, hay que mejorar esto sí o sí.
    // El delete se hace en la primer línea de este método.
//    garbageCollectorTableModelProductos = tableModel;

    tableModel->setTable("fotos");  // Esta es la tabla de la base para visualizar y modificar
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // Esto muestra la descripcion del producto en lugar de su id.
    // Cuidado que el id en tabla producto es numérico, en item_venta debe ser numérico también
    tableModel->setRelation(1, QSqlRelation("fiestas", "id", "nombre"));

    QString filtro;
    filtro = "id_fiesta like '" + AdminDB::getInstance()->getIdFiesta(this->hashtag) + "'";
    tableModel->setFilter(filtro);

    // Hace el SELECT
    tableModel->select();

    // Son los títulos de los campos que se visualizarán en el widget.
    // Ojo con los números de las columnas. Recordar que más adelante se oculta la columna id.
    tableModel->setHeaderData(1, Qt::Horizontal, "Fiesta");
    tableModel->setHeaderData(2, Qt::Horizontal, "Archivo");
    tableModel->setHeaderData(3, Qt::Horizontal, "Impreso");

    // Seteamos el QSqlTableModel sobre el QTableView
    // Aca decimos que las consultas de tableModelCliente las muestre en nuestro tableViewProductos.
    ui->tabla->setModel(tableModel);

    // En lugar de remover la consulta de los id (usando tableModelProductos->removeColumn(0)),
    // Lo que hacemos es ocultar la columna a nivel Widget. Ya que nos hará falta ese dato.
    // Por ejemplo, para saber qué row queremos borrar, y lo hacemos borrando el registro con ese id.
    ui->tabla->setColumnHidden(0, true);
//    ui->tabla->setColumnHidden(1, true);
    ui->tabla->setColumnHidden(3, true);
    ui->tabla->setColumnHidden(4, true);

    // Ajusta el ancho de la celda con el texto en su interior. Para todas las columnas.
    ui->tabla->resizeColumnsToContents();

    ui->tabla->setColumnWidth(1, qMax(150, 20*ui->tabla->width()/100));
    ui->tabla->setColumnWidth(2, qMax(400, 60*ui->tabla->width()/100));
    ui->tabla->setColumnWidth(3, qMax(100, 20*ui->tabla->width()/100));

    // Este metodo slot_updateTabla se invoca frecuentemente porque puede haber nuevas fotos.
    // Limpiamos las conecxiones previas cosas que solo se conecte una sola vez. Igual creo que no hace falta.
    disconnect(ui->tabla->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
               this, SLOT(slot_itemChanged(const QModelIndex &, const QModelIndex &)));

    // Esta linea selecciona nuevamente la row que estaba seleccionada antes de actualizar la tabla
    ui->tabla->selectRow(selectedIndex);

    connect(ui->tabla->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(slot_itemChanged(const QModelIndex &, const QModelIndex &)));
}

void Impresor::slot_setIndexFoto(int index)  {
    ui->tabla->selectRow(index);

    qDebug() << ui->tabla->currentIndex();
//    qDebug() << ui->tabla->currentIndex().data(Qt::DisplayRole).toString();
//    qDebug() << ui->tabla->indexAt(QPoint(3, index)).data().toString() + "*";
//    qDebug() << ui->tabla->indexAt(QPoint(1, 3)).data().toString() + "----------------------------";

    QModelIndex modelIndex = ui->tabla->currentIndex();
    int columnaTableView = 2;
    QString nombreFoto = modelIndex.sibling(modelIndex.row(), columnaTableView).data().toString();

    qDebug() << nombreFoto;
}




