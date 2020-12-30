#include "auth_window.h"
#include "./ui_auth_window.h"

auth_window::auth_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::auth_window) {
  ui->setupUi(this);
}

auth_window::~auth_window() {
  delete ui;
}

void auth_window::on_inp_login_textEdited(const QString &arg1) {
  _username = arg1;
}

void auth_window::on_inp_password_textEdited(const QString &arg1) {
  _userpass = arg1;
}

void auth_window::on_btn_login_clicked() {
  if (!_username.isEmpty() && !_userpass.isEmpty()) {
    emit login_button_clicked();
  } else {
    QMessageBox msg;
    msg.setText("Заполните форму");
    msg.exec();
  }
}

void auth_window::on_btn_register_clicked() {
  emit register_button_clicked();
}

QString auth_window::GetLogin() {
  return _username;
}

QString auth_window::GetPass() {
  return _userpass;
}
