#include "MenuBar.hpp"
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
  setStyle();

  mainLaout_->addWidget(menuFrame_);
};

void MenuBar::setButtons() {
  openBtn_ = new QPushButton("Открыть");
  openBtn_->setFixedSize(100, 40);

  saveBtn_ = new QPushButton("Сохранить");
  saveBtn_->setFixedSize(100, 40);

  runBtn_ = new QPushButton();
  runBtn_->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  runBtn_->setFixedSize(100, 40);

  stopBtn_ = new QPushButton();
  stopBtn_->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  stopBtn_->setFixedSize(100, 40);

  wrapBtn_ = new QPushButton();
  wrapBtn_->setObjectName("wtapBtn");
  wrapBtn_->setFixedSize(30, 30);
  wrapBtn_->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  wrapBtn_->setCursor(Qt::PointingHandCursor);

  connect(wrapBtn_, &QPushButton::clicked, this, &MenuBar::wrapMenu);
}

void MenuBar::addButtons() {
  buttonLayout_->addWidget(openBtn_);
  buttonLayout_->addWidget(saveBtn_);
  buttonLayout_->addWidget(runBtn_);
  buttonLayout_->addWidget(stopBtn_);
  mainLaout_->addWidget(wrapBtn_, 0, Qt::AlignCenter);
}

void MenuBar::setStyle() {
  QString styleSheet = R"(
        #menubar {
            background-color: #2c3e50;
            border-radius: 15px; /* Круглые углы */
            padding: 5px;
        }

        #navbar QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            padding: 8px 15px;
            font-size: 14px;
            border-radius: 8px;
        }

        #navbar QPushButton:hover {
            background-color: #34495e;
        }

        #navbar QPushButton:pressed {
            background-color: #1abc9c;
        }

        /* Стиль кнопки сворачивания */
        #wrapBtn {
            background-color: #ecf0f1;
            border: 2px solid #bdc3c7;
            border-radius: 15px; /* Круглая кнопка */
        }
        
        #wrapBtn:hover {
            background-color: #bdc3c7;
        }
    )";

  this->setStyleSheet(styleSheet);
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
