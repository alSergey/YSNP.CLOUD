#pragma once

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class settings;
}

class settings : public QDialog {
 Q_OBJECT

 public:
  explicit settings(QWidget *parent = nullptr);
  ~settings() override;
  void SetDirectory(const QString &dir);
  void SetLogin(const QString &name);
  bool CheckPass();
  QString GetNewPassword();
  QString GetOldPassword();
  void SendCanChange();
  void SendStopChange();

 signals:
  void change_directory_button_clicked();
  void change_password_button_clicked();
  void logout_button_clicked();
  void check_old_password_clicked();

 private slots:
  void on_btn_changeDirectory_clicked();
  void on_btn_changePassword_clicked();
  void on_btn_logout_clicked();
  void on_btn_checkOldPass_clicked();
  void on_txt_oldPass_textChanged();
  void on_txt_newPass_textChanged(const QString &arg1);
  void on_txt_newPassConfirm_textChanged(const QString &arg1);

 private:
  Ui::settings *ui;
  QString _oldPass;
  QString _newPass;
  QString _confirm;
};
