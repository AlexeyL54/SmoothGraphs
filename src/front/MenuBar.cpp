#include "MenuBar.hpp"
#include "ThemeManager.hpp"
#include "qboxlayout.h"
#include "qnamespace.h"
#include "qpushbutton.h"
#include "qwidget.h"

MenuBar::MenuBar(QWidget *parent) : QWidget(parent) {
  mainLaout_ = new QVBoxLayout(this);
  mainLaout_->setContentsMargins(40, 0, 40, 0);
  mainLaout_->setSpacing(0);

  menuFrame_ = new QFrame();
  menuFrame_->setObjectName("menubar");

  buttonLayout_ = new QHBoxLayout(menuFrame_);
  buttonLayout_->setContentsMargins(40, 5, 40, 5);
  buttonLayout_->setSpacing(30);

  setButtons();
  setWrapAnimation();
  addButtons();

  mainLaout_->addWidget(menuFrame_);
};

void MenuBar::setButtons() {
  openBtn_ = new QPushButton("Открыть");
  openBtn_->setFixedSize(110, 40);

  saveBtn_ = new QPushButton("Сохранить");
  saveBtn_->setFixedSize(110, 40);

  // Создаем кнопку для выбора темы
  themeBtn_ = new QToolButton(this);
  themeBtn_->setText("Тема ▼");
  themeBtn_->setFixedSize(110, 40);
  themeBtn_->setPopupMode(QToolButton::InstantPopup);
  themeBtn_->setCursor(Qt::PointingHandCursor);

  // Создаем меню для кнопки
  QMenu *themeMenu = new QMenu(themeBtn_);

  QAction *lightAction = themeMenu->addAction("☀️ Светлая");
  lightAction->setData(Light);

  themeMenu->addSeparator(); // Разделитель для красоты

  QAction *darkAction = themeMenu->addAction("☁️ Тёмная");
  darkAction->setData(Dark);

  themeBtn_->setMenu(themeMenu);

  runBtn_ = new QPushButton();
  runBtn_->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  runBtn_->setFixedSize(110, 40);

  stopBtn_ = new QPushButton();
  stopBtn_->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  stopBtn_->setFixedSize(110, 40);

  wrapBtn_ = new QPushButton();
  wrapBtn_->setObjectName("wrapBtn");
  wrapBtn_->setFixedSize(30, 30);
  wrapBtn_->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  wrapBtn_->setCursor(Qt::PointingHandCursor);

  connect(wrapBtn_, &QPushButton::clicked, this, &MenuBar::wrapMenu);
}

void MenuBar::addButtons() {
  buttonLayout_->addWidget(openBtn_);
  buttonLayout_->addWidget(saveBtn_);
  buttonLayout_->addWidget(themeBtn_);
  buttonLayout_->addWidget(runBtn_);
  buttonLayout_->addWidget(stopBtn_);
  mainLaout_->addWidget(wrapBtn_, 0, Qt::AlignCenter);
}

void MenuBar::setWrapAnimation() {
  wrapAnimation_ = new QPropertyAnimation(menuFrame_, "maximumHeight");
  wrapAnimation_->setDuration(150);
  wrapAnimation_->setEasingCurve(QEasingCurve::InQuad);
  menuFrame_->setMaximumHeight(MAX_HEIGHT_);
}

void MenuBar::wrapMenu() {
  if (isExpanded_) {
    wrapAnimation_->setStartValue(menuFrame_->height());
    wrapAnimation_->setEndValue(0);
    wrapBtn_->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  } else {
    wrapAnimation_->setStartValue(menuFrame_->height());
    wrapAnimation_->setEndValue(MAX_HEIGHT_);
    wrapBtn_->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  }
  wrapAnimation_->start();
  isExpanded_ = !isExpanded_;
}

int MenuBar::maxHeight() { return MAX_HEIGHT_; }

MenuBar::~MenuBar() {};
