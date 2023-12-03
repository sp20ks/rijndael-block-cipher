#include "widget.h"
#include "ui_widget.h"
#include "file_cryptor.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
Widget::Widget( QWidget * parent )
    : QWidget( parent )
    , ui( new Ui::Widget )
{
    ui->setupUi( this );
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName( this, "Выберите файл", QDir::homePath() );
    if ( !file_name.isEmpty() )
    {
        ui->lineEdit_2->setText( file_name );
    } else
    {
        QMessageBox::critical( this, "Error", "Файл не был выбран." );
    };
}


void Widget::on_pushButton_2_clicked()
{
    if (ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty() )
    {
        QMessageBox::critical( this, "Error", "Все поля должны быть заполнены." );
    } else
    {
        AesCipher aesCipher;
        QString path = FileCryptor(ui->lineEdit_2->text(), aesCipher ).Encrypt( ui->lineEdit->text() );
        if ( path.isEmpty() )
        {
            QMessageBox::critical( this, "Error", "Ошибка шифрования." );
        } else
        {
            QMessageBox::information( this, "Success", "Зашифрованный файл находится по пути "  + path );
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
        };
    };
};


void Widget::on_pushButton_3_clicked()
{
    if (ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty() )
    {
        QMessageBox::critical( this, "Error", "Все поля должны быть заполнены." );
    } else
    {
        AesCipher aesCipher;
        QString path = FileCryptor( ui->lineEdit_2->text(), aesCipher ).Decrypt( ui->lineEdit->text() );
        if ( path.isEmpty() )
        {
            QMessageBox::critical( this, "Error", "Ошибка дешифрования." );
        } else
        {
            QMessageBox::information( this, "Success", "Дешифрованный файл находится по пути "  + path );
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
        };
    };
}

