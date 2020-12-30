#include "reg_window.h"
#include "./ui_reg_window.h"

reg_window::reg_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reg_window) {
  ui->setupUi(this);
}

reg_window::~reg_window() {
  delete ui;
}

void reg_window::on_inp_login_textEdited(const QString &arg1) {
  if (arg1.length() > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _userName = arg1;
}

void reg_window::on_inp_password_textEdited(const QString &arg1) {
  if (arg1.length() > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _userPass = arg1;
}

void reg_window::on_inp_confirmPassword_textEdited(const QString &arg1) {
  if (arg1.length() > 30) {
    QMessageBox msg;
    msg.setText("Превышен лимит символов <=30");
    msg.exec();
  } else
    _confirmation = arg1;
}

void reg_window::on_btn_register_clicked() {
  if (!_userName.isEmpty() && !_userPass.isEmpty() && !_confirmation.isEmpty()) {
    emit register_button_clicked2();
  } else {
    QMessageBox msg;
    msg.setText("Заполните форму");
    msg.exec();
  }
}

void reg_window::on_btn_login_clicked() {
  emit login_button_clicked2();
}

QString reg_window::GetName() {
  return _userName;
}

QString reg_window::GetPass() {
  return _userPass;
}

bool reg_window::CheckPass() {
  return (_confirmation == _userPass);
}
