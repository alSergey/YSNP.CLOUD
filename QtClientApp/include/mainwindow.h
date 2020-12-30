#pragma once

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QModelIndex>
#include <QMovie>
#include <QSettings>
#include <QShortcut>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QStringListModel>
#include <QTreeView>
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include "App.h"
#include "AppExceptions.h"
#include "auth_window.h"
#include "reg_window.h"
#include "resourcePath.h"
#include "settings.h"
#include "structs/FileMeta.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public QStandardItem {
 Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;
  void Display();

 private:
  void parseVectorFiles();
  static std::string getFilePath(const FileMeta &file);
  std::string getAbsoluteFilePath(const FileMeta &file);
  FileMeta &getFile();
  int getFileId();
  static bool compareFileMeta(const FileMeta &x, const FileMeta &y);
  void updateFiles();
  void startLoadingLabel();
  void stopLoadingLabel();

 private:
  void callbackRefresh(const std::string &msg);
  void callbackOk(const std::string &msg);
  void callbackError(const std::string &msg);

 private:
  void printMsgBox();
  QString _msg;

  void printInfoBox(const QString &title, const QString &text);

 signals:
  void printMsgBoxSignal();
  void printLoadingLabel();

 private slots:
  void slotShortcutCMDR();

  void authorizeUser();
  void registerUser();
  void registerWindowShow();
  void authWindowShow();

  void logout();
  void changePassword();
  void checkOldPassword();

  void slotCustomMenuRequested(QPoint pos);
  void changeDirectory();
  void open_file();
  void save_file();
  void rename_file();
  void download_on_device();
  void delete_from_device();
  void delete_from_cloud();
  void view_properties();
  void onBtnAddFile();
  void onBtnSettings();
  void onBtnRefresh();
  static void onBtnAbout();

  void on_treeView_doubleClicked(const QModelIndex &index);

 private:
  Ui::MainWindow *ui;

  auth_window uiAuth;
  reg_window uiReg;
  settings uiSett;

  QString _username;
  QString _userpass;

  QShortcut *keyCMDR;
  QWidget *_settingsForm;
  QMovie *_movie;
  QStandardItemModel _cloudModel;
  std::vector<FileMeta> _filesInfo;
  App _app;
};
