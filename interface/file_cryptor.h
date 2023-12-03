#ifndef FILE_CRYPTOR_H
#define FILE_CRYPTOR_H
#include <QFile>
#include "aes/rijndael.hpp"

class FileCryptor
{
    QFile input_;
    ICryptographicAdapter & encoder_;
public:
    FileCryptor(QString path, ICryptographicAdapter & enc );
    ~FileCryptor();
    QString Encrypt( QString key );
    QString Decrypt( QString key );
};

#endif // FILE_CRYPTOR_H
