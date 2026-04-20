#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

/**
 * @brief Кастомный виджет - меню
 *
 * Структура компонента
 *
 *QVBoxLayout(Главный контейнер)
├── QFrame(Панель меню / Navbar)
│   └── QHBoxLayout
│       ├── QPushButton("Кнопка 1")
│       ├── QPushButton("Кнопка 2")
│       ├── QPushButton(Play Icon)
│       └── QPushButton(Stop Icon)
└── QPushButton(Кнопка - триггер для сворачивания)
 */
class MenuBar : public QWidget {
public:
  explicit MenuBar(QWidget *parent = nullptr);
  ~MenuBar();
  int maxHeight();

private:
  const int MAX_HEIGHT_ = 60;

  QVBoxLayout *mainLaout_ = nullptr;
  QFrame *menuFrame_ = nullptr;
  QHBoxLayout *buttonLayout_ = nullptr;

  QPushButton *openBtn_ = nullptr;
  QPushButton *saveBtn_ = nullptr;
  QPushButton *runBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *wrapBtn_ = nullptr;

  QPropertyAnimation *wrapAnimation_ = nullptr;

  bool isExpanded_ = true;

  void setButtons();
  void addButtons();
  void setStyle();
  void setWrapAnimation();
  void wrapMenu();
};
