#include "mytranslator.h"

MyTranslator::MyTranslator(QString iniFile)
{
    curLan = "En";
    qDebug()<<iniFile;
    configIniRead = new QSettings(iniFile, QSettings::IniFormat);
    configIniRead->setIniCodec("UTF-8");
}
