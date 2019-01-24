#include "novaembed.h"
#include "ui_novaembed.h"
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>
#include <QDir>
#include <QStatusBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QUrl>
#include <QtCore>
#include <QDesktopServices>
//#include <QHostInfo>
#include <QDirIterator>
#include <QNetworkInterface>
#include <QDialog>
#include <iostream>

extern  QString instpath;

QString fileName1, fileName2;

void NOVAembed::on_CodeBlock_pushButton_clicked()
{
    system("codeblocks &");
}

void NOVAembed::on_Meld_pushButton_clicked()
{
    system("meld &");
}

void NOVAembed::on_QtCreator_pushButton_clicked()
{
    system("qtcreator &");

}

void NOVAembed::on_SPIProg_pushButton_clicked()
{
    //std::cout << FileSystemName.toLatin1().constData() << std::flush;
    std::cout << "on_SPIProg_pushButton_clicked"<< std::flush;
    QFile scriptfile("/tmp/script");
    update_status_bar("Clean SPI flash");
    if ( ! scriptfile.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        update_status_bar("Unable to create /tmp/script");
        return;
    }
    if ( ui->Board_comboBox->currentText() != "N1")
        return;
    QTextStream out(&scriptfile);
    out << QString("#!/bin/sh\n");
    out << QString("cd "+instpath+"/Qt/NOVAembed/SPI_Programmer\n");
    out << QString("sudo rmmod ftdi_sio ; sleep 1; sudo LD_LIBRARY_PATH=libs:$LD_LIBRARY_PATH bin/Debug/SPI_Programmer -b > "+instpath+"/Logs/SPI_Programmer.log\n");
    out << QString("echo \"$?\" > /tmp/result\n");
    scriptfile.close();
    if ( run_script() == 0)
    {
        update_status_bar("SPI blanked");
    }
    else
    {
        update_status_bar("SPI error");
        return;
    }
    if ( ! scriptfile.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        update_status_bar("Unable to create /tmp/script");
        return;
    }
    out << QString("#!/bin/sh\n");
    out << QString("cd "+instpath+"/Qt/NOVAembed/SPI_Programmer\n");
    out << QString("sudo rmmod ftdi_sio ; sleep 1; sudo LD_LIBRARY_PATH=libs:$LD_LIBRARY_PATH bin/Debug/SPI_Programmer -w >> "+instpath+"/Logs/SPI_Programmer.log\n");
    out << QString("echo \"$?\" > /tmp/result\n");
    scriptfile.close();
    if ( run_script() == 0)
    {
        update_status_bar("SPI written");
    }
    else
    {
        update_status_bar("SPI error");
        return;
    }
}
