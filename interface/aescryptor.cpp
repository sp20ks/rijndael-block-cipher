#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QByteArray>
#include <QDateTime>
#include "aescryptor.h"
#include "aes/rijndael.hpp"

AESCryptor::AESCryptor( QString path )
{
    input_.setFileName(path);
    if ( !input_.open( QIODevice::ReadOnly ) )
    {
        QMessageBox error_msg;
        error_msg.setWindowTitle( "Error" );
        error_msg.setText( "Ошибка открытия файла." );
        error_msg.setIcon( QMessageBox::Critical );
        error_msg.exec();
    };
};

AESCryptor::~AESCryptor() {
    input_.close();
};

QString AESCryptor::Encrypt( QString key )
{
    QByteArray block;
    QByteArray buf;
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString outputPath = QDir::toNativeSeparators(QDir(QFileInfo(input_.fileName()).absolutePath()).filePath("encrypted_file_" + currentTime + "." + QFileInfo(input_).suffix()));
    QFile output_(outputPath);
    output_.open( QIODevice::WriteOnly );
    while ( !input_.atEnd() )
    {
        block = input_.read( 16 );
        while ( block.size() < 16 )
        {
            block.append( '\0' );
        };

        buf = QByteArray::fromStdString( aes::Encrypt( block.toStdString(), key.toStdString() ) );
        output_.write( buf );
    };
    output_.close();
    return outputPath;
};

QString AESCryptor::Decrypt( QString key )
{
    QByteArray block;
    QByteArray buf;
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString outputPath = QDir::toNativeSeparators(QDir(QFileInfo(input_.fileName()).absolutePath()).filePath("decrypted_file_" + currentTime + "." + QFileInfo(input_).suffix() ));
    QFile output_(outputPath);
    output_.open( QIODevice::WriteOnly );
    while ( !input_.atEnd() )
    {
        block = input_.read( 16 );
        while ( block.size() < 16 )
        {
            block.append( '\0' );
        };

        buf = QByteArray::fromStdString( aes::Decrypt( block.toStdString(), key.toStdString() ) );
        output_.write( buf );
    };
    output_.close();
    return outputPath;
};
