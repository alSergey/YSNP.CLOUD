#include "settings.h"
#include "./ui_settings.h"

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings) {
  ui->setupUi(this);
}

settings::~settings() {
  delete ui;
}

void settings::on_btn_changeDirectory_clicked() {
  emit change_directory_button_clicked();
}

void settings::on_txt_oldPass_textChanged() {
  if (ui->txt_oldPass->toPlainText().length() > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _oldPass = ui->txt_oldPass->toPlainText();
}

void settings::on_txt_newPass_textChanged(const QString &arg1) {
  if (arg1 > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _newPass = arg1;
}

void settings::on_txt_newPassConfirm_textChanged(const QString &arg1) {
  if (arg1 > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _confirm = arg1;
}

void settings::on_btn_changePassword_clicked() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Изменение пароля", "Вы действительно хотите изменить пароль?",
                                QMessageBox::Cancel | QMessageBox::Ok);
  if (reply == QMessageBox::Cancel) {
    return;
  }
  emit change_password_button_clicked();
}

void settings::on_btn_logout_clicked() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Выход", "Вы действительно хотите выйти?",
                                QMessageBox::Cancel | QMessageBox::Ok);
  if (reply == QMessageBox::Cancel) {
    return;
  }
  emit logout_button_clicked();
}

void settings::on_btn_checkOldPass_clicked() {
  emit check_old_password_clicked();
}

void settings::SetDirectory(const QString &dir) {
  ui->lbl_directory->setText(dir);
}

bool settings::CheckPass() {
  return (_newPass == _confirm);
}

QString settings::GetNewPassword() {
  return _newPass;
}

QString settings::GetOldPassword() {
  return _oldPass;
}

void settings::SendCanChange() {
  ui->txt_newPass->setEnabled(true);
  ui->txt_newPassConfirm->setEnabled(true);
  ui->btn_changePassword->setEnabled(true);
}

void settings::SendStopChange() {
  ui->txt_newPass->setEnabled(false);
  ui->txt_newPassConfirm->setEnabled(false);
  ui->btn_changePassword->setEnabled(false);
}

void settings::SetLogin(const QString &name) {
  ui->txt_login->setText(name);
}


