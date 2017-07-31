#ifndef CONFIGURACIONGLOBAL_H
#define CONFIGURACIONGLOBAL_H

#include <QWidget>
#include <QShowEvent>

namespace Ui {
class ConfiguracionGlobal;
}

class ConfiguracionGlobal : public QWidget
{
    Q_OBJECT

public:
    explicit ConfiguracionGlobal(QWidget *parent = 0);
    ~ConfiguracionGlobal();

private:
    Ui::ConfiguracionGlobal *ui;

protected:
    void showEvent(QShowEvent *);

private slots:
    void slot_actualizar();
};

#endif // CONFIGURACIONGLOBAL_H
