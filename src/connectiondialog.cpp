/**
 * qjournalctl: A Qt-based GUI for systemd's journalctl command
 *
 * Copyright (c) 2016-2018 by Patrick Eigensatz <patrick.eigensatz@gmail.com>
 * Some rights reserved. See LICENSE.
 */


#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QMessageBox>
#include <QDir>

ConnectionDialog::ConnectionDialog(QWidget *parent, SSHConnectionSettings **settings, SSHConnectionSerializer *sshConnectionSerializer, bool createOnly) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    this->createOnly = createOnly;
    this->settings = settings;
    this->sshConnectionSerializer = sshConnectionSerializer;

    // Adjust keyfile path to standard ssh rsa key location
    ui->keyfileLineEdit->setText(QDir::homePath() + "/.ssh/id_rsa");

    // If we only want to use the Dialog to create a new connection, don't display the "Open" button
    if(createOnly){
        ui->openButton->hide();
        ui->saveOpenButton->setText("Save");
    }
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

SSHConnectionSettings *ConnectionDialog::generateConnectionSettingsFromData()
{
    QMessageBox *incompleteMessageBox = new QMessageBox(QMessageBox::Warning, "Incomplete data", "Please provide a correct hostname!");

    if(ui->hostnameLineEdit->text().trimmed() == ""){
        incompleteMessageBox->show();
        return nullptr;
    }

    unsigned int port = ui->portLineEdit->text().toUInt();
    if(port == 0)
        port = 22;

    if(port > 65635){
        incompleteMessageBox->setText("Please provide a correct port! The default port for ssh is 22.");
        incompleteMessageBox->show();
        return nullptr;
    }

    return new SSHConnectionSettings(ui->connectionNameLineEdit->text().trimmed(),
                                        ui->hostnameLineEdit->text().trimmed(),
                                        port,
                                        ui->usernameLineEdit->text().trimmed(),
                                        ui->keyfileLineEdit->text().trimmed(),
                                        ui->authKeyfileRadio->isChecked()
                                    );
}

void ConnectionDialog::on_openButton_clicked()
{
    // todo: Check for nullptr first
    // todo: free?
    *settings = generateConnectionSettingsFromData();
    close();
}

void ConnectionDialog::on_saveOpenButton_clicked()
{
    // todo: Check for nullptr first
    // todo: free?
    // check: has name set!

    SSHConnectionSettings *newSettings = generateConnectionSettingsFromData();
    if(newSettings == nullptr){
        return;
    }

    // Prepare the new connection details
    if(!createOnly){
        *settings = newSettings;
    }

    // Add the connection to the serializer
    sshConnectionSerializer->add(newSettings);
    close();
}


void ConnectionDialog::on_hostnameLineEdit_returnPressed()
{
    on_openButton_clicked();
}

void ConnectionDialog::on_usernameLineEdit_returnPressed()
{
    on_openButton_clicked();
}

void ConnectionDialog::on_authKeyfileRadio_toggled(bool checked)
{
    ui->keyfileLineEdit->setEnabled(checked);
}