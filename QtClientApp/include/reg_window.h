#pragma once

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class reg_window;
}

class reg_window : public QDialog {
 Q_OBJECT

 public:
  explicit reg_window(QWidget *parent = nullptr);
  ~reg_window() override;
  QString GetName();
  QString GetPass();
  bool CheckPass();

 signals:
  void register_button_clicked2();
  void login_button_clicked2();

 private slots:
  void on_inp_login_textEdited(const QString &arg1);
  void on_inp_password_textEdited(const QString &arg1);
  void on_inp_confirmPassword_textEdited(const QString &arg1);
  void on_btn_register_clicked();
  void on_btn_login_clicked();

 private:
  Ui::reg_window *ui;
  QString _userName;
  QString _userPass;
  QString _confirmation;
};
