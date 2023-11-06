#ifndef AESCRYPTOR_H
#define AESCRYPTOR_H
#include <QFile>

class AESCryptor
{
    QFile input_;
public:
    AESCryptor( QString path );
    ~AESCryptor();
    QString Encrypt( QString key );
    QString Decrypt( QString key );
};

#endif // AESCRYPTOR_H
