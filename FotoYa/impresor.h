#ifndef IMPRESOR_H
#define IMPRESOR_H

#include <QWidget>
#include <QTimer>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QList>
#include <QTreeWidgetItem>
#include <QSqlTableModel>
#include <QSqlRelation>
#include <QSqlRelationalTableModel>
#include <QPrinter>
#include <QPainter>
#include <QFileInfo>
#include <QRectF>
#include <QBrush>
#include <QColor>

namespace Ui {
class Impresor;
}

class Impresor : public QWidget
{
    Q_OBJECT

public:
    explicit Impresor(QWidget *parent = 0);
    ~Impresor();
    void setHashTag(QString hashtag);

    void setFilePlantilla(const QString &value);
    void setSizeHoja(const QString &value);

private:
    Ui::Impresor *ui;
    QTimer *timerDir, *timerImpresion;
    QStringList listaFotos;  // Lista de fotos en el directorio
    QString hashtag;

    QString filePlantilla;  // Es el nombre del archivo de la plantilla elegida
    QString sizeHoja;  // Es el tamano de hoja para las impresiones

    QSqlRelationalTableModel* tableModel;

    int selectedIndex;

private slots:
    void slot_timerDir();
    void slot_timerImpresion();
    void slot_updateTabla();
    void slot_itemChanged(const QModelIndex & current, const QModelIndex &);

public slots:
    void slot_setIndexFoto(int index);

signals:
    void signal_fotoEnGaleria(QString);
    void signal_fotoOriginalEnGaleria(QString);


};

#endif // IMPRESOR_H
