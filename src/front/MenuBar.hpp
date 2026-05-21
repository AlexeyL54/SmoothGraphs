#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMainWindow>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

class MenuBar : public QWidget {
  Q_OBJECT

public:
  explicit MenuBar(QWidget *parent = nullptr);
  ~MenuBar();
  int maxHeight();

  // Геттеры для кнопок
  QPushButton *getOpenBtn() const { return openBtn_; }
  QToolButton *getSaveBtn() const { return saveBtn_; }
  QToolButton *getThemeBtn() const { return themeBtn_; }
  QPushButton *getRunBtn() const { return runBtn_; }
  QPushButton *getStopBtn() const { return stopBtn_; }

signals:
  void saveGraphRequested();
  void saveSolutionRequested();

private:
  const int MAX_HEIGHT_ = 60;

  QVBoxLayout *mainLaout_ = nullptr;
  QFrame *menuFrame_ = nullptr;
  QHBoxLayout *buttonLayout_ = nullptr;

  QPushButton *openBtn_ = nullptr;
  QToolButton *saveBtn_ = nullptr;
  QToolButton *themeBtn_ = nullptr;
  QPushButton *runBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *wrapBtn_ = nullptr;

  QPropertyAnimation *wrapAnimation_ = nullptr;

  bool isExpanded_ = true;

  void setButtons();
  void addButtons();
  void setWrapAnimation();
  void wrapMenu();
};
